/*
 * Disambigmain.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: ysun
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "DisambigDefs.h"
#include "DisambigEngine.h"
#include "DisambigFileOper.h"
#include "DisambigRatios.h"
#include "DisambigTraining.h"
#include "DisambigCluster.h"
#include "DisambigPostProcess.h"
#include "DisambigUtilities.h"
#include <cmath>
#include <cstring>
using std::list;
using std::string;
#include "Disambigmain.h"



namespace EngineConfiguration {
	using std::vector;
	using std::string;

	const string WORKING_DIR_LABEL = "WORKING DIRECTORY";
	const string SOURCE_CSV_LABEL = "ORIGINAL CSV FILE";
	const string NUM_THREADS_LABEL = "NUMBER OF THREADS";
	const string WHETHER_GENERATE_STABLE_TRAINING_SETS_LABEL = "GENERATE STABLE TRAINING SETS";
	const string WHETHER_USE_AVAILABLE_RATIOS_DATABASE_LABEL = "USE AVAILABLE RATIOS DATABASE";
	const string THRESHOLDS_LABEL = "THRESHOLDS";
	const string NECESSARY_ATTRIBUTES_LABEL = "NECESSARY ATTRIBUTES";
	const string WHETHER_ADJUST_PRIOR_BY_FREQUENCY_LABEL = "ADJUST PRIOR BY FREQUENCY";
	const string DEBUG_MODE_LABEL = "DEBUG MODE";
	const string NUMBER_OF_TRAINING_PAIRS_LABEL = "NUMBER OF TRAINING PAIRS";
	const string STARTING_ROUND_LABEL = "STARTING ROUND";
	const string STARTING_FILE_LABEL = "STARTING FILE";
	const string POSTPROCESS_AFTER_EACH_ROUND_LABEL = "POSTPROCESS AFTER EACH ROUND";

	string working_dir;
	string source_csv_file;
	unsigned int number_of_threads;
	bool generate_stable_training_sets;
	bool use_available_ratios_database;
	vector < double > thresholds;
	vector < string > involved_columns;
	bool frequency_adjustment_mode;
	bool debug_mode;
	unsigned int number_of_training_pairs;
	unsigned int starting_round;
	string previous_disambiguation_result;
	bool postprocess_after_each_round;
}

namespace BlockingConfiguration {
	using std::string;
	using std::vector;
	vector < BlockingConfiguration::cBlockingDetail > BlockingConfig;
	vector < string > active_similarity_attributes;
	const string ACTIVE_SIMILARITY_ATTRIBUTES_LABEL = "ACTIVE SIMILARITY ATTRIBUTES";
	std::auto_ptr < cBlocking_Operation > active_blocker_pointer;
	unsigned int firstname_cur_truncation = 0;
}



int BlockingConfiguration::config_blocking( const char * filename, const string & module_id ) {
	std::cout << std::endl;
	std::cout << "====================== " << module_id << " ===========================" << std::endl;
	std::cout << "Reading Blocking Configuration from " << filename << " ... ..." << std::endl;
	std::cout << std::endl;
	const char * delim = ":";
	const char * secondary_delim = ",";
	const char module_specifier = '[';
	const unsigned int delim_len = strlen(delim);
	const unsigned int secondary_delim_len = strlen(secondary_delim);
	std::ifstream infile(filename);
	if ( ! infile.good() ) {
		std::cout << "Blocking configuration file " << filename << " does not exist." << std::endl;
		return 1;
	}
	string linedata;
	size_t pos, prevpos;
	BlockingConfiguration::BlockingConfig.clear();

	while ( getline ( infile, linedata ) && linedata.find(module_id) == string::npos );
	if ( ! infile ) {
		std::cout << "Cannot find module id: " << module_id << std::endl;
		return 2;
	}

	do {
		getline ( infile, linedata );
		if ( linedata.find(module_specifier) != string::npos )
			break;
		if ( linedata.find(BlockingConfiguration::ACTIVE_SIMILARITY_ATTRIBUTES_LABEL) != string::npos )
			break;
		if ( ! infile ) {
			std::cout << "End of file." << std::endl;
			break;
		}
		pos = linedata.find(delim);
		if ( pos == string::npos )
			continue;

		string lhs (linedata.c_str(), pos );
		const string columnname = remove_headtail_space(lhs);
		prevpos = pos + delim_len ;
		pos = linedata.find(delim, prevpos);
		const string dataindexstr = remove_headtail_space( linedata.substr(prevpos, pos - prevpos) );
		prevpos = pos + delim_len ;
		pos = linedata.find(secondary_delim, prevpos);
		const string beginstr = remove_headtail_space( linedata.substr(prevpos, pos - prevpos) );
		prevpos = pos + secondary_delim_len ;
		pos = linedata.find(secondary_delim, prevpos);
		const string ncharstr = remove_headtail_space( linedata.substr(prevpos, pos - prevpos) );
		prevpos = pos + secondary_delim_len ;
		pos = linedata.find(secondary_delim, prevpos);
		const string forwardstr = remove_headtail_space( linedata.substr(prevpos, pos - prevpos) );


		BlockingConfiguration::cBlockingDetail temp;
		temp.m_columnname = columnname;
		temp.m_dataindex = atoi ( dataindexstr.c_str());
		temp.m_begin = atoi ( beginstr.c_str());
		temp.m_nchar = atoi (ncharstr.c_str());
		temp.m_isforward = ( forwardstr == "false" ) ? false : true;


		std::cout << "Column name = " << columnname
				<< ", Data index = " << temp.m_dataindex
				<< ", Beginning position = " << temp.m_begin
				<< ", Number of chars = " << temp.m_nchar
				<< ", Direction = " << ( temp.m_isforward ? "true" : "false")
				<< std::endl;

		if ( columnname == cFirstname::static_get_class_name() )
			BlockingConfiguration::firstname_cur_truncation = temp.m_nchar;

		BlockingConfiguration::BlockingConfig.push_back(temp);

	} while ( linedata.find(BlockingConfiguration::ACTIVE_SIMILARITY_ATTRIBUTES_LABEL) == string::npos);

	// now linedata contains similarity information.
	prevpos = 0;
	pos = linedata.find(delim);
	prevpos = pos + delim_len;
	BlockingConfiguration::active_similarity_attributes.clear();
	std::cout << "Similarity Profiles include :";
	do {
		pos = linedata.find(secondary_delim, prevpos);
		const string tempname = remove_headtail_space( linedata.substr(prevpos, pos - prevpos) );
		prevpos = pos + secondary_delim_len ;
		if ( ! tempname.empty()) {
			BlockingConfiguration::active_similarity_attributes.push_back(tempname);
			std::cout << tempname << ", ";
		}

	} while ( pos != string::npos );
	std::cout << std::endl;


	vector < const cString_Manipulator *> pstring_oper;
	vector < string > columns_for_blocking;
	vector < unsigned int > data_indice_for_blocking;
	for ( vector < BlockingConfiguration::cBlockingDetail >::const_iterator p = BlockingConfiguration::BlockingConfig.begin();
			p != BlockingConfiguration::BlockingConfig.end(); ++p ) {

		pstring_oper.push_back(p->m_psm);
		columns_for_blocking.push_back(p->m_columnname);
		data_indice_for_blocking.push_back(p->m_dataindex);

		cString_Truncate * q = dynamic_cast< cString_Truncate *>(p->m_psm);
		if ( q == NULL ) {
			std::cout << "------> ATTENTION: STRING OPERATOR OF " << p->m_columnname << " CANNOT BE DYNAMICALLY CAST. SKIP THIS OPERATION." << std::endl;
		}
		else {
			q->set_truncater( p->m_begin, p->m_nchar, p->m_isforward);
		}

	}
	std::auto_ptr < cBlocking_Operation> bptr (
			new cBlocking_Operation_Multiple_Column_Manipulate (pstring_oper, columns_for_blocking, data_indice_for_blocking));
	BlockingConfiguration::active_blocker_pointer = bptr;


	return 0;
}

bool EngineConfiguration::config_engine(const char * filename, std::ostream & os) {
	os << "Reading Engine Configuration from " << filename << " ... ..." << std::endl;
	const char * delim = "=";
	const unsigned int delim_len = strlen(delim);
	std::ifstream infile(filename);
	if ( ! infile.good() ) {
		std::cout << "Engine configuration file " << filename << " does not exist." << std::endl;
		return false;
	}
	string linedata;
	size_t pos;
	int pieces_of_information = 0;
	const int must_have_information = 13;
	while ( getline (infile, linedata )) {
		pos = linedata.find(delim);
		if ( pos == string::npos )
			continue;
		string lhs (linedata.c_str(), pos );
		const string clean_lhs = remove_headtail_space(lhs);
		string rhs ( linedata.c_str() + pos + delim_len );
		string clean_rhs = remove_headtail_space(rhs);
		if ( clean_lhs == EngineConfiguration::WORKING_DIR_LABEL ) {
			EngineConfiguration::working_dir = clean_rhs;
			os << EngineConfiguration::WORKING_DIR_LABEL << " : "
					<< EngineConfiguration::working_dir << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::SOURCE_CSV_LABEL) {
			EngineConfiguration::source_csv_file = clean_rhs;
			os << EngineConfiguration::SOURCE_CSV_LABEL << " : "
					<< EngineConfiguration::source_csv_file << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::STARTING_FILE_LABEL) {
			EngineConfiguration::previous_disambiguation_result = clean_rhs;
			os << EngineConfiguration::STARTING_FILE_LABEL << " : "
					<< EngineConfiguration::previous_disambiguation_result << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::NUM_THREADS_LABEL) {
			EngineConfiguration::number_of_threads = atoi(clean_rhs.c_str());
			os << EngineConfiguration::NUM_THREADS_LABEL << " : "
					<<  EngineConfiguration::number_of_threads << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::NUMBER_OF_TRAINING_PAIRS_LABEL){
			EngineConfiguration::number_of_training_pairs = atoi(clean_rhs.c_str());
			os << EngineConfiguration::NUMBER_OF_TRAINING_PAIRS_LABEL << " : "
					<<  EngineConfiguration::number_of_training_pairs << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::STARTING_ROUND_LABEL){
			EngineConfiguration::starting_round = atoi(clean_rhs.c_str());
			os << EngineConfiguration::STARTING_ROUND_LABEL << " : "
					<<  EngineConfiguration::starting_round << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::WHETHER_GENERATE_STABLE_TRAINING_SETS_LABEL ) {
			os << EngineConfiguration::WHETHER_GENERATE_STABLE_TRAINING_SETS_LABEL << " : ";
			if ( clean_rhs == "true" ) {
				EngineConfiguration::generate_stable_training_sets = true;
				os << " true ";
			}
			else if ( clean_rhs == "false") {
				EngineConfiguration::generate_stable_training_sets = false;
				os << " false ";
			}
			else
				throw cException_Other("Config Error: generate stable training sets.");
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::WHETHER_USE_AVAILABLE_RATIOS_DATABASE_LABEL ){
			os << EngineConfiguration::WHETHER_USE_AVAILABLE_RATIOS_DATABASE_LABEL << " : ";
			if ( clean_rhs == "true" ) {
				EngineConfiguration::use_available_ratios_database = true;
				os << " true ";
			}
			else if ( clean_rhs == "false") {
				EngineConfiguration::use_available_ratios_database= false;
				os << " false ";
			}
			else
				throw cException_Other("Config Error: use available ratios database");
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::POSTPROCESS_AFTER_EACH_ROUND_LABEL ){
			os << EngineConfiguration::POSTPROCESS_AFTER_EACH_ROUND_LABEL << " : ";
			if ( clean_rhs == "true" ) {
				EngineConfiguration::postprocess_after_each_round = true;
				os << " true ";
			}
			else if ( clean_rhs == "false") {
				EngineConfiguration::postprocess_after_each_round = false;
				os << " false ";
			}
			else
				throw cException_Other("Config Error: post processing");
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::WHETHER_ADJUST_PRIOR_BY_FREQUENCY_LABEL ){
			os << EngineConfiguration::WHETHER_ADJUST_PRIOR_BY_FREQUENCY_LABEL<< " : ";
			if ( clean_rhs == "true" ) {
				EngineConfiguration::frequency_adjustment_mode = true;
				os << " true ";
			}
			else if ( clean_rhs == "false") {
				EngineConfiguration::frequency_adjustment_mode = false;
				os << " false";
			}
			else
				throw cException_Other("Config Error: frequency adjustment mode");
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::DEBUG_MODE_LABEL ){
			os << EngineConfiguration::DEBUG_MODE_LABEL<< " : ";
			if ( clean_rhs == "true" ) {
				EngineConfiguration::debug_mode = true;
				os << " true ";
			}
			else if ( clean_rhs == "false") {
				EngineConfiguration::debug_mode = false;
				os << " false ";
			}
			else
				throw cException_Other("Config Error: debug mode");
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::THRESHOLDS_LABEL ) {
			const char * strdelim = ", ";
			const unsigned int strdelim_len = strlen(strdelim);
			size_t strpos = 0, strprevpos = 0;
			EngineConfiguration::thresholds.clear();
			os << EngineConfiguration::THRESHOLDS_LABEL << " : ";
			do {
				strpos = clean_rhs.find(strdelim, strprevpos);
				string temp = clean_rhs.substr(strprevpos, strpos - strprevpos );
				double t = atof(temp.c_str());
				if ( t <= 0 || t >= 1 )
					throw cException_Other("Config Error: thresholds");
				EngineConfiguration::thresholds.push_back(t);
				os << t << strdelim;
				strprevpos = strpos + strdelim_len;
			}
			while ( strpos != string::npos );
			os << std::endl;
		}
		else if ( clean_lhs == EngineConfiguration::NECESSARY_ATTRIBUTES_LABEL ) {
			const char * strdelim = ", ";
			const unsigned int strdelim_len = strlen(strdelim);
			size_t strpos = 0, strprevpos = 0;
			EngineConfiguration::involved_columns.clear();
			os << EngineConfiguration::NECESSARY_ATTRIBUTES_LABEL << " : ";
			do {
				strpos = clean_rhs.find(strdelim, strprevpos);
				string temp = clean_rhs.substr(strprevpos, strpos - strprevpos );
				EngineConfiguration::involved_columns.push_back(temp);
				os << temp << strdelim;
				strprevpos = strpos + strdelim_len;
			}
			while ( strpos != string::npos );
			os << std::endl;
		}
		else {
			continue;
		}

		++pieces_of_information;
	}
	if ( must_have_information == pieces_of_information )
		return true;
	else
		return false;
}

int main( int argc, char * argv[]) {
	std::cout << std::endl;
	std::cout << "====================== STARTING DISAMBIGUATION ===========================" << std::endl;
	std::cout << std::endl;

	Full_Disambiguation("/media/data/edwardspace/workplace/testcpp/Disambiguation/EngineConfig.txt",
			"/media/data/edwardspace/workplace/testcpp/Disambiguation/BlockingConfig.txt");

	std::cout << std::endl;
	std::cout << "====================== END OF DISAMBIGUATION =============================" << std::endl;
	std::cout << std::endl;
	return 0;
}




//******************************************************************************
//******************************************************************************
//******************************************************************************

int Full_Disambiguation( const char * EngineConfigFile, const char * BlockingConfigFile ) {
	if ( ! EngineConfiguration::config_engine(EngineConfigFile, std::cout ) )
		throw cException_Other("Engine Configuration is not complete!");

	const bool train_stable = EngineConfiguration::generate_stable_training_sets;
	const bool use_available_ratios = EngineConfiguration::use_available_ratios_database;
	const string working_dir = EngineConfiguration::working_dir;
	const string final_file = working_dir + "/final.txt";
	const vector < double > threshold_vec = EngineConfiguration::thresholds ;
	const unsigned int buff_size = 512;
	const unsigned int num_threads = EngineConfiguration::number_of_threads;
	const vector <string> column_vec = EngineConfiguration::involved_columns;
	const unsigned int limit = EngineConfiguration::number_of_training_pairs;
	bool frequency_adjust_mode = EngineConfiguration::frequency_adjustment_mode;
	bool debug_mode = EngineConfiguration::debug_mode;
	const unsigned int starting_round = EngineConfiguration::starting_round;


	list <cRecord> all_records;
	char filename2[buff_size];
	sprintf(filename2, "%s", EngineConfiguration::source_csv_file.c_str());
	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec);
	if (not is_success) return 1;


	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));
	cAssignee::configure_assignee(all_rec_pointers);

	//patent stable
	const string training_stable [] = { working_dir + "/xset03_stable.txt",
												working_dir + "/tset02_stable.txt" };
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	if ( train_stable )
		make_stable_training_sets_by_personal ( all_records, limit, training_stable_vec);

	map <string, const cRecord *> uid_dict;
	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	//train patent info
	cRatioComponent patentinfo(uid_dict, string("Patent") );

	list < const cRecord * > record_pointers;

	bool matching_mode = true;

	cCluster_Info match ( uid_dict, matching_mode, frequency_adjust_mode, debug_mode);
	match.set_thresholds( threshold_vec);

	char xset01[buff_size], tset05[buff_size], ratiofile[buff_size], matchfile[buff_size], stat_patent[buff_size], stat_personal[buff_size];
	char oldmatchfile[buff_size], debug_block_file[buff_size], network_file[buff_size], postprocesslog[buff_size], prior_save_file[buff_size];
	char roundstr[buff_size];
	sprintf(oldmatchfile,"%s", EngineConfiguration::previous_disambiguation_result.c_str() );


	bool network_clustering = EngineConfiguration::postprocess_after_each_round;

	if ( debug_mode )
		network_clustering = false;

	unsigned int round = starting_round;



	cRatioComponent personalinfo(uid_dict, string("Personal") );

	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( all_rec_pointers, num_coauthors_to_group );

	//Reconfigure
	std::cout << "Reconfiguring ..." << std::endl;
	const cReconfigurator_AsianNames corrector_asiannames;
	std::for_each(all_rec_pointers.begin(), all_rec_pointers.end(), corrector_asiannames);
	cReconfigurator_Coauthor corrector_coauthor ( blocker_coauthor.get_patent_tree());
	std::for_each(all_rec_pointers.begin(), all_rec_pointers.end(), corrector_coauthor);

	std::cout << "Reconfiguration done." << std::endl;

	cCluster::set_reference_patent_tree_pointer( blocker_coauthor.get_patent_tree());


	vector < string > prev_train_vec;
	unsigned int firstname_prev_truncation = BlockingConfiguration::firstname_cur_truncation;
	const string module_prefix = "Round ";

	string module_name ;
	int is_blockingconfig_success;

	while ( true ) {
		sprintf(roundstr, "%d", round);
		module_name = module_prefix + roundstr;
		is_blockingconfig_success = BlockingConfiguration::config_blocking(BlockingConfigFile, module_name);
		if ( is_blockingconfig_success != 0 )
			break;

		sprintf(xset01, "%s/xset01_%d.txt", working_dir.c_str(), round);
		sprintf(tset05, "%s/tset05_%d.txt", working_dir.c_str(), round);
		sprintf(ratiofile, "%s/ratio_%d.txt", working_dir.c_str(), round);
		sprintf(matchfile, "%s/newmatch_%d.txt", working_dir.c_str(), round);
		sprintf(stat_patent, "%s/stat_patent_%d.txt", working_dir.c_str(), round);
		sprintf(stat_personal, "%s/stat_personal_%d.txt", working_dir.c_str(), round);
		sprintf(debug_block_file, "%s/debug_block_%d.txt", working_dir.c_str(), round);
		sprintf(network_file, "%s/network_%d.txt", working_dir.c_str(), round);
		sprintf(postprocesslog, "%s/postprocesslog_%d.txt", working_dir.c_str(), round);
		sprintf(prior_save_file, "%s/prior_saved_%d.txt", working_dir.c_str(), round);


		//if ( ! BlockingConfiguration::config_blocking(BlockingConfigFile, module_name) )
		//	throw cException_Other("Blocking Configuration is not complete!");

		cRecord::activate_comparators_by_name(BlockingConfiguration::active_similarity_attributes);
		//now training
		//match.output_list(record_pointers);

		const string training_changable [] = { xset01, tset05 };
		const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));

		switch (round) {
			case 1:
			{
				vector <string> presort_columns;
				cString_Remain_Same operator_no_change;
				presort_columns.push_back(cFirstname::static_get_class_name());
				presort_columns.push_back(cLastname::static_get_class_name());
				presort_columns.push_back(cAssignee::static_get_class_name());
				presort_columns.push_back(cStreet::static_get_class_name());
				presort_columns.push_back(cCity::static_get_class_name());
				presort_columns.push_back(cCountry::static_get_class_name());

				//presort_columns.push_back(cClass::static_get_class_name());

				const vector < const cString_Manipulator *> presort_strman( presort_columns.size(), &operator_no_change);
				const vector < unsigned int > presort_data_indice( presort_columns.size(), 0);

				const cBlocking_Operation_Multiple_Column_Manipulate presort_blocker(presort_strman, presort_columns, presort_data_indice);
				match.preliminary_consolidation(presort_blocker, all_rec_pointers);
				match.output_current_comparision_info(oldmatchfile);
			}
			default:
				;
		}
		cFirstname::set_truncation( firstname_prev_truncation, BlockingConfiguration::firstname_cur_truncation);
		firstname_prev_truncation = BlockingConfiguration::firstname_cur_truncation;
		
		match.reset_blocking( * BlockingConfiguration::active_blocker_pointer, oldmatchfile);
		if ( network_clustering ) {
			blocker_coauthor.build_uid2uinv_tree(match);
			cCluster_Set cs;
			cs.convert_from_ClusterInfo(&match);
			post_polish( cs, blocker_coauthor.get_uid2uinv_tree(), blocker_coauthor.get_patent_tree(), string(postprocesslog));
			cs.output_results(network_file);
			match.reset_blocking( * BlockingConfiguration::active_blocker_pointer, network_file);
		}
	      


		if ( ! use_available_ratios ) {
			const cBlocking_Operation_Multiple_Column_Manipulate & blocker_ref =
					dynamic_cast < cBlocking_Operation_Multiple_Column_Manipulate &> ( * BlockingConfiguration::active_blocker_pointer );
			make_changable_training_sets_by_patent( all_rec_pointers, blocker_ref.get_blocking_attribute_names(),
					blocker_ref.get_blocking_string_manipulators(), limit,  training_changable_vec);
		}

		const cRatios * ratio_pointer;
		if ( ! use_available_ratios ) {
			personalinfo.prepare(training_changable_vec.at(0).c_str(),
									training_changable_vec.at(1).c_str() );
			patentinfo.prepare(training_stable_vec.at(0).c_str(),
							   training_stable_vec.at(1).c_str() );

			personalinfo.stats_output(stat_personal);
			patentinfo.stats_output(stat_patent);

			vector < const cRatioComponent *> component_vector;
			component_vector.push_back(&patentinfo);
			component_vector.push_back(&personalinfo);

			ratio_pointer = new cRatios (component_vector, ratiofile, all_records.front());
		}
		else
			ratio_pointer = new cRatios (ratiofile);



		cCluster::set_ratiomap_pointer(*ratio_pointer);
		// now disambiguate
		cRecord::clean_member_attrib_pool();

		match.disambiguate(*ratio_pointer, num_threads, debug_block_file, prior_save_file);

		delete ratio_pointer;

		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);
		++round;
	}


	// post-processing now
	if ( is_blockingconfig_success == 2 ) {
		std::cout << "Final post processing ... ..." << std::endl;
		one_step_prostprocess( all_records, oldmatchfile, ( string(working_dir) + "/final.txt").c_str() );
	}

	return 0;
}


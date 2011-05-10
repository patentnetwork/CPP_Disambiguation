/*
 * Disambigmain.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: ysun
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sqlite3.h>
#include "sqlite3op.h"
#include "DisambigDefs.h"
#include "DisambigEngine.h"
#include "DisambigFileOper.h"
#include "DisambigRatios.h"
#include "DisambigTraining.h"
#include "DisambigCluster.h"
#include "DisambigPostProcess.h"
#include <cmath>
#include <cstring>
using std::list;
using std::string;
#include "Disambigmain.h"

int fullrun_iterative_v4();
int unique_inventors_per_period ( unsigned int starting_year, unsigned int interval, const char * wholedatabase, const char * disambigresult, const char * outputfile);

int main() {
	fullrun_iterative_v4();
	//unique_inventors_per_period ( 1975, 3, "/home/ysun/cpps/Disambiguation/invpat.txt",
	//						"/home/ysun/cpps/Disambiguation/final.txt", "/home/ysun/cpps/Disambiguation/uy.txt");
	return 0;
}


bool make_changable_training_sets_by_names(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames) {


	if ( training_filenames.size() != 2 )
		throw cException_Other("Training: there should be 2 changeable training sets.");


	const bool is_coauthor_active = cCoauthor::static_is_comparator_activated();
	const bool is_class_active = cClass::static_is_comparator_activated();

	if ( ! is_coauthor_active )
		cCoauthor::activate_comparator();

	if ( ! is_class_active )
		cClass::activate_comparator();

	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	cBlocking_For_Training bft (record_pointers, blocking_column_names, pstring_oper, uid_identifier, limit);

	cString_Remain_Same donotchange;
	vector <const cString_Manipulator*> t_extract_equal, t_extract_nonequal, x_extract_equal, x_extract_nonequal;
	x_extract_equal.push_back(& donotchange);
	x_extract_nonequal.push_back(& donotchange);
	x_extract_nonequal.push_back(&donotchange);

	std::ofstream outfile;
	//xset01
	const string xset01_equal_name_array[] = {cApplyYear::static_get_class_name() };
	const string xset01_nonequal_name_array[] = { cAsgNum::static_get_class_name(), cCity::static_get_class_name() };
	const vector <string> xset01_equal_name_vec (xset01_equal_name_array, xset01_equal_name_array + sizeof(xset01_equal_name_array)/sizeof(string));
	const vector <string> xset01_nonequal_name_vec (xset01_nonequal_name_array, xset01_nonequal_name_array + sizeof(xset01_nonequal_name_array)/sizeof(string));



	bft.create_set(&cBlocking_For_Training::create_xset01_on_block, xset01_equal_name_vec, x_extract_equal, xset01_nonequal_name_vec, x_extract_nonequal);
	const char * current_file = training_filenames.at(0).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;

	// tset05
	bft.reset(blocking_column_names.size());
	const string tset05_equal_name_array[] = {};
	const string tset05_nonequal_name_array[] = {};
	const vector <string> tset05_equal_name_vec (tset05_equal_name_array, tset05_equal_name_array + sizeof(tset05_equal_name_array)/sizeof(string));
	const vector <string> tset05_nonequal_name_vec (tset05_nonequal_name_array, tset05_nonequal_name_array + sizeof(tset05_nonequal_name_array)/sizeof(string));

	bft.create_set(&cBlocking_For_Training::create_tset05_on_block, tset05_equal_name_vec, t_extract_equal, tset05_nonequal_name_vec, t_extract_nonequal );

	current_file = training_filenames.at(1).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;

	if ( ! is_coauthor_active )
		cCoauthor::deactivate_comparator();

	if ( ! is_class_active )
		cClass::deactivate_comparator();

	return true;
}

bool make_changable_training_sets_by_assignee(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames) {


	if ( training_filenames.size() != 2 )
		throw cException_Other("Training: there should be 2 changeable training sets.");

	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	cBlocking_For_Training bft (record_pointers, blocking_column_names, pstring_oper, uid_identifier, limit);


	cString_Remain_Same donotchange;

	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;


	vector <const cString_Manipulator*> t_extract_equal, t_extract_nonequal, x_extract_equal, x_extract_nonequal;

	std::ofstream outfile;
	//xset01
	/*
	x_extract_nonequal.push_back(& donotchange);
	x_extract_equal.push_back(&donotchange);

	const string xset01_equal_name_array[] = {cApplyYear::static_get_class_name() };
	const string xset01_nonequal_name_array[] = { cCity::static_get_class_name() };
	const vector <string> xset01_equal_name_vec (xset01_equal_name_array, xset01_equal_name_array + sizeof(xset01_equal_name_array)/sizeof(string));
	const vector <string> xset01_nonequal_name_vec (xset01_nonequal_name_array, xset01_nonequal_name_array + sizeof(xset01_nonequal_name_array)/sizeof(string));


	bft.create_set(&cBlocking_For_Training::create_xset01_on_block, xset01_equal_name_vec, x_extract_equal, xset01_nonequal_name_vec, x_extract_nonequal);
	const char * current_file = training_filenames.at(0).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;
	*/

	outfile.open(training_filenames.at(0).c_str());
	list < std::pair< const cRecord*, const cRecord*> > chosen_pairs;
	cPrint_Pair do_print(outfile, cUnique_Record_ID::static_get_class_name());
	create_xset01( chosen_pairs, record_pointers, limit);
	std::for_each(chosen_pairs.begin(), chosen_pairs.end(), do_print);
	outfile.close();
	std::cout << "Done" << std::endl;

	// tset05

	operator_truncate_firstname.set_truncater(0, 1, true);
	operator_truncate_lastname.set_truncater(0, 2, true);
	t_extract_equal.push_back(& operator_truncate_firstname);
	t_extract_equal.push_back(& operator_truncate_lastname);


	bft.reset(blocking_column_names.size());
	const string tset05_equal_name_array[] = { cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
	const string tset05_nonequal_name_array[] = {};
	const vector <string> tset05_equal_name_vec (tset05_equal_name_array, tset05_equal_name_array + sizeof(tset05_equal_name_array)/sizeof(string));
	const vector <string> tset05_nonequal_name_vec (tset05_nonequal_name_array, tset05_nonequal_name_array + sizeof(tset05_nonequal_name_array)/sizeof(string));

	bft.create_set(&cBlocking_For_Training::create_tset05_on_block, tset05_equal_name_vec, t_extract_equal, tset05_nonequal_name_vec, t_extract_nonequal );

	const char * current_file = training_filenames.at(1).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;

	return true;
}



bool make_stable_training_sets_by_personal( const list <cRecord> & all_records, const unsigned int limit, const vector <string> & training_filenames) {
	//if ( training_filenames.size() != 2 )
		//throw cException_Other("Training: there should be 2 changeable training sets.");

	cGroup_Value rare_firstname_set;
	cGroup_Value rare_lastname_set;

	std::ofstream outfile;
	cPrint_Pair do_print(outfile, cUnique_Record_ID::static_get_class_name());
	const char * current_file;
	vector<cGroup_Value *> rare_pointer_vec;
	rare_pointer_vec.push_back(&rare_firstname_set);
	rare_pointer_vec.push_back(&rare_lastname_set);
	const vector< const cGroup_Value * > const_rare_pointer_vec(rare_pointer_vec.begin(), rare_pointer_vec.end());

	list < const cRecord*> record_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		record_pointers.push_back(&(*p));

	find_rare_names_v2(rare_pointer_vec, record_pointers);
	list<pointer_pairs> pair_list;
	vector <string> rare_column_names;
	rare_column_names.push_back(string(cFirstname::static_get_class_name()));
	rare_column_names.push_back(string(cLastname::static_get_class_name()));

	//xset03
	pair_list.clear();
	create_xset03(pair_list, record_pointers, const_rare_pointer_vec, limit);
	current_file = training_filenames.at(0).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	std::for_each(pair_list.begin(), pair_list.end(), do_print);
	outfile.close();
	std::cout << "Done" << std::endl;


	//tset02
	pair_list.clear();
	create_tset02(pair_list, record_pointers, rare_column_names, const_rare_pointer_vec, limit);

	current_file = training_filenames.at(1).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	std::for_each(pair_list.begin(), pair_list.end(), do_print);
	outfile.close();
	std::cout << "Done" << std::endl;

	return true;
}


bool make_stable_training_sets_by_patent(const list <const cRecord*> & record_pointers, const unsigned int limit, const vector <string> & training_filenames) {


	if ( training_filenames.size() != 2 )
		throw cException_Other("Training: there should be 2 changeable training sets.");

	const string uid_identifier = cUnique_Record_ID::static_get_class_name();



	cString_Remain_Same donotchange;

	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;


	vector <const cString_Manipulator*> t_extract_equal, t_extract_nonequal, x_extract_equal, x_extract_nonequal;

	std::ofstream outfile;
	//xset01
	/*
	x_extract_nonequal.push_back(& donotchange);
	x_extract_equal.push_back(&donotchange);

	const string xset01_equal_name_array[] = {cApplyYear::static_get_class_name() };
	const string xset01_nonequal_name_array[] = { cCity::static_get_class_name() };
	const vector <string> xset01_equal_name_vec (xset01_equal_name_array, xset01_equal_name_array + sizeof(xset01_equal_name_array)/sizeof(string));
	const vector <string> xset01_nonequal_name_vec (xset01_nonequal_name_array, xset01_nonequal_name_array + sizeof(xset01_nonequal_name_array)/sizeof(string));


	bft.create_set(&cBlocking_For_Training::create_xset01_on_block, xset01_equal_name_vec, x_extract_equal, xset01_nonequal_name_vec, x_extract_nonequal);
	const char * current_file = training_filenames.at(0).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;
	*/

	outfile.open(training_filenames.at(0).c_str());
	list < std::pair< const cRecord*, const cRecord*> > chosen_pairs;
	cPrint_Pair do_print(outfile, cUnique_Record_ID::static_get_class_name());
	create_xset01( chosen_pairs, record_pointers, limit);
	std::for_each(chosen_pairs.begin(), chosen_pairs.end(), do_print);
	outfile.close();
	std::cout << "Done" << std::endl;

	// tset05


	operator_truncate_firstname.set_truncater(0, 1, true);
	operator_truncate_lastname.set_truncater(0, 2, true);
	t_extract_equal.push_back(& operator_truncate_firstname);
	t_extract_equal.push_back(& operator_truncate_lastname);


	const string blocking_columns [] = {cAssignee::static_get_class_name(), cCity::static_get_class_name(), cApplyYear::static_get_class_name()};
	const unsigned int num_columns = sizeof(blocking_columns)/sizeof(string);
	const vector < string > blocking_column_names(blocking_columns, blocking_columns + num_columns );
	vector < const cString_Manipulator *> pstring_oper;
	for ( unsigned int i = 0; i < num_columns; ++i )
		pstring_oper.push_back(&donotchange);


	cBlocking_For_Training bft (record_pointers, blocking_column_names, pstring_oper, uid_identifier, limit);

	const string tset05_equal_name_array[] = { cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
	const string tset05_nonequal_name_array[] = {};
	const vector <string> tset05_equal_name_vec (tset05_equal_name_array, tset05_equal_name_array + sizeof(tset05_equal_name_array)/sizeof(string));
	const vector <string> tset05_nonequal_name_vec (tset05_nonequal_name_array, tset05_nonequal_name_array + sizeof(tset05_nonequal_name_array)/sizeof(string));

	bft.create_set(&cBlocking_For_Training::create_tset05_on_block, tset05_equal_name_vec, t_extract_equal, tset05_nonequal_name_vec, t_extract_nonequal );

	const char * current_file = training_filenames.at(1).c_str();
	outfile.open(current_file);
	if ( ! outfile.good() )
		throw cException_File_Not_Found(current_file);
	std::cout << "Creating " << current_file << " ..." << std::endl;
	bft.print(outfile, uid_identifier);
	outfile.close();
	std::cout << "Done" << std::endl;

	return true;
}




//******************************************************************************
//******************************************************************************
//******************************************************************************

int fullrun_iterative_v4() {
	const bool do_not_train_stable = true;
	const bool use_available_ratios = false;
	const string working_dir = "/media/data/edwardspace/workplace/testcpp/Disambiguation";
	const string final_file = working_dir + "/final.txt";
	const double thresholds[] = { 0.99, 0.95, 0.90 };
	const unsigned int buff_size = 512;
	const unsigned int num_threads = 2;
	std::cout << std::endl;
	std::cout << "====================== STARTING DISAMBIGUATION ===========================" << std::endl;
	std::cout << std::endl;
	//cFirstname::activate_comparator();
	//cMiddlename::activate_comparator();
	//cLastname::activate_comparator();
	//cLatitude::activate_comparator();
	//cAssignee::activate_comparator();
	//cClass::activate_comparator();
	//cCoauthor::activate_comparator();
	list <cRecord> all_records;
	char filename2[buff_size];
	sprintf(filename2, "%s/invpat.txt", working_dir.c_str());

	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
									"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street", "ApplyYear", "City", "Patent"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	char assignee_file[buff_size];
	sprintf(assignee_file, "%s/asg.txt", working_dir.c_str());
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec);
	if (not is_success) return 1;

	cAssignee::set_assignee_tree_pointer (assignee_tree);
	const unsigned int limit = 100000;
	//patent stable
	const string training_stable [] = { working_dir + "/xset03_stable.txt",
												working_dir + "/tset02_stable.txt" };
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	if ( ! do_not_train_stable )
		make_stable_training_sets_by_personal ( all_records, limit, training_stable_vec);

	map <string, const cRecord *> uid_dict;
	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	//train patent info
	cRatioComponent patentinfo(uid_dict, string("Patent") );
	//patentinfo.prepare(training_stable_vec.at(0).c_str(),
		//			   training_stable_vec.at(1).c_str() );


	//personal stable
	list < const cRecord * > record_pointers;
	/*
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		record_pointers.push_back(&(*p));
	const string personal_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01_stable.txt",
												"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05_stable.txt" };
	const vector<string> personal_stable_vec ( personal_stable, personal_stable + sizeof(personal_stable)/sizeof(string));
	make_stable_training_sets_by_patent ( record_pointers, limit, personal_stable_vec);
	*/


	//const unsigned int max_round_by_name = 3;
	const unsigned int max_round_by_coauthor = 8;

	const vector < double > threshold_vec( thresholds, thresholds + sizeof(thresholds)/sizeof(double) ) ;
	bool matching_mode = true;
	bool frequency_adjust_mode = true;
	bool debug_mode = false;
	cCluster_Info match ( uid_dict, matching_mode, frequency_adjust_mode, debug_mode);
	match.set_thresholds( threshold_vec);

	cString_Remove_Space operator_no_space;
	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;
	cString_NoSpace_Truncate operator_truncate_middlename;
	cString_Remain_Same operator_no_change;



	char xset01[buff_size], tset05[buff_size], ratiofile[buff_size], matchfile[buff_size], stat_patent[buff_size], stat_personal[buff_size];
	char oldmatchfile[buff_size], debug_block_file[buff_size], network_file[buff_size], postprocesslog[buff_size], prior_save_file[buff_size];
	sprintf(oldmatchfile, "%s/match_cons.txt", working_dir.c_str() );
	//char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_1.txt";


	bool network_clustering = true;

	if ( debug_mode )
		network_clustering = false;
	const unsigned int starting_round =  1;
	unsigned int round = starting_round;

	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));

	cRatioComponent personalinfo(uid_dict, string("Personal") );

	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( all_rec_pointers, num_coauthors_to_group );

	//Reconfigure
	std::cout << "Reconfiguring ..." << std::endl;
	const cReconfigurator_AsianNames corrector_asiannames;
	const cReconfigurator_Latitude_Interactives corrector_lat_interactives;

	std::for_each(all_rec_pointers.begin(), all_rec_pointers.end(), corrector_asiannames);
	std::for_each(all_rec_pointers.begin(), all_rec_pointers.end(), corrector_lat_interactives);
	cReconfigurator_Coauthor corrector_coauthor ( blocker_coauthor.get_patent_tree());
	std::for_each(all_rec_pointers.begin(), all_rec_pointers.end(), corrector_coauthor);

	std::cout << "Reconfiguration done." << std::endl;

	cCluster::set_reference_patent_tree_pointer( blocker_coauthor.get_patent_tree());

	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_truncate_firstname);
	pstring_oper.push_back(& operator_truncate_middlename);
	pstring_oper.push_back(& operator_truncate_lastname);

	const string blocking_names[] = {cFirstname::static_get_class_name(), cMiddlename::static_get_class_name(), cLastname::static_get_class_name()};
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
	vector < unsigned int > blocking_column_data_indice ( blocking_column_names.size(), 0 );
	blocking_column_data_indice.at(0) = 1;
	blocking_column_data_indice.at(1) = 1;
	cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names, blocking_column_data_indice);

	vector < string > prev_train_vec;
	unsigned int firstname_prev_truncation = 0, firstname_cur_truncation = 0;
	for ( ; round <= max_round_by_coauthor; ++round ) {
		std::cout << std::endl;
		std::cout << "====================== ROUND " << round << " ===========================" << std::endl;
		std::cout << std::endl;


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

		//now training
		//match.output_list(record_pointers);

		const string training_changable [] = { xset01, tset05 };
		const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));



		firstname_prev_truncation = firstname_cur_truncation;

		switch (round) {
		case 1:
			//======preliminary consolidation. very strict conditions.
		{
			vector <string> presort_columns;
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

			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::activate_comparator();
			cAssignee::deactivate_comparator();
			cClass::deactivate_comparator();
			cCoauthor::deactivate_comparator();

			operator_truncate_firstname.set_truncater(0, 0, true);
			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 0, true);

			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;
		case 2:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::activate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();


			firstname_cur_truncation = 0;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);

			operator_truncate_middlename.set_truncater(0, 0, true);
			operator_truncate_lastname.set_truncater(0, 0, true);
			//match.reset_blocking(blocker, oldmatchfile);
			//if ( ! use_available_ratios )
			//	make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			break;
		case 3:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::activate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();


			firstname_cur_truncation = 5;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);
			operator_truncate_middlename.set_truncater(0, 1, true);
			operator_truncate_lastname.set_truncater(0, 8, true);
			//match.reset_blocking(blocker, oldmatchfile);
			//if ( ! use_available_ratios )
			//	make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;
		case 4:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::activate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();


			firstname_cur_truncation = 3;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);
			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 5, true);
			//match.reset_blocking(blocker, oldmatchfile);
			//if ( ! use_available_ratios )
			//	make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			//prev_train_vec = training_changable_vec;
			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;


		case 5:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::activate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();



			firstname_cur_truncation = 1;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);
			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 5, true);
			//match.reset_blocking(blocker, oldmatchfile);
			//if ( ! use_available_ratios )
			//	make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			break;

		case 6:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::deactivate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();



			firstname_cur_truncation = 1;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);
			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 5, true);
			//match.reset_blocking(blocker, oldmatchfile);
			sprintf( xset01, "%s/xset01_%d.txt", working_dir.c_str(), round - 1 );
			sprintf( tset05, "%s/tset05_%d.txt", working_dir.c_str(), round - 1 );
			prev_train_vec.clear();
			prev_train_vec.push_back(string(xset01));
			prev_train_vec.push_back(string(tset05));

			for ( unsigned int i = 0; i < training_changable_vec.size(); ++i ) {
				copyfile(training_changable_vec.at(i).c_str(), prev_train_vec.at(i).c_str());
			}

			std::cout << "=============== WARNING: STEP " << round << " IS SKIPPED ! ================="  << std::endl;
			continue;

			break;


		case 7:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::activate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();


			firstname_cur_truncation = 1;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);

			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 3, true);
			//match.reset_blocking(blocker, oldmatchfile);
			//if ( ! use_available_ratios )
			//	make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			break;

		case 8:
			cFirstname::activate_comparator();
			cMiddlename::activate_comparator();
			cLastname::activate_comparator();
			cLatitude::deactivate_comparator();
			cAssignee::deactivate_comparator();
			cClass::activate_comparator();
			cCoauthor::activate_comparator();

			firstname_cur_truncation = 1;
			cFirstname::set_truncation( firstname_prev_truncation, firstname_cur_truncation);
			operator_truncate_firstname.set_truncater(0, firstname_cur_truncation, true);
			operator_truncate_middlename.set_truncater(0, 0, false);
			operator_truncate_lastname.set_truncater(0, 3, true);
			//match.reset_blocking(blocker, oldmatchfile);
			sprintf( xset01, "%s/xset01_%d.txt", working_dir.c_str(), round - 1 );
			sprintf( tset05, "%s/tset05_%d.txt", working_dir.c_str(), round - 1 );
			prev_train_vec.clear();
			prev_train_vec.push_back(string(xset01));
			prev_train_vec.push_back(string(tset05));

			for ( unsigned int i = 0; i < training_changable_vec.size(); ++i ) {
				copyfile(training_changable_vec.at(i).c_str(), prev_train_vec.at(i).c_str());
			}

			std::cout << "=============== WARNING: STEP " << round << " IS SKIPPED ! ================="  << std::endl;
			continue;

			break;


		default:
			throw cException_Other("Invalid round.");
		}
		
		match.reset_blocking(blocker, oldmatchfile);
		if ( network_clustering ) {
			blocker_coauthor.build_uid2uinv_tree(match);
			cCluster_Set cs;
			cs.convert_from_ClusterInfo(&match);
			post_polish( cs, blocker_coauthor.get_uid2uinv_tree(), blocker_coauthor.get_patent_tree(), string(postprocesslog));
			cs.output_results(network_file);
			match.reset_blocking(blocker, network_file);
		}
	      


		if ( ! use_available_ratios )
			make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);

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

	}


	// post-processing now

	operator_truncate_firstname.set_truncater(0, 0, true);
	operator_truncate_middlename.set_truncater(0, 0, false);
	operator_truncate_lastname.set_truncater(0, 0, true);

	match.reset_blocking(blocker, oldmatchfile);

	blocker_coauthor.build_uid2uinv_tree(match);
	cCluster_Set cs;
	cs.convert_from_ClusterInfo(&match);
	post_polish( cs, blocker_coauthor.get_uid2uinv_tree(), blocker_coauthor.get_patent_tree(), "final_postprocess_log.txt");
	cs.output_results(final_file.c_str());


	return 0;
}


std::pair < const cRecord *, set < const cRecord * > > ones_temporal_unique_coauthors ( const cCluster & record_cluster,
		const map < const cRecord *, const cRecord *> & complete_uid2uinv,
		const map < const cRecord *, cGroup_Value, cSort_by_attrib > & complete_patent_tree,
		const unsigned int begin_year, const unsigned int end_year, const unsigned int year_index ) {
	//[ begin_year, end_year )

	const cRecord * ret1 = NULL;
	set < const cRecord * > ret2;

	const cGroup_Value & same_author = record_cluster.get_fellows();

	cGroup_Value qualified_same_author;
	for ( cGroup_Value::const_iterator psa = same_author.begin(); psa != same_author.end(); ++psa ) {
		//check year range
		const cAttribute * pAttrib = (*psa)->get_attrib_pointer_by_index(year_index);
		const unsigned int checkyear = atoi (pAttrib->get_data().at(0)->c_str());
		if ( checkyear >= begin_year && checkyear < end_year )
			qualified_same_author.push_back(*psa);
		//end of year range check
	}
	for ( cGroup_Value::const_iterator pqsa = qualified_same_author.begin(); pqsa != qualified_same_author.end(); ++pqsa) {
		const map < const cRecord *, cGroup_Value, cSort_by_attrib >::const_iterator tempi = complete_patent_tree.find(*pqsa);
		if ( tempi == complete_patent_tree.end() )
			throw cException_Other("patent not in patent tree.");
		const cGroup_Value & coauthor_per_patent = tempi->second;
		for ( cGroup_Value::const_iterator pp = coauthor_per_patent.begin(); pp != coauthor_per_patent.end(); ++pp ) {
			if ( *pp == *pqsa)
				continue;
			map < const cRecord *, const cRecord *>::const_iterator tempi2 = complete_uid2uinv.find(*pp);
			if ( tempi2 == complete_uid2uinv.end() )
				throw cException_Other("uid not in uid tree.");
			const cRecord * inv = tempi2->second;
			//finally, got it.
			ret2.insert(inv);
		}
	}
	if ( ! qualified_same_author.empty())
		ret1 = qualified_same_author.front();

	return std::pair < const cRecord * ,set < const cRecord * > >(ret1, ret2);

}

int unique_inventors_per_period ( unsigned int starting_year, unsigned int interval, const char * wholedatabase, const char * disambigresult, const char * outputfile) {
	typedef std::pair< const cRecord *, set < const cRecord *> > cUINV2UCOAUTHOR;
	list <cRecord> all_records;
	const string columns[] = {"Unique_Record_ID", "Patent",  "ApplyYear"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );

	bool is_success = fetch_records_from_txt(all_records, wholedatabase, column_vec);
	if (not is_success) return 1;

	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));

	cString_Remain_Same manobj;
	cBlocking_Operation_Column_Manipulate tempblocker (manobj, "ApplyYear");
	cBlocking_Operation_By_Coauthors bocobj(all_rec_pointers, 1);
	cCluster::set_reference_patent_tree_pointer(bocobj.get_patent_tree());

	map <string, const cRecord *> uid_dict;
	create_btree_uid2record_pointer(uid_dict, all_records, cUnique_Record_ID::static_get_class_name());

	cCluster_Info ci( uid_dict, true, true, false);
	ci.reset_blocking(tempblocker, disambigresult);
	bocobj.build_uid2uinv_tree(ci);

	cCluster_Set all_clusters;
	all_clusters.convert_from_ClusterInfo(&ci);

	const map < const cRecord *, const cRecord *> & uid2uinv = bocobj.get_uid2uinv_tree();
	const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree = bocobj.get_patent_tree();
	const list < cCluster > & full_list = all_clusters.get_set();



	const string & beginyearstring = ci.get_cluster_map().begin()->first;
	const string & endyearstring = ci.get_cluster_map().rbegin()->first;
	const unsigned int endyear = atoi( endyearstring.c_str() );

	const unsigned int appyearindex = cRecord::get_index_by_name(cApplyYear::static_get_class_name());
	std::cout << "Begin year = "<<beginyearstring << " , End year = " << endyearstring << std::endl;

	map < unsigned int, unsigned int > unique_coauthor_year_chunk;
	map < unsigned int, unsigned int > unique_inventor_year_chunk;
	for ( unsigned int y = starting_year; y <= endyear; y += interval ) {
		unsigned int unique_inventors = 0;
		unsigned int unique_coauthors = 0;
		for ( list< cCluster >::const_iterator puinv = full_list.begin() ; puinv != full_list.end(); ++puinv) {
			std::pair< const cRecord * , set< const cRecord *> > kk =
					ones_temporal_unique_coauthors ( *puinv, uid2uinv, patent_tree, y, y + interval, appyearindex );
			if ( kk.first != NULL ) {
				++ unique_inventors;
				unique_coauthors += kk.second.size();
			}
		}
		unique_coauthor_year_chunk.insert(std::pair<unsigned int, unsigned int>(y, unique_coauthors));
		unique_inventor_year_chunk.insert(std::pair<unsigned int, unsigned int>(y, unique_inventors));
		std::cout << "Year " << y << " done." << std::endl;
	}

	std::ostream & os = std::cout;
	const string space_delim = "          ";
	os << "Year Chunk:" << space_delim << "Number of Unique Inventors:" << space_delim << "Number of Unique Coauthors:" << std::endl;
	for ( map < unsigned int, unsigned int >::const_iterator p = unique_inventor_year_chunk.begin(); p != unique_inventor_year_chunk.end(); ++p ) {
		os << p->first << space_delim << p->second << space_delim << unique_coauthor_year_chunk.find(p->first)->second << std::endl;
	}

	return 0;
}

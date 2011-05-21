/*
 * DisambigUtilities.cpp
 *
 *  Created on: May 11, 2011
 *      Author: ysun
 */

#include "DisambigUtilities.h"
#include "DisambigDefs.h"
#include "DisambigEngine.h"
#include "DisambigFileOper.h"
#include "DisambigRatios.h"
#include "DisambigTraining.h"
#include "DisambigCluster.h"
#include "DisambigPostProcess.h"
bool make_changable_training_sets_by_patent(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames) {


	if ( training_filenames.size() != 2 )
		throw cException_Other("Training: there should be 2 changeable training sets.");


	const bool is_coauthor_active = cCoauthor::static_is_comparator_activated();
	const bool is_class_active = cClass::static_is_comparator_activated();

	if ( ! is_coauthor_active )
		cCoauthor::static_activate_comparator();

	if ( ! is_class_active )
		cClass::static_activate_comparator();

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
		cCoauthor::static_deactivate_comparator();

	if ( ! is_class_active )
		cClass::static_deactivate_comparator();

	return true;
}
#if 0
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
#endif




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



#if 0
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

#endif


//====================


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


//====================


void one_step_prostprocess( const list < cRecord > & all_records, const char * last_disambig_result, const char * outputfile) {
	map <string, const cRecord *> uid_dict;
	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));

	cCluster_Info match ( uid_dict, true, true, false);

	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( all_rec_pointers, num_coauthors_to_group );

	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;
	cString_NoSpace_Truncate operator_truncate_middlename;

	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_truncate_firstname);
	pstring_oper.push_back(& operator_truncate_middlename);
	pstring_oper.push_back(& operator_truncate_lastname);

	const string blocking_names[] = {cFirstname::static_get_class_name(), cMiddlename::static_get_class_name(), cLastname::static_get_class_name()};
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
	vector < unsigned int > blocking_column_data_indice ( blocking_column_names.size(), 0 );
	//blocking_column_data_indice.at(0) = 1;
	//blocking_column_data_indice.at(1) = 1;
	cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names, blocking_column_data_indice);

	operator_truncate_firstname.set_truncater(0, 0, true);
	operator_truncate_middlename.set_truncater(0, 0, false);
	operator_truncate_lastname.set_truncater(0, 0, true);

	match.reset_blocking( blocker , last_disambig_result );

	blocker_coauthor.build_uid2uinv_tree(match);
	cCluster_Set cs;
	cs.convert_from_ClusterInfo(&match);
	const char * suffix = ".pplog";
	const string logfile = string(outputfile) + suffix ;
	post_polish( cs, blocker_coauthor.get_uid2uinv_tree(), blocker_coauthor.get_patent_tree(), logfile);
	cs.output_results(outputfile);
}


string remove_headtail_space( const string & s ) {
	string::const_iterator istart = s.begin() , iend = s.end();
	while( istart != iend ) {
		if( *istart != ' ' )
			break;
		++istart;
	}
	while( iend != istart )
	{
		--iend;
		if( *iend != ' ' ) {
			++iend;
			break;
		}
	}
	string str_result(istart,iend);
	return str_result;
}

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
#include <cmath>
using std::list;
using std::string;
#include "Disambigmain.h"

int fullrun_iterative_v4();
int main() {
	fullrun_iterative_v4();
	//main_to_dump();
	return 0;
}


bool make_changable_training_sets_by_names(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames) {


	if ( training_filenames.size() != 2 )
		throw cException_Other("Training: there should be 2 changeable training sets.");


	const bool is_coauthor_active = cCoauthor::static_is_comparator_activated();

	if ( ! is_coauthor_active )
		cCoauthor::activate_comparator();

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


























int main_to_train() {
	list <cRecord> all_records;
	const char * filename2 = "/media/data/edwardspace/workplace/testcpp/Disambiguation/invpat.txt";
	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
									"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street", "ApplyYear", "City", "Patent"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	const char * assignee_file = "/media/data/edwardspace/workplace/testcpp/Disambiguation/asg.txt";
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success) return 1;


	list < const cRecord*> record_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		record_pointers.push_back(&(*p));

	const string blocking_names[] = {"Firstname", "Lastname"};
	cString_Remove_Space operator_no_space;
	cString_Truncate operator_truncate;
	operator_truncate.set_truncater(0,6,true);
	cString_Remain_Same operator_no_change;
	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_no_space);
	pstring_oper.push_back(& operator_no_space);

	const string uid_identifier = "Unique_Record_ID";
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );

	const string training_changable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01.txt",
										"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05.txt"};
	const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));
	make_changable_training_sets_by_names( record_pointers, blocking_column_names, pstring_oper, 100000,  training_changable_vec);


	const string training_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset03.txt",
											"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset02.txt"};
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	make_stable_training_sets_by_personal ( all_records, 100000, training_stable_vec);

	clear_records(all_records);
	return 0;
}
#if 0
int main_to_dump() {
	dump_match ( "/media/data/edwardspace/workplace/testcpp/Disambiguation/mini_to_dump.sqlite3", "invpat",
						"/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_3.txt",
						string("Invnum"), string("Invnum_N") );
	return 0;
}
#endif

int main_to_dump(const char * source_txt, const char * target_sqlite3) {
	//dump_match ( "/home/ysun/disambig/newcode/all/invpatC.good.Jan2011.sqlite3", "invpat",
	//					"/home/ysun/disambig/newcode/all/newmatch_1.txt",
	//					string("Invnum"), string("Invnum_N") );
	dump_match ( target_sqlite3, "invpat",
						source_txt,
						string("Invnum"), string("Invnum_N") );

	return 0;
}


#if 0
int main_to_go() {
	list <cRecord> all_records;

	//const char *filename = "/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/Trainingsets/invpatC.train.mini.sqlite3";
	//const char * filename = "/media/data/edwardspace/alexdisambig/new/single/sqlite_dbs/invpatC.train.mini.sqlite3";
	//const char * filename = "/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/Trainingsets/invpatC.train.mini.sqlite3";
	const char * filename2 = "/media/data/edwardspace/workplace/testcpp/Disambiguation/invpat.txt";
	//const char * statement = "select * from invpat;";
	char statement[500];
	sprintf(statement, "select Firstname, Firstname as Middlename, Lastname, lat as Latitude, lon as Longitude, \
						Street, Country, Class, coauths as Coauthor, Assignee, AsgNum, Invnum as Unique_Record_ID \
						from invpat;" );

	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
								"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	const char * assignee_file = "/media/data/edwardspace/workplace/testcpp/Disambiguation/asg.txt";
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	//bool is_success = fetch_records_from_sqlite3(all_records, filename,statement, assignee_tree);
	bool is_success2 = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success2) return 1;
	
	/*
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p ) {
		p->print();
	}

	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p ) {
		for ( list<cRecord>::const_iterator q = p; q != all_records.end(); ++q ) {
			vector < unsigned int > res = p->record_compare(*q);
			for ( vector <unsigned int >::const_iterator pui = res.begin(); pui != res.end(); ++pui ) {
				std::cout << *pui << " | ";
			}
			std::cout << std::endl;
		}
	}
	*/

	const string blocking_names[] = {"Firstname", "Lastname"};
	cString_Remove_Space operator_no_space;
	cString_Truncate operator_truncate(0, 6, true);
	cString_Remain_Same operator_no_change;
	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_no_space);
	pstring_oper.push_back(& operator_no_space);

	const string uid_identifier = "Unique_Record_ID";
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
	cBlocking_For_Disambiguation bb (all_records, blocking_column_names, pstring_oper, uid_identifier);


	map <string, const cRecord *> uid_dict;
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);
	/*
	cRatioComponent patentinfo(all_records, uid_dict, string("Patent") );
	patentinfo.prepare("/media/data/edwardspace/workplace/testcpp/Disambiguation/x_patent.txt",
					   "/media/data/edwardspace/workplace/testcpp/Disambiguation/m_patent.txt");
	cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );
	personalinfo.prepare("/media/data/edwardspace/workplace/testcpp/Disambiguation/x_personal.txt",
						 "/media/data/edwardspace/workplace/testcpp/Disambiguation/m_personal.txt");
	vector < const cRatioComponent *> component_vector;
	component_vector.push_back(&patentinfo);
	component_vector.push_back(&personalinfo);
	*/
	//cRatios test_ratio(component_vector, "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio.txt");
	cRatios test_ratio( "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio.txt");
	//test_ratio.write_ratios_file("/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/mydisambiguation/verify_ratio.txt");
	
	
	cCluster_Info match ("/media/data/edwardspace/workplace/testcpp/Disambiguation/match.txt", uid_identifier, uid_dict, true);
	cCluster_Info nonmatch ("/media/data/edwardspace/workplace/testcpp/Disambiguation/nonmatch.txt", uid_identifier, uid_dict, false);
	//match.retrieve_last_comparision_info();
	//match.output_current_comparision_info("/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/Trainingsets/kkk.txt");
	bb.disambiguate(match, nonmatch, test_ratio.get_ratios_map());
	match.output_current_comparision_info("/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch.txt");
	nonmatch.output_current_comparision_info("/media/data/edwardspace/workplace/testcpp/Disambiguation/newnonmatch.txt");

	
	clear_records(all_records);
	return 0;
}
#endif


#if 0
int fullrun() {
	cFirstname::activate_comparator();
	cMiddlename::activate_comparator();
	cLastname::activate_comparator();
	cLatitude::activate_comparator();
	cAssignee::activate_comparator();
	cClass::activate_comparator();
	//cCoauthor::deactivate_comparator();
	list <cRecord> all_records;
	const char * filename2 = "/media/data/edwardspace/workplace/testcpp/Disambiguation/invpat.txt";
	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
									"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street", "ApplyYear", "City"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	const char * assignee_file = "/media/data/edwardspace/workplace/testcpp/Disambiguation/asg.txt";
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success) return 1;


	list < const cRecord*> record_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		record_pointers.push_back(&(*p));

	const string blocking_names[] = {cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
	cString_Remove_Space operator_no_space;
	cString_NoSpace_Truncate operator_truncate;
	operator_truncate.set_truncater(0,6,true);
	cString_Remain_Same operator_no_change;
	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_no_space);
	pstring_oper.push_back(& operator_no_space);

	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );

	cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names);
	//now training

	const unsigned int limit = 100000;
	const string training_changable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01.txt",
										"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05.txt"};
	const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));
	make_changable_training_sets_by_names( record_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);


	const string training_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset03.txt",
											"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset02.txt"};
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	make_stable_training_sets_by_personal ( all_records, limit, training_stable_vec);


	// now creating ratios

	//cBlocking_For_Disambiguation bb (record_pointers, blocking_column_names, pstring_oper, uid_identifier);


	map <string, const cRecord *> uid_dict;
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	cRatioComponent patentinfo(all_records, uid_dict, string("Patent") );
	patentinfo.prepare(training_stable_vec.at(0).c_str(),
					   training_stable_vec.at(1).c_str() );
	cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );
	personalinfo.prepare(training_changable_vec.at(0).c_str(),
						 training_changable_vec.at(1).c_str() );

	vector < const cRatioComponent *> component_vector;
	component_vector.push_back(&patentinfo);
	component_vector.push_back(&personalinfo);

	cRatios test_ratio(component_vector, "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio.txt", all_records.front());
	//cRatios test_ratio( "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio.txt");
	//test_ratio.write_ratios_file("/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/mydisambiguation/verify_ratio.txt");

	cCluster_Info match ( uid_dict, true);
	match.reset_blocking(blocker, "/media/data/edwardspace/workplace/testcpp/Disambiguation/match.txt");
	//cCluster_Info nonmatch ("/media/data/edwardspace/workplace/testcpp/Disambiguation/nonmatch.txt", uid_identifier, uid_dict, false, bb);
	//match.retrieve_last_comparision_info();
	//match.output_current_comparision_info("/Users/katelin-yutingyang/Documents/LaoGong/XcodeCPP/Trainingsets/kkk.txt");
	//bb.disambiguate(match, nonmatch, test_ratio.get_ratios_map());
	match.disambiguate(test_ratio, 4);
	match.output_current_comparision_info("/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch.txt");
	//nonmatch.output_current_comparision_info("/media/data/edwardspace/workplace/testcpp/Disambiguation/newnonmatch.txt");

	dump_match ( "/media/data/edwardspace/workplace/testcpp/Disambiguation/mini_to_dump.sqlite3", "invpat",
					"/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch.txt", string("Invnum"), string("Invnum_N") );

	clear_records(all_records);
	return 0;
}


int fullrun_iterative() {
	const unsigned int num_threads = 4;
	std::cout << std::endl;
	std::cout << "====================== STARTING DISAMBIGUATION ===========================" << std::endl;
	std::cout << std::endl;
	cFirstname::activate_comparator();
	cMiddlename::activate_comparator();
	cLastname::activate_comparator();
	cLatitude::activate_comparator();
	cAssignee::activate_comparator();
	cClass::activate_comparator();
	//cCoauthor::deactivate_comparator();
	list <cRecord> all_records;
	const char * filename2 = "/media/data/edwardspace/workplace/testcpp/Disambiguation/invpat.txt";
	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
									"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street", "ApplyYear", "City", "Patent"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	const char * assignee_file = "/media/data/edwardspace/workplace/testcpp/Disambiguation/asg.txt";
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success) return 1;

	const unsigned int limit = 100000;
	//patent stable
	const string training_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset03_stable.txt",
												"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset02_stable.txt" };
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	make_stable_training_sets_by_personal ( all_records, limit, training_stable_vec);


	map <string, const cRecord *> uid_dict;
	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	//train patent info
	cRatioComponent patentinfo(all_records, uid_dict, string("Patent") );
	patentinfo.prepare(training_stable_vec.at(0).c_str(),
					   training_stable_vec.at(1).c_str() );


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


	const unsigned int max_round_by_name = 3;
	const unsigned int max_round_by_coauthor = 5;

	cCluster_Info match ( uid_dict, true);

	cString_Remove_Space operator_no_space;
	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;
	cString_Remain_Same operator_no_change;


	const unsigned int buff_size = 500;
	char xset01[buff_size], tset05[buff_size], ratiofile[buff_size], matchfile[buff_size];
	char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/match.txt";
	//char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_1.txt";


	const unsigned int starting_round =  1;
	unsigned int round = starting_round;

	sprintf(xset01, "/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01_%d.txt", round);
	sprintf(tset05, "/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05_%d.txt", round);
	sprintf(ratiofile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio_%d.txt", round);

	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));

	cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );

	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_truncate_firstname);
	pstring_oper.push_back(& operator_truncate_lastname);

	const string blocking_names[] = {cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
	cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names);

	for ( ; round <= max_round_by_name; ++round ) {
		std::cout << std::endl;
		std::cout << "====================== ROUND " << round << " ===========================" << std::endl;
		std::cout << std::endl;
		switch (round) {
		case 1:
			operator_truncate_firstname.set_truncater(0, 0, true);
			operator_truncate_lastname.set_truncater(0, 0, true);
			break;
		case 2:
			operator_truncate_firstname.set_truncater(0, 5, true);
			operator_truncate_lastname.set_truncater(0, 8, true);
			break;
		case 3:
			operator_truncate_firstname.set_truncater(0, 3, true);
			operator_truncate_lastname.set_truncater(0, 5, true);
			break;
		default:
			throw cException_Other("Invalid round!");
		}


		sprintf(matchfile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_%d.txt", round);



		match.reset_blocking(blocker, oldmatchfile);
		//now training
		match.output_list(record_pointers);

		const string training_changable [] = { xset01, tset05 };
		const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));
		make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);

		//now obtaining ratios


		personalinfo.prepare(training_changable_vec.at(0).c_str(),
							 training_changable_vec.at(1).c_str() );

		vector < const cRatioComponent *> component_vector;
		component_vector.push_back(&patentinfo);
		component_vector.push_back(&personalinfo);

		cRatios test_ratio(component_vector, ratiofile, all_records.front());

		// now disambiguate
		match.disambiguate(test_ratio, num_threads);
		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);

		if ( round == 1 )
			match.disambig_assignee("/media/data/edwardspace/workplace/testcpp/Disambiguation/assignee_disambig.txt");

	}



	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( all_rec_pointers, num_coauthors_to_group );

	const unsigned int topNcoauthor = 8;

	cLatitude::deactivate_comparator();
	cCoauthor::activate_comparator();
	cAssignee::deactivate_comparator();


	for ( ; round <= max_round_by_coauthor; ++round ) {
		std::cout << std::endl;
		std::cout << "====================== ROUND " << round << " ===========================" << std::endl;
		std::cout << std::endl;
		/*
		vector<string> blocking_column_names;
		switch (round) {
		case 4:
			blocking_column_names.push_back(cAssignee::static_get_class_name() );
			blocking_column_names.push_back(cCity::static_get_class_name());
			break;
		case 5:
			blocking_column_names.push_back(cAsgNum::static_get_class_name() );
			blocking_column_names.push_back(cCity::static_get_class_name());
			break;

		default:
			throw cException_Other("Invalid round!");
		}
		*/

		sprintf(xset01, "/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01_%d.txt", round);
		sprintf(tset05, "/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05_%d.txt", round);
		sprintf(ratiofile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio_%d.txt", round);
		sprintf(matchfile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_%d.txt", round);

		//vector <const cString_Manipulator*> pstring_oper;
		//pstring_oper.push_back(& operator_no_change);
		//pstring_oper.push_back(& operator_no_change);

		//cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names);

		match.reset_blocking(blocker, oldmatchfile);
		//now training
		match.output_list(record_pointers);

		const string training_changable [] = { xset01, tset05 };
		const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));
		make_changable_training_sets_by_assignee( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);

		//now obtaining ratios
		blocker_coauthor.build_uid2uinv_tree(match);
		for ( list<const cRecord *>::iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p )
			(*p)->reset_coauthors(blocker_coauthor, topNcoauthor);

		match.reset_blocking(blocker_coauthor, oldmatchfile);

		cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );
		personalinfo.prepare(training_changable_vec.at(0).c_str(),
							 training_changable_vec.at(1).c_str() );

		patentinfo.prepare(training_stable_vec.at(0).c_str(),
						   training_stable_vec.at(1).c_str() );

		vector < const cRatioComponent *> component_vector;
		component_vector.push_back(&patentinfo);
		component_vector.push_back(&personalinfo);

		cRatios test_ratio(component_vector, ratiofile, all_records.front());




		// now disambiguate
		match.disambiguate(test_ratio, num_threads);
		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);

	}

	dump_match ( "/media/data/edwardspace/workplace/testcpp/Disambiguation/mini_to_dump.sqlite3", "invpat",
							matchfile, string("Invnum"), string("Invnum_N") );


	clear_records(all_records);
	return 0;
}


//===============================================================================================================
//==============================================================================================================
//===============================================================================================================

int fullrun_iterative_v2() {
	const unsigned int num_threads = 4;
	std::cout << std::endl;
	std::cout << "====================== STARTING DISAMBIGUATION ===========================" << std::endl;
	std::cout << std::endl;
	cCoauthor::deactivate_comparator();
	list <cRecord> all_records;
	const char * filename2 = "/media/data/edwardspace/workplace/testcpp/Disambiguation/invpat.txt";
	const string columns[] = {"Firstname", "Lastname", "Unique_Record_ID", "Middlename", "Longitude",
									"Class", "Latitude", "Coauthor", "Assignee", "AsgNum", "Country", "Street", "ApplyYear", "City", "Patent"};
	const vector <string> column_vec(columns, columns + sizeof(columns)/sizeof(string) );
	map <string, asgdetail> assignee_tree;
	//create_assignee_tree( assignee_tree, "/home/ysun/workdir/alexdisambig/new/single/sqlite_dbs/invpatC.train.sqlite3", "invpat",
		//						string("Assignee"), string("AsgNum"), string("Invnum") );

	const char * assignee_file = "/media/data/edwardspace/workplace/testcpp/Disambiguation/asg.txt";
	//write_asgtree_file(assignee_tree, assignee_file);
	//assignee_tree.clear();
	read_asgtree_file(assignee_tree, assignee_file);

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success) return 1;

	const unsigned int limit = 100000;
	//patent stable
	const string training_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset03_stable.txt",
												"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset02_stable.txt" };
	const vector<string> training_stable_vec ( training_stable, training_stable + sizeof(training_stable)/sizeof(string));
	make_stable_training_sets_by_personal ( all_records, limit, training_stable_vec);


	map <string, const cRecord *> uid_dict;
	const string uid_identifier = cUnique_Record_ID::static_get_class_name();
	create_btree_uid2record_pointer(uid_dict, all_records, uid_identifier);

	//train patent info
	cRatioComponent patentinfo(all_records, uid_dict, string("Patent") );
	patentinfo.prepare(training_stable_vec.at(0).c_str(), training_stable_vec.at(1).c_str() );


	//personal stable
	list < const cRecord * > record_pointers;

	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		record_pointers.push_back(&(*p));
	const string personal_stable [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset01_stable.txt",
												"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset05_stable.txt" };
	const vector<string> personal_stable_vec ( personal_stable, personal_stable + sizeof(personal_stable)/sizeof(string));
	make_stable_training_sets_by_patent ( record_pointers, limit, personal_stable_vec);

	cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );
	personalinfo.prepare(personal_stable_vec.at(0).c_str(), personal_stable_vec.at(1).c_str() );

	vector < const cRatioComponent *> component_vector;
	component_vector.push_back(&patentinfo);
	component_vector.push_back(&personalinfo);

	const char * ratiofile = "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio_stable_by_names.txt";
	//cRatios test_ratio(component_vector, ratiofile, all_records.front());
	cRatios test_ratio( ratiofile );

	const unsigned int max_round_by_name = 3;
	const unsigned int max_round_by_coauthor = 5;

	cCluster_Info match ( uid_dict, true);

	cString_Remove_Space operator_no_space;
	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;
	cString_Remain_Same operator_no_change;


	const unsigned int buff_size = 500;
	char matchfile[buff_size];
	char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/match.txt";
	//char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_1.txt";


	const unsigned int starting_round =  1;
	unsigned int round = starting_round;

	for ( ; round <= max_round_by_name; ++round ) {
		std::cout << std::endl;
		std::cout << "====================== ROUND " << round << " ===========================" << std::endl;
		std::cout << std::endl;
		switch (round) {
		case 1:
			operator_truncate_firstname.set_truncater(0, 0, true);
			operator_truncate_lastname.set_truncater(0, 0, true);
			break;
		case 2:
			operator_truncate_firstname.set_truncater(0, 5, true);
			operator_truncate_lastname.set_truncater(0, 8, true);
			break;
		case 3:
			operator_truncate_firstname.set_truncater(0, 3, true);
			operator_truncate_lastname.set_truncater(0, 5, true);
			break;
		default:
			throw cException_Other("Invalid round!");
		}

		sprintf(matchfile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_%d.txt", round);

		vector <const cString_Manipulator*> pstring_oper;
		pstring_oper.push_back(& operator_truncate_firstname);
		pstring_oper.push_back(& operator_truncate_lastname);

		const string blocking_names[] = {cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
		vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
		cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names);

		match.reset_blocking(blocker, oldmatchfile);
		//now training
		//match.output_list(record_pointers);

		//const string training_changable [] = { xset01, tset05 };
		//const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));
		//make_changable_training_sets_by_names( record_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);

		//now obtaining ratios



		// now disambiguate
		match.disambiguate(test_ratio, num_threads);
		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);

		if ( round == 1 )
			match.disambig_assignee("/media/data/edwardspace/workplace/testcpp/Disambiguation/assignee_disambig.txt");

	}

	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( record_pointers, num_coauthors_to_group );

	const unsigned int topNcoauthor = 8;

	cLatitude::deactivate_comparator();
	cCoauthor::activate_comparator();

	const char *ratiofile_coauthor = "/media/data/edwardspace/workplace/testcpp/Disambiguation/ratio_stable_by_coauthors.txt";
	const string patent_stable_coauthor [] = {"/media/data/edwardspace/workplace/testcpp/Disambiguation/xset03_stable_coauthor.txt",
													"/media/data/edwardspace/workplace/testcpp/Disambiguation/tset02_stable_coauthor.txt" };
	const vector<string> patent_stable_coauthor_vec ( patent_stable_coauthor, patent_stable_coauthor + sizeof(patent_stable_coauthor)/sizeof(string));
	make_stable_training_sets_by_personal ( all_records, limit, patent_stable_coauthor_vec);
	patentinfo.prepare(patent_stable_coauthor_vec.at(0).c_str(), patent_stable_coauthor_vec.at(1).c_str() );

	//cRatios ratio_coauthor(component_vector, ratiofile_coauthor, all_records.front());
	cRatios ratio_coauthor(ratiofile_coauthor);


	for ( ; round <= max_round_by_coauthor; ++round ) {
		std::cout << std::endl;
		std::cout << "====================== ROUND " << round << " ===========================" << std::endl;
		std::cout << std::endl;

		blocker_coauthor.build_uid2uinv_tree(match);
		for ( list<const cRecord *>::iterator p = record_pointers.begin(); p != record_pointers.end(); ++p )
			(*p)->reset_coauthors(blocker_coauthor, topNcoauthor);

		sprintf(matchfile, "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_%d.txt", round);

		match.reset_blocking(blocker_coauthor, oldmatchfile);

		// now disambiguate
		match.disambiguate(ratio_coauthor, num_threads);
		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);

	}

	dump_match ( "/media/data/edwardspace/workplace/testcpp/Disambiguation/mini_to_dump.sqlite3", "invpat",
							matchfile, string("Invnum"), string("Invnum_N") );


	clear_records(all_records);
	return 0;
}

#endif

//******************************************************************************
//******************************************************************************
//******************************************************************************

int fullrun_iterative_v4() {
	const bool do_not_train_stable = false;
	const bool use_available_ratios = false;
	const string working_dir = "/media/data/edwardspace/workplace/testcpp/Disambiguation";
	const unsigned int buff_size = 500;
	const unsigned int num_threads = 4;
	std::cout << std::endl;
	std::cout << "====================== STARTING DISAMBIGUATION ===========================" << std::endl;
	std::cout << std::endl;
	cFirstname::activate_comparator();
	cMiddlename::activate_comparator();
	cLastname::activate_comparator();
	cLatitude::activate_comparator();
	cAssignee::activate_comparator();
	cClass::activate_comparator();
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

	bool is_success = fetch_records_from_txt(all_records, filename2, column_vec, assignee_tree);
	if (not is_success) return 1;

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
	cRatioComponent patentinfo(all_records, uid_dict, string("Patent") );
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
	const unsigned int max_round_by_coauthor = 5;

	cCluster_Info match ( uid_dict, true);

	cString_Remove_Space operator_no_space;
	cString_NoSpace_Truncate operator_truncate_firstname;
	cString_NoSpace_Truncate operator_truncate_lastname;
	cString_Remain_Same operator_no_change;



	char xset01[buff_size], tset05[buff_size], ratiofile[buff_size], matchfile[buff_size], stat_patent[buff_size], stat_personal[buff_size];
	char oldmatchfile[buff_size], debug_block_file[buff_size];
	sprintf(oldmatchfile, "%s/match_cons.txt", working_dir.c_str() );
	//char oldmatchfile[buff_size] = "/media/data/edwardspace/workplace/testcpp/Disambiguation/newmatch_1.txt";


	const unsigned int starting_round =  1;
	unsigned int round = starting_round;

	list < const cRecord *> all_rec_pointers;
	for ( list<cRecord>::const_iterator p = all_records.begin(); p != all_records.end(); ++p )
		all_rec_pointers.push_back(&(*p));

	cRatioComponent personalinfo(all_records, uid_dict, string("Personal") );

	const unsigned int num_coauthors_to_group = 2;
	cBlocking_Operation_By_Coauthors blocker_coauthor( all_rec_pointers, num_coauthors_to_group );

	cCluster::set_reference_patent_tree_pointer( blocker_coauthor.get_patent_tree());
	const unsigned int topNcoauthor = 8;

	/*
	//======preliminary consolidation. very strict conditions.
	vector <string> presort_columns;
	presort_columns.push_back(cFirstname::static_get_class_name());
	presort_columns.push_back(cLastname::static_get_class_name());
	presort_columns.push_back(cAssignee::static_get_class_name());
	presort_columns.push_back(cStreet::static_get_class_name());
	presort_columns.push_back(cCity::static_get_class_name());
	presort_columns.push_back(cCountry::static_get_class_name());
	presort_columns.push_back(cClass::static_get_class_name());

	const vector < const cString_Manipulator *> presort_strman( presort_columns.size(), &operator_no_change);

	const cBlocking_Operation_Multiple_Column_Manipulate presort_blocker(presort_strman, presort_columns);
	match.preliminary_consolidation(presort_blocker, all_rec_pointers);
	match.output_current_comparision_info(oldmatchfile);
	*/

	vector <const cString_Manipulator*> pstring_oper;
	pstring_oper.push_back(& operator_truncate_firstname);
	pstring_oper.push_back(& operator_truncate_lastname);

	const string blocking_names[] = {cFirstname::static_get_class_name(), cLastname::static_get_class_name()};
	vector < string > blocking_column_names(blocking_names, blocking_names + sizeof(blocking_names)/sizeof(string) );
	cBlocking_Operation_Multiple_Column_Manipulate blocker(pstring_oper, blocking_column_names);



	vector < string > prev_train_vec;
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

		//now training
		//match.output_list(record_pointers);

		const string training_changable [] = { xset01, tset05 };
		const vector<string> training_changable_vec ( training_changable, training_changable + sizeof(training_changable)/sizeof(string));


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

			const cBlocking_Operation_Multiple_Column_Manipulate presort_blocker(presort_strman, presort_columns);
			match.preliminary_consolidation(presort_blocker, all_rec_pointers);
			match.output_current_comparision_info(oldmatchfile);
		}

			operator_truncate_firstname.set_truncater(0, 0, true);
			operator_truncate_lastname.set_truncater(0, 0, true);
			match.reset_blocking(blocker, oldmatchfile);
			if ( ! use_available_ratios )
				make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;
		case 2:
			operator_truncate_firstname.set_truncater(0, 5, true);
			operator_truncate_lastname.set_truncater(0, 8, true);
			match.reset_blocking(blocker, oldmatchfile);
			if ( ! use_available_ratios )
				make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;
		case 3:
			operator_truncate_firstname.set_truncater(0, 3, true);
			operator_truncate_lastname.set_truncater(0, 5, true);
			match.reset_blocking(blocker, oldmatchfile);
			if ( ! use_available_ratios )
				make_changable_training_sets_by_names( all_rec_pointers, blocking_column_names, pstring_oper, limit,  training_changable_vec);
			//prev_train_vec = training_changable_vec;
			//personalinfo.prepare(training_changable_vec.at(0).c_str(),
			//					 training_changable_vec.at(1).c_str() );
			break;

		case 4: case 5:
			cLatitude::deactivate_comparator();
			cCoauthor::activate_comparator();
			cAssignee::deactivate_comparator();

			operator_truncate_firstname.set_truncater(0, 3, true);
			operator_truncate_lastname.set_truncater(0, 5, true);
			match.reset_blocking(blocker, oldmatchfile);
			sprintf( xset01, "%s/xset01_%d.txt", working_dir.c_str(), round - 1 );
			sprintf( tset05, "%s/tset05_%d.txt", working_dir.c_str(), round - 1 );
			prev_train_vec.clear();
			prev_train_vec.push_back(string(xset01));
			prev_train_vec.push_back(string(tset05));

			for ( unsigned int i = 0; i < training_changable_vec.size(); ++i ) {
				copyfile(training_changable_vec.at(i).c_str(), prev_train_vec.at(i).c_str());
			}

			blocker_coauthor.build_uid2uinv_tree(match);
			for ( list<const cRecord *>::iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p )
				(*p)->reset_coauthors(blocker_coauthor, topNcoauthor);

			//match.reset_blocking(blocker_coauthor, oldmatchfile);
			//match.reset_blocking(blocker, oldmatchfile);
			//patentinfo.prepare(training_stable_vec.at(0).c_str(),
			//				   training_stable_vec.at(1).c_str() );
			break;
		default:
			throw cException_Other("Invalid round.");
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

		match.disambiguate(*ratio_pointer, num_threads, debug_block_file);

		delete ratio_pointer;

		match.output_current_comparision_info(matchfile);

		strcpy ( oldmatchfile, matchfile);

		//if ( round == 1 )
		//	match.disambig_assignee( string( working_dir + string( "/assignee_disambig.txt") ).c_str());

	}

	//dump_match ( string( working_dir + string ( "/mini_to_dump.sqlite3")  ).c_str(), "invpat",
	//						matchfile, string("Invnum"), string("Invnum_N") );


	//clear_records(all_records);
	return 0;
}




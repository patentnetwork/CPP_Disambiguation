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

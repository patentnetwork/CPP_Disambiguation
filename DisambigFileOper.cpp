/*
 *  DisambigFileOper.cpp
 *  mydisambiguation
 *
 *  Created by Katelin on 10-12-27.
 *  Copyright 2010 Dartmouth College. All rights reserved.
 *
 */

#include "DisambigFileOper.h"
#include "DisambigEngine.h"
#include "sqlite3op.h"
#include "DisambigCluster.h"
#include <fstream>
#include <cstring>


const char cSql3query::nullchar;

const cRecord * retrieve_record_pointer_by_unique_id(const string & uid, const map <string, const cRecord*> & uid_tree) {
	map <string, const cRecord *>::const_iterator cpm = uid_tree.find(uid);
	if ( cpm == uid_tree.end())
		throw cException_Attribute_Not_In_Tree(uid.c_str());
	else
		return cpm->second;
}

void create_btree_uid2record_pointer(map<string, const cRecord *> & uid_tree, const list<cRecord> & reclist, const string& uid_name ) {
	uid_tree.clear();
	const unsigned int uid_index = cRecord::get_index_by_name(uid_name);
	cException_Vector_Data except(uid_name.c_str());
	map <string, const cRecord *>::iterator pm;
	for ( list<cRecord>::const_iterator p = reclist.begin(); p != reclist.end(); ++p ) {
		const cAttribute * pattrib = p->get_attrib_pointer_by_index(uid_index);
		//if ( pattrib->get_data().size() != 1 )
		//	throw except;
		const string & label = * pattrib->get_data().at(0);
		pm = uid_tree.find( label );
		if ( pm != uid_tree.end())
			throw cException_Duplicate_Attribute_In_Tree(label.c_str());
		uid_tree.insert(std::pair< string, const cRecord *>(label, &(*p)));
	}
}

//================================================

bool dump_match ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name) {


	sqlite3* pDB;
	int sqlres;
	std::cout << "Dumping " << txt_source << " to file << " << sqlite3_target << " >>, tablename << " << tablename << " >> ......" << std::endl;


	sqlres = sqlite3_open_v2(sqlite3_target,&pDB,SQLITE_OPEN_READWRITE ,NULL);
	if (SQLITE_OK != sqlres ) {
		std::cout << "SQL DB open error." <<sqlres<< std::endl;
		throw cException_SQLITE3();
	}

	std::ifstream::sync_with_stdio(false);
	std::ifstream infile(txt_source);
	const unsigned int primary_delim_size = strlen(cCluster_Info::primary_delim);
	const unsigned int secondary_delim_size = strlen(cCluster_Info::secondary_delim);
	map < string, string > update_dict;
	map < string, string >::iterator pm;

	if (infile.good()) {
		string filedata;
		register size_t pos, prev_pos;
		while ( getline(infile, filedata)) {
			pos = prev_pos = 0;
			pos = filedata.find(cCluster_Info::primary_delim, prev_pos);
			string valuestring = filedata.substr( prev_pos, pos - prev_pos);
			prev_pos = pos + primary_delim_size;

			pos = filedata.find(cCluster_Info::primary_delim, prev_pos);
			prev_pos = pos + primary_delim_size;


			while ( ( pos = filedata.find(cCluster_Info::secondary_delim, prev_pos) )!= string::npos){
				string keystring = filedata.substr( prev_pos, pos - prev_pos);
				pm = update_dict.find(keystring);
				if ( pm != update_dict.end() )
					throw cException_Duplicate_Attribute_In_Tree(keystring.c_str());
				update_dict.insert(std::pair<string,string>(keystring, valuestring));
				prev_pos = pos + secondary_delim_size;
			}
		}
		std::cout << txt_source << " is ready to be dumped into "<< sqlite3_target << std::endl;
	}
	else {
		throw cException_File_Not_Found(txt_source);
	}

	sqlite3_exec(pDB, "BEGIN TRANSACTION", NULL, NULL, NULL);
	std::ifstream::sync_with_stdio(true);

	const unsigned int buff_size = 512;
	char buffer[buff_size];
	sqlite3_stmt *statement;
	sqlite3_exec(pDB, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
	sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s on %s(%s);", unique_record_name.c_str(), tablename, tablename, unique_record_name.c_str());
	std::cout << "Creating index ......" << std::endl;
	sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
	std::cout << "Index created." << std::endl;


	sprintf(buffer, "UPDATE %s set %s = @VAL WHERE %s = @KEY;", tablename, unique_inventor_name.c_str(), unique_record_name.c_str());
	sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);
	if ( sqlres != SQLITE_OK )
		throw cException_SQLITE3();
	//char *zSQL;
	const unsigned int base = 100000;
	unsigned int count = 0;
	for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
		sqlite3_bind_text(statement, 1, cpm->second.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(statement, 2, cpm->first.c_str(), -1, SQLITE_TRANSIENT);

		sqlres = sqlite3_step(statement);
		if ( sqlres != SQLITE_DONE )
			throw cException_SQLITE3();
		sqlite3_clear_bindings(statement);
		sqlite3_reset(statement);
		++count;
		if ( count % base == 0 )
			std::cout << count << " records has been updated. " << std::endl;
	}

	sqlite3_exec(pDB, "END TRANSACTION", NULL, NULL, NULL);
	sqlite3_finalize(statement);
	sqlite3_close(pDB);

	std::cout << "Dumping complete. " << std::endl;
	return true;
}


#if 0
//================================================================== OBSOLETE ======================
bool fetch_records_from_sqlite3(list <cRecord> & source, const char * sql_file, const char * statement, const map<string, asgdetail>& asgtree){

	sqlite3 *pDB;
	int sqlres;

	sqlres = sqlite3_open_v2(sql_file,&pDB,SQLITE_OPEN_READONLY,NULL);
	if (SQLITE_OK != sqlres ) {
		std::cout << "SQL DB open error." <<sqlres<< std::endl;
		return false;
	}
	sqlite3_exec (pDB, "PRAGMA synchronous = OFF", NULL, NULL, NULL);

	register cSql3query query(pDB, statement);
	//cDisambigRecord::column_names = query.column_name;
	const unsigned int num_cols = query.column_name.size();
	//const unsigned int num_cols = vec_attribs.size();
	cRecord::column_names = query.column_name;

	cAttribute ** pointer_array = new cAttribute *[num_cols];

	for ( unsigned int i = 0; i < num_cols; ++i ) {
		if ( cRecord::column_names[i] == cFirstname::class_name ) {
			cFirstname::enable();
			pointer_array[i] = new cFirstname();
			cFirstname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLastname::class_name ) {
			cLastname::enable();
			pointer_array[i] = new cLastname();
			cLastname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cMiddlename::class_name ) {
			cMiddlename::enable();
			pointer_array[i] = new cMiddlename();
			cMiddlename::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLatitude::class_name ) {
			cLatitude::enable();
			pointer_array[i] = new cLatitude();
			cLatitude::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLongitude::class_name ) {
			cLongitude::enable();
			pointer_array[i] = new cLongitude();
			cLongitude::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cStreet::class_name ) {
			cStreet::enable();
			pointer_array[i] = new cStreet();
			cStreet::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cCountry::class_name ) {
			cCountry::enable();
			pointer_array[i] = new cCountry();
			cCountry::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cClass::class_name ) {
			cClass::enable();
			pointer_array[i] = new cClass();
			cClass::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cCoauthor::class_name ) {
			cCoauthor::enable();
			pointer_array[i] = new cCoauthor();
			cCoauthor::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cAssignee::class_name ) {
			cAssignee::enable();
			pointer_array[i] = new cAssignee();
			cAssignee::column_index_in_query = i;
			cAssignee::set_assignee_tree_pointer (asgtree);
		}
		else if ( cRecord::column_names[i] == cAsgNum::class_name ) {
			cAsgNum::enable();
			pointer_array[i] = new cAsgNum();
			cAsgNum::column_index_in_query = i;
			cAssignee::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cUnique_Record_ID::class_name ) {
			cUnique_Record_ID::enable();
			pointer_array[i] = new cUnique_Record_ID();
			cUnique_Record_ID::column_index_in_query = i;
		}
		else {
			delete [] pointer_array;
			throw cException_ColumnName_Not_Found(cRecord::column_names[i].c_str());
		}
	}
	cLatitude::static_check_interactive_consistency( cRecord::column_names );	// always do this for all the attribute classes.

	vector <const char *> string_cache(num_cols);

	unsigned long size = 0;
	std::cout << "Reading " << sql_file << " ......"<< std::endl;

	unsigned int base = 100000;
	const cAttribute * pAttrib;
	vector <const cAttribute *> temp_vec_attrib;
	vector <const cAttribute *> Latitude_interactive_attribute_pointers;
	while (SQLITE_DONE != query.next() ) {
		temp_vec_attrib.clear();
		for ( unsigned int i = 0; i < num_cols ; ++i ) {
			string_cache[i] = (const char *) query.get_string_data(i);
			if (string_cache[i] == NULL)
				string_cache[i] = &cSql3query::nullchar;
			pointer_array[i]->reset_data(string_cache[i]);
			pAttrib = pointer_array[i]->clone();	//HERE CREATED NEW CLASS INSTANCES.
			temp_vec_attrib.push_back(pAttrib);
		}
		//now do some reset correlation vector work

		Latitude_interactive_attribute_pointers.clear();
		for (vector <unsigned int>::const_iterator pp = cLatitude::interactive_column_indice_in_query.begin(); pp != cLatitude::interactive_column_indice_in_query.end(); ++pp )
			Latitude_interactive_attribute_pointers.push_back(temp_vec_attrib.at(*pp));
		cAttribute * changesomething = const_cast<cAttribute*> (temp_vec_attrib.at(cLatitude::column_index_in_query));
		changesomething->reset_interactive( Latitude_interactive_attribute_pointers );

		// end of correlation reset

		cRecord temprec(temp_vec_attrib);
		source.push_back(temprec);
		++size;
		if ( size % base == 0 )
			std::cout << size << " records obtained." << std::endl;
	}
	std::cout << std::endl;
	std::cout << size << " records have been fetched from sqlite3 file." << std::endl;

	for ( unsigned int i = 0; i < num_cols; ++i )
		delete pointer_array[i];
	delete [] pointer_array;

	return true;
}
#endif

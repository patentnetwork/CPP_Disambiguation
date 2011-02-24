/*
 *  DisambigFileOper.h
 *  mydisambiguation
 *
 *  Created by Katelin on 10-12-27.
 *  Copyright 2010 Dartmouth College. All rights reserved.
 *
 */
#ifndef _DISAMBIG_FILE_OPERATIONS_H_
#define _DISAMBIG_FILE_OPERATIONS_H_

#include <iostream>
#include <map>
#include <set>
#include <list>



using std::string;
using std::set;
using std::map;
using std::list;

class cRecord;

const cRecord * retrieve_record_pointer_by_unique_id(const string & uid, const map <string, const cRecord*> & uid_tree);
typedef std::pair<string, unsigned int> asgdetail;
void write_asgtree_file(const map<string, asgdetail> & asgtree, const char * filename);
void read_asgtree_file(map<string, asgdetail> & asgtree, const char * filename);
void create_assignee_tree(map<string, asgdetail> & asgtree, const char * sql_file, const char * tablename,
						  const string & assignee_label, const string & asgnum_label, const string & uniqueid_label);

void create_btree_uid2record_pointer(map<string, const cRecord *> & uid_tree, const list<cRecord> & reclist, const string& uid_name );
bool dump_match ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name);






#endif

/*
 *  DisambigFileOper.h
 *  mydisambiguation
 *
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
void create_btree_uid2record_pointer(map<string, const cRecord *> & uid_tree, const list<cRecord> & reclist, const string& uid_name );
bool dump_match ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name);






#endif

/*
 * txt2sqlite3.h
 *
 *  Created on: Mar 25, 2011
 *      Author: ysun
 */

#ifndef TXT2SQLITE3_H_
#define TXT2SQLITE3_H_

extern "C" {
	#include <sqlite3.h>
}
#include <string>
using std::string;

bool stepwise_dump ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name);
bool dump_match ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name);
bool stepwise_add_column ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name);






#endif /* TXT2SQLITE3_H_ */

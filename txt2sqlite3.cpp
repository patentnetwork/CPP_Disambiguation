/*
 * txt2sqlite3.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: ysun
 */

#include "txt2sqlite3.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
using std::string;
using std::map;

static const char * primary_delim = "###";
static const char * secondary_delim = ",";



int main( const int argc, const char * argv[] ) {

	if ( argc != 6 ) {
		std::cout << "Invalid number of parameters. Should be 5 parameters." << std::endl;
		std::cout << "Usage: ./txt2sqlite3 target.sqlite3 tablename source.txt UNIQUE_RECORD_COLUMN_NAME UNIQUE_INVENTOR_COLUMN_NAME" << std::endl
				<< " ----------> dump the source.txt into target.sqlite3.table with unique_record_id = UNIQUE_RECORD_COLUMN_NAME" << std::endl
					<<	"  and unique_inventor_id = UNIQUE_INVENTOR_COLUMN_NAME." << std::endl;
		return 1;
	}

	const char * targetsqlite3 = argv[1];
	const char * tablename = argv[2];
	const char * sourcetxt = argv[3];
	const string unique_record_id ( argv[4]);
	const string unique_inventor_id( argv[5]);

	stepwise_add_column(targetsqlite3, tablename , sourcetxt, unique_record_id, unique_inventor_id);


	return 0;
}






























#if 0



bool dump_match ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name) {


	sqlite3* pDB;
	int sqlres;
	std::cout << "Dumping " << txt_source << " to file << " << sqlite3_target << " >>, tablename << " << tablename << " >> ......" << std::endl;


	sqlres = sqlite3_open_v2(sqlite3_target,&pDB,SQLITE_OPEN_READWRITE ,NULL);
	if (SQLITE_OK != sqlres ) {
		std::cout << "SQL DB open error." <<sqlres<< std::endl;
		return false;
	}

	std::ifstream::sync_with_stdio(false);
	std::ifstream infile(txt_source);
	const unsigned int primary_delim_size = strlen(primary_delim);
	const unsigned int secondary_delim_size = strlen(secondary_delim);
	map < string, string > update_dict;
	map < string, string >::iterator pm;

	if (infile.good()) {
		string filedata;
		register size_t pos, prev_pos;
		while ( getline(infile, filedata)) {
			pos = prev_pos = 0;
			pos = filedata.find(primary_delim, prev_pos);
			string valuestring = filedata.substr( prev_pos, pos - prev_pos);
			prev_pos = pos + primary_delim_size;

			pos = filedata.find(primary_delim, prev_pos);
			prev_pos = pos + primary_delim_size;


			while ( ( pos = filedata.find(secondary_delim, prev_pos) )!= string::npos){
				string keystring = filedata.substr( prev_pos, pos - prev_pos);
				pm = update_dict.find(keystring);
				if ( pm != update_dict.end() ) {
					std::cout << "Duplicate records: " << keystring << std::endl;
					return false;
				}
				update_dict.insert(std::pair<string,string>(keystring, valuestring));
				prev_pos = pos + secondary_delim_size;
			}
		}
		std::cout << txt_source << " is ready to be dumped into "<< sqlite3_target << std::endl;
	}
	else {
		std::cout << "File not found: " << txt_source << std::endl;
		return false;
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
	if ( sqlres != SQLITE_OK ) {
		std::cout << "Statement preparation error: " << buffer << std::endl;
		return false;
	}

	//char *zSQL;
	const unsigned int base = 100000;
	unsigned int count = 0;
	for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
		sqlite3_bind_text(statement, 1, cpm->second.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(statement, 2, cpm->first.c_str(), -1, SQLITE_TRANSIENT);

		sqlres = sqlite3_step(statement);
		if ( sqlres != SQLITE_DONE ) {
			std::cout << "Statement step error." << std::endl;
			return false;
		}
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



bool stepwise_dump ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name) {


	sqlite3* pDB;
	int sqlres;
	std::cout << "Dumping " << txt_source << " to file << " << sqlite3_target << " >>, tablename << " << tablename << " >> ......" << std::endl;


	sqlres = sqlite3_open_v2(sqlite3_target,&pDB,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ,NULL);
	if (SQLITE_OK != sqlres ) {
		std::cout << "SQL DB open error." <<sqlres<< std::endl;
		return false;
	}

	std::ifstream::sync_with_stdio(false);
	std::ifstream infile(txt_source);
	const unsigned int primary_delim_size = strlen(primary_delim);
	const unsigned int secondary_delim_size = strlen(secondary_delim);
	map < string, string > update_dict;
	map < string, string >::iterator pm;

	if (infile.good()) {
		string filedata;
		register size_t pos, prev_pos;
		while ( getline(infile, filedata)) {
			pos = prev_pos = 0;
			pos = filedata.find(primary_delim, prev_pos);
			string valuestring = filedata.substr( prev_pos, pos - prev_pos);
			prev_pos = pos + primary_delim_size;

			pos = filedata.find(primary_delim, prev_pos);
			prev_pos = pos + primary_delim_size;


			while ( ( pos = filedata.find(secondary_delim, prev_pos) )!= string::npos){
				string keystring = filedata.substr( prev_pos, pos - prev_pos);
				pm = update_dict.find(keystring);
				if ( pm != update_dict.end() ) {
					std::cout << "Duplicate records: " << keystring << std::endl;
					return false;
				}
				update_dict.insert(std::pair<string,string>(keystring, valuestring));
				prev_pos = pos + secondary_delim_size;
			}
		}
		std::cout << txt_source << " is ready to be dumped into "<< sqlite3_target << std::endl;
	}
	else {
		std::cout << "File not found: " << txt_source << std::endl;
		return false;
	}


	std::ifstream::sync_with_stdio(true);
	sqlite3_exec(pDB, "BEGIN TRANSACTION", NULL, NULL, NULL);

	const unsigned int buff_size = 512;
	char buffer[buff_size];
	sqlite3_stmt *statement;
	sqlite3_exec(pDB, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
	sprintf(buffer, "DROP TABLE IF EXISTS %s;", tablename);
	sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
	sprintf(buffer, "CREATE TABLE %s ( %s, %s );", tablename, unique_record_name.c_str(), unique_inventor_name.c_str() );
	sqlite3_exec(pDB, buffer, NULL, NULL, NULL);



	sprintf(buffer, "INSERT INTO %s (%s, %s ) VALUES ( @KEY, @VAL );", tablename, unique_record_name.c_str(), unique_inventor_name.c_str() );
	sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);
	if ( sqlres != SQLITE_OK ) {
		std::cout << "Statement preparation error: " << buffer << std::endl;
		std::cout << "Maybe the table name is invalid." << std::endl;
		return false;
	}

	//char *zSQL;
	const unsigned int base = 100000;
	unsigned int count = 0;
	for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
		sqlite3_bind_text(statement, 1, cpm->first.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(statement, 2, cpm->second.c_str(), -1, SQLITE_TRANSIENT);

		sqlres = sqlite3_step(statement);
		if ( sqlres != SQLITE_DONE ) {
			std::cout << "Statement step error." << std::endl;
			return false;
		}
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
#endif

bool stepwise_add_column ( const char * sqlite3_target, const char * tablename, const char * txt_source, const string & unique_record_name, const string & unique_inventor_name) {


	sqlite3* pDB;
	int sqlres;
	std::cout << "Dumping " << txt_source << " to file << " << sqlite3_target << " >>, tablename << " << tablename << " >> ......" << std::endl;


	sqlres = sqlite3_open_v2(sqlite3_target,&pDB,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ,NULL);
	if (SQLITE_OK != sqlres ) {
		std::cout << "SQL DB open error." <<sqlres<< std::endl;
		return false;
	}

	std::ifstream::sync_with_stdio(false);
	std::ifstream infile(txt_source);
	const unsigned int primary_delim_size = strlen(primary_delim);
	const unsigned int secondary_delim_size = strlen(secondary_delim);
	map < string, string > update_dict;
	map < string, string >::iterator pm;

	if (infile.good()) {
		string filedata;
		register size_t pos, prev_pos;
		while ( getline(infile, filedata)) {
			pos = prev_pos = 0;
			pos = filedata.find(primary_delim, prev_pos);
			string valuestring = filedata.substr( prev_pos, pos - prev_pos);
			prev_pos = pos + primary_delim_size;

			pos = filedata.find(primary_delim, prev_pos);
			prev_pos = pos + primary_delim_size;


			while ( ( pos = filedata.find(secondary_delim, prev_pos) )!= string::npos){
				string keystring = filedata.substr( prev_pos, pos - prev_pos);
				pm = update_dict.find(keystring);
				if ( pm != update_dict.end() ) {
					std::cout << "Duplicate records: " << keystring << std::endl;
					return false;
				}
				update_dict.insert(std::pair<string,string>(keystring, valuestring));
				prev_pos = pos + secondary_delim_size;
			}
		}
		std::cout << txt_source << " is ready to be dumped into "<< sqlite3_target << std::endl;
	}
	else {
		std::cout << "File not found: " << txt_source << std::endl;
		return false;
	}
	std::ifstream::sync_with_stdio(true);

	const unsigned int base = 100000;
	unsigned int count = 0;



	const unsigned int buff_size = 512;
	char buffer[buff_size];
	sqlite3_stmt *statement;

	const string commands [] = { "PRAGMA synchronous = OFF;",
								"PRAGMA cache_size = 8000; ",
								"PRAGMA temp_store = MEMORY; ",
								"PRAGMA journal_mode = MEMORY; ",
								"PRAGMA page_size = 8192; "
								};

	for ( unsigned int i = 0; i < sizeof(commands)/sizeof(string); ++i ) {
		sqlres = sqlite3_exec(pDB, commands[i].c_str(), NULL, NULL, NULL);
		if ( SQLITE_OK != sqlres )
			std::cout << "Failed: ";
		else
			std::cout << "Success: ";
		std::cout << commands[i] << std::endl;
	}


	sprintf( buffer, "CREATE TABLE %s ( %s) ;", tablename, unique_record_name.c_str());
	sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
	if ( SQLITE_OK != sqlres ) {
		//std::cout  << tablename << " already exists."<< std::endl;
		//return 2;
	}
	else {
		std::cout << tablename << " is created." << std::endl;
		sprintf ( buffer, "INSERT INTO %s VALUES (@KEY);", tablename );
		sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);
		if ( sqlres != SQLITE_OK ) {
			std::cout << "Statement preparation error: " << buffer << std::endl;
			std::cout << "Maybe the table name is invalid." << std::endl;
			return false;
		}

		sqlite3_exec(pDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
			sqlite3_bind_text(statement, 1, cpm->first.c_str(), -1, SQLITE_TRANSIENT);

			sqlres = sqlite3_step(statement);
			if ( sqlres != SQLITE_DONE ) {
				std::cout << "Statement step error." << std::endl;
				return false;
			}
			sqlite3_clear_bindings(statement);
			sqlite3_reset(statement);
			++count;
			if ( count % base == 0 )
				std::cout << count << " records has been initialized. " << std::endl;
		}
		sqlite3_exec(pDB, "END TRANSACTION;", NULL, NULL, NULL);
		std::cout << tablename << " is initialized." << std::endl;
	}


	sprintf(buffer, "CREATE UNIQUE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;", unique_record_name.c_str(), tablename, tablename, unique_record_name.c_str() );
	sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
	if ( SQLITE_OK != sqlres ) {
		std::cout << "Column " << unique_record_name << " does not exist. Adding column ..........   ";
		sprintf(buffer, "ALTER TABLE %s ADD COLUMN %s ;", tablename, unique_record_name.c_str() );
		sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
		if ( SQLITE_OK != sqlres ) {
			std::cout <<  std::endl << " Error in adding column " << unique_record_name << std::endl;
			return 2;
		}
		std::cout << "Done." << std::endl;
		sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;", unique_record_name.c_str(), tablename, tablename, unique_record_name.c_str() );
		sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
		if ( SQLITE_OK != sqlres ) {
			std::cout <<  " Error in adding index " << unique_record_name << std::endl;
			return 3;
		}
	}

	sprintf(buffer, "SELECT %s from %s; ", unique_inventor_name.c_str(), tablename);
	//sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;", unique_inventor_name.c_str(), tablename, tablename, unique_inventor_name.c_str() );
	sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
	if ( SQLITE_OK != sqlres ) {
		std::cout << "Column " << unique_inventor_name << " does not exist. Adding column ..........   ";
		sprintf(buffer, "ALTER TABLE %s ADD COLUMN %s ;", tablename, unique_inventor_name.c_str() );
		sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
		if ( SQLITE_OK != sqlres ) {
			std::cout << std::endl << " Error in adding column " << unique_inventor_name << std::endl;
			return 2;
		}
		std::cout << "Done." << std::endl;
		/*
		sprintf(buffer, "CREATE INDEX IF NOT EXISTS index_%s_on_%s ON %s(%s) ;", unique_inventor_name.c_str(), tablename, tablename, unique_inventor_name.c_str() );
		sqlres = sqlite3_exec(pDB, buffer, NULL, NULL, NULL);
		if ( SQLITE_OK != sqlres ) {
			std::cout <<  " Error in adding index " << unique_inventor_name << std::endl;
			return 3;
		}
		*/
	}


	sprintf(buffer, "UPDATE %s set %s = @VAL WHERE %s = @KEY;", tablename, unique_inventor_name.c_str(), unique_record_name.c_str());
	sqlres = sqlite3_prepare_v2(pDB,  buffer, -1, &statement, NULL);
	if ( sqlres != SQLITE_OK ) {
		std::cout << "Statement preparation error: " << buffer << std::endl;
		std::cout << "Maybe the table name is invalid." << std::endl;
		return false;
	}

	//char *zSQL;
	count = 0;

	sqlite3_exec(pDB, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	for ( map<string, string>::const_iterator cpm = update_dict.begin(); cpm != update_dict.end(); ++cpm) {
		sqlite3_bind_text(statement, 2, cpm->first.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(statement, 1, cpm->second.c_str(), -1, SQLITE_TRANSIENT);

		sqlres = sqlite3_step(statement);
		if ( sqlres != SQLITE_DONE ) {
			std::cout << "Statement step error." << std::endl;
			return false;
		}
		sqlite3_clear_bindings(statement);
		sqlite3_reset(statement);
		++count;
		if ( count % base == 0 )
			std::cout << count << " records has been updated. " << std::endl;
	}

	sqlite3_exec(pDB, "END TRANSACTION;", NULL, NULL, NULL);
	sqlite3_finalize(statement);
	sqlite3_close(pDB);

	std::cout << "Dumping complete. " << std::endl;
	return true;
}







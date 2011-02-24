/*
 * sqlite3op.h
 *
 *  Created on: Aug 26, 2010
 *      Author: ysun
 */

#ifndef SQLITE3OP_H_
#define SQLITE3OP_H_
#include <iostream>
#include <vector>

extern "C" {
#include <sqlite3.h>
}

class cSql3query {
	private:
		//static const int MAX_LENGTH_SQLSTMT = 300;

    	sqlite3 *pDB;
    	sqlite3_stmt* pstmt;

	public:
		static const int npos = -1;
		static const char nullchar = '\0';
    	std::vector <std::string> column_name;
    	explicit cSql3query (sqlite3*, const char*);
    	bool get_col_name();
		//std::vector <std::string> output_column_names();
    	int get_col_index(const char *);
    	const unsigned char * get_string_data(int);
    	const unsigned char * get_string_data(const char*);
    	int get_int_data(int);
    	int get_int_data(const char*);
    	int next();
    	~cSql3query();
};

class cException_SQLITE3 : public std::exception {};




#endif /* SQLITE3OP_H_ */

#include "sqlite3op.h"
#include <iostream>
#include <vector>
#include <cstdlib>


cSql3query::cSql3query (sqlite3* pinputDB, const char* sql_query): pDB(pinputDB){
	//const char * p_unused_chars;
	//int sqlres = sqlite3_prepare_v2(pDB, sql_query, MAX_LENGTH_SQLSTMT, &pstmt, &p_unused_chars );
	int sqlres = sqlite3_prepare_v2(pDB, sql_query, -1, &pstmt, NULL );
	if (SQLITE_OK != sqlres ) {
		std::cout << "Statement preparation error: " << sql_query<< std::endl;
		throw cException_SQLITE3();
	}
	get_col_name();
}

bool cSql3query::get_col_name() {
	column_name.clear();
	int count = sqlite3_column_count(pstmt);
	if (0 == count)
		return false;
	for (int i = 0; i < count;++i)
		column_name.push_back(std::string(sqlite3_column_name(pstmt, i)));
	return true;
}

int cSql3query::get_col_index(const char * input_column_name){
	const std::string cname(input_column_name);
	for (unsigned int i = 0; i < column_name.size(); ++i) {
		if (cname == column_name[i])
			return i;
	}
	return npos;
}
cSql3query::~cSql3query(){
	sqlite3_finalize(pstmt);
	pDB = NULL;
	pstmt = NULL;
}

const unsigned char * cSql3query::get_string_data(int input_index){
	/*
	const int type = sqlite3_column_type(pstmt, input_index);
	if (SQLITE_TEXT != type ) {
		std::cout << "Get_string_data error: invalid index "<< input_index <<" or wrong data type!" <<std::endl;
		std::cout << "column " <<input_index<< " has the data type of "<< sqlite3_column_type(pstmt, input_index) <<" defined in sqlite3"<<std::endl;
		return NULL;
		//return sqlite3_column_text(pstmt, input_index);
	}
	else
	 */
		return sqlite3_column_text(pstmt, input_index);
}

const unsigned char * cSql3query::get_string_data(const char *input_colname){
	return this->get_string_data(this->get_col_index(input_colname));
}

int cSql3query::get_int_data(int input_index) {
	if (SQLITE_INTEGER != sqlite3_column_type(pstmt, input_index) ) {
		std::cout << "Get_int_data error: invalid index or wrong data type!" <<std::endl;
		return NULL;
	}
	else
		return sqlite3_column_int(pstmt, input_index);
}
int cSql3query::get_int_data(const char* input_colname) {
	return this->get_int_data(this->get_col_index(input_colname));
}

int cSql3query::next() {
	return sqlite3_step(pstmt);
}

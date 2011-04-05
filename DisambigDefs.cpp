/*
 * DisambigLib.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: ysun
 */

#include <iostream>
#include <cstdlib>
#include "DisambigDefs.h"
#include "sqlite3op.h"
#include <algorithm>
#include <typeinfo>
extern "C" {
	#include "strcmp95.h"
}
#include "DisambigComp.h"
using std::list;
using std::string;



const char cSql3query::nullchar;
const vector <const cAttribute *> cAttribute::empty_interactive(0);
const cSimilarity_Compare::cException_Different_Similarity_Dimensions cSimilarity_Compare::default_sp_exception("Error: Different Similarity profile dimensions");


bool cAttribute::split_string(const char* recdata) {
	static const string emptystring ("");
	const char * p = recdata;
	const char * pend = p + strlen(recdata);
	if ( pend == p ) {
		//data.push_back(string(""));
		data.push_back( this->add_string(emptystring));
		return true;
	}

	char string_count_cache[10];
	const char delim = '/';
	const char secondary_delim = '~';
	const char * q;
	unsigned int count_length;
	while ( (q = std::find(p, pend, delim)) != pend ) {
		// r points to the secondary delimiter
		// q points to the primary delimiter
		const char * r = std::find(p, q, secondary_delim);
		const string temp ( p, r );
		data.push_back(this->add_string(temp));
		if ( r != q ) {
			count_length = q - r - 1;
			memcpy(string_count_cache, r + 1, count_length *sizeof(char) );
			*(string_count_cache + count_length ) = '\0';
			//data_count.push_back(atoi(string_count_cache));
		}
		else {
			//data_count.push_back( 0 );
		}
		p = q + 1;
	}
	const char * r = std::find(p, q, secondary_delim);
	const string tp ( p, r);
	const string * ptr = this->add_string(tp);
	data.push_back( ptr );
	if ( r != q ) {
		count_length = q - r - 1;
		memcpy(string_count_cache, r + 1, count_length *sizeof(char) );
		*(string_count_cache + count_length ) = '\0';
		//data_count.push_back(atoi(string_count_cache));
	}
	else {
		//data_count.push_back( 0 );
	}
	
	// now use swap trick to minimize the volumn of each attribute. Effective STL by Scott Meyers, Item 17
	vector< const string* > (data).swap(data);
	//vector<unsigned int> (data_count).swap(data_count);

	
	
	if ( data.size() > 1 )
		throw cException_Vector_Data(recdata);
	


	return true;
}




void attrib_merge ( list < const cAttribute * *> & l1, list < const cAttribute * *> & l2 ) {
	static const string errmsg = "Error: attribute pointers are not pointing to the same object. Attribute Type = ";
	if ( l1.empty() || l2.empty() )
		return;

	const cAttribute * new_object_pointer = (*l1.front())->attrib_merge(** l2.front());
	if ( new_object_pointer == NULL )
		return;

	for ( list < const cAttribute * * >::const_iterator p = l1.begin(); p != l1.end(); ++p ) {
		if ( **p != * l1.front() ) {
			std::cout << "------------" <<std::endl;
			std::cout << "Front address: " << *l1.front() << " Other address: " << **p << std::endl;
			(*l1.front())->print();
			(**p)->print();
			std::cout << "------------" << std::endl;
			throw cException_Other ( ( errmsg + (*l1.front())->get_class_name()).c_str() ) ;
		}
	}

	for ( list < const cAttribute *  * >::const_iterator p = l2.begin(); p != l2.end(); ++p ) {
		if ( **p != *l2.front() ) {
			std::cout << "------------" <<std::endl;
			std::cout << "Front address: " << *l2.front() << " Other address: " << **p << std::endl;
			(*l2.front())->print();
			(**p)->print();
			std::cout << "------------" << std::endl;
			throw cException_Other ( ( errmsg + (*l2.front())->get_class_name()).c_str() );
		}
	}


	const unsigned int l1_size = l1.size();
	const unsigned int l2_size = l2.size();

	if ( l1_size != 1 )
		(*l1.front())->reduce_attrib(l1_size - 1);
	if ( l2_size != 1)
		(*l2.front())->reduce_attrib(l2_size - 1);
	if ( l1_size + l2_size != 2 )
		new_object_pointer->add_attrib( l1_size + l2_size - 2 );

	for ( list < const cAttribute * * >::const_iterator p = l1.begin(); p != l1.end(); ++p )
		**p = new_object_pointer;
	for ( list < const cAttribute *  * >::const_iterator p = l2.begin(); p != l2.end(); ++p )
		**p = new_object_pointer;

}






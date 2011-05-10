/*
 * DisambigLib.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: ysun
 */

#include "DisambigDefs.h"
#include <algorithm>
#include <cstring>

using std::list;
using std::string;

const cSimilarity_Compare::cException_Different_Similarity_Dimensions cSimilarity_Compare::default_sp_exception("Error: Different Similarity profile dimensions");
vector <string> cAttribute::Derived_Class_Name_Registry;



/*
 * This function splits the input string and save it into the attribute object.
 * Legacy format of data is in the form of "DATA1~COUNT1/DATA2~COUNT2/DATA3~COUNT3", so this function splits the string and
 * saves the pointer to "DATA1", 'DATA2', 'DATA3'. There was a counter class member, but it was removed later.
 *
 * One needs to be very familiar with how the strings look like and what they can be in the source text file. Especially the delimiters.
 * However, one is also assumed to have familiarized with the source text format.
 *
 */

bool cAttribute::split_string(const char* recdata) {
	static const string emptystring ("");
	vector < const string * > & data = this->get_data_modifiable();
	const char * p = recdata;
	const char * pend = p + strlen(recdata);
	if ( pend == p ) {
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
		}
		else {
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
	}
	else {
	}
	
	// now use swap trick to minimize the volumn of each attribute. Effective STL by Scott Meyers, Item 17
	vector< const string* > (data).swap(data);
	
	if ( data.size() > 1 )
		throw cException_Vector_Data(recdata);
	
	return true;
}


/*
 * This is a global function, not a class member function!!!!!!!
 * usually for set_mode classes, attrib_merge works, and classes of single_mode do not support such operation.
 * if this function is called, the attributes in list1 and list2 will be merged into a larger attribute object,
 * and the pointers in list1 and list2 will then point to the newly created large object.
 *
 */

void attrib_merge ( list < const cAttribute * *> & l1, list < const cAttribute * *> & l2 ) {
	static const string errmsg = "Error: attribute pointers are not pointing to the same object. Attribute Type = ";
	if ( l1.empty() || l2.empty() )
		return;

	// calls the class member function to check if attrib merge is supported.
	// if Null is returned, it means attrib_merge is not supported.
	// usually for set_mode classes, attrib_merge works, and classes of single_mode do not support such operation.
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

void cAttribute::register_class_names( const vector < string > & input) {
	Derived_Class_Name_Registry = input;
}

int cAttribute::position_in_registry( const string & s ) {
	int i = 0;
	for ( i = 0; i < static_cast<int> (Derived_Class_Name_Registry.size()); ++i ) {
		if ( s == Derived_Class_Name_Registry.at(i))
			return i;
	}
	return -1;
}

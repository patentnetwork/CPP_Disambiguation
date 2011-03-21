/*
 * DisambigLib.hpp
 *
 *  Created on: Dec 7, 2010
 *      Author: ysun
 */

#ifndef DISAMBIGLIB_HPP_
#define DISAMBIGLIB_HPP_

#include <iostream>
#include <list>
#include <cstring>
#include <vector>
#include "DisambigComp.h"
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <pthread.h>
#include <typeinfo>
#define DEBUG



using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

class cRecord;
typedef list<const cRecord*> cGroup_Value;
class cAttribute;
//======================================================================================


class cAbstract_Exception: public std::exception {
private:
	string m_errmsg;
public:
	cAbstract_Exception(const char *errmsg) : m_errmsg(errmsg) {};
	const char * what() const throw() {return m_errmsg.c_str();}
	~cAbstract_Exception() throw() {};
};

class cException_Diff_Attribute : public cAbstract_Exception{};
class cException_No_Comparision_Function : public cAbstract_Exception {
public:
	cException_No_Comparision_Function(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_ColumnName_Not_Found : public cAbstract_Exception {
public:
	cException_ColumnName_Not_Found(const char* errmsg): cAbstract_Exception(errmsg) {};
};


class cException_Insufficient_Interactives: public cAbstract_Exception {
public:
	cException_Insufficient_Interactives(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_No_Interactives: public cAbstract_Exception {
public:
	cException_No_Interactives(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Invalid_Function : public cAbstract_Exception {
public:
	cException_Invalid_Function( const char * errmsg) : cAbstract_Exception(errmsg) {};
};

class cException_Interactive_Misalignment: public cAbstract_Exception {
public:
	cException_Interactive_Misalignment (const char* errmsg) : cAbstract_Exception(errmsg) {};
};

class cException_File_Not_Found: public cAbstract_Exception {
public:
	cException_File_Not_Found(const char* errmsg): cAbstract_Exception(errmsg) {};
};
class cException_Assignee_Not_In_Tree: public cAbstract_Exception {
public:
	cException_Assignee_Not_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Invalid_Attribute_For_Sort: public cAbstract_Exception{
public:
	cException_Invalid_Attribute_For_Sort(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Invalid_Probability: public cAbstract_Exception {
public:
	cException_Invalid_Probability(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Vector_Data: public cAbstract_Exception {
public:
	cException_Vector_Data(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Attribute_Not_In_Tree: public cAbstract_Exception {
public:
	cException_Attribute_Not_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Duplicate_Attribute_In_Tree: public cAbstract_Exception {
public:
	cException_Duplicate_Attribute_In_Tree(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Unknown_Similarity_Profile: public cAbstract_Exception {
public:
	cException_Unknown_Similarity_Profile(const char* errmsg): cAbstract_Exception(errmsg) {};
};

class cException_Attribute_Disabled : public cAbstract_Exception {
public:
	cException_Attribute_Disabled(const char* errmsg): cAbstract_Exception(errmsg){};
};

class cException_Other: public cAbstract_Exception {
public:
	cException_Other(const char* errmsg): cAbstract_Exception(errmsg){};
};

class cException_Blocking_Disabled : public cAbstract_Exception {
public:
	cException_Blocking_Disabled(const char* errmsg): cAbstract_Exception(errmsg){};
};


class cSimilarity_Compare {
	class cException_Different_Similarity_Dimensions : public cAbstract_Exception {
	public:
		cException_Different_Similarity_Dimensions (const char * errmsg): cAbstract_Exception(errmsg) {};
	};
private:
	static const cException_Different_Similarity_Dimensions default_sp_exception;
public:
	bool operator()(const vector <unsigned int> & s1, const vector < unsigned int> & s2) const {
		if ( s1.size() != s2.size() )
			throw cSimilarity_Compare::default_sp_exception;
		return lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
	};
	bool operator() ( const vector < unsigned int> *ps1, const vector < unsigned int > *ps2 ) const { return cSimilarity_Compare()(*ps1, *ps2);}
};



//=============



//====================================================================
class cAttribute {
private:
	vector <const string *> data;
	//vector <unsigned int> data_count;
	friend void attrib_merge ( list < const cAttribute **> & l1, list < const cAttribute **> & l2 );
protected:
	static const vector <const cAttribute *> empty_interactive;
	vector < const string * > & get_data_modifiable() {return data;}
	virtual const cAttribute * attrib_merge ( const cAttribute & rhs) const { return NULL;};

public:
	virtual unsigned int compare(const cAttribute & rhs) const = 0 ;
	virtual bool split_string(const char* );	//can be overridden if necessary.
	cAttribute (const char * inputstring ) {
		//(NULL != inputstring ) && split_string(inputstring);
		/*
		for ( vector<string>::iterator ps = data.begin(); ps != data.end(); ++ps )
			string(*ps).swap(*ps);
		vector<string> (data).swap(data);
		vector<unsigned int> (data_count).swap(data_count);
		*/
	}
	bool operator == ( const cAttribute & rhs) const { return data == rhs.data;}
	virtual void reset_subclass_data(const void *) {};
	void reset_data(const char * inputstring) {data.clear(); /*data_count.clear(); */ split_string(inputstring);}
	virtual void reset_interactive (const vector <const cAttribute *> &inputvec ) { throw cException_No_Interactives(get_class_name().c_str());};
	const vector <const string*> & get_data() const {return data;}
	virtual const vector <const cAttribute *> & get_interactive_vector() const { throw cException_No_Interactives(get_class_name().c_str()); };
	virtual const string & get_class_name() const = 0;
	virtual bool is_comparator_activated() const = 0;
	// critical for using base pointers to create and copy derived class
	virtual ~cAttribute() {} // Polymorphic destructor to allow deletion via cAttribute*
	virtual const cAttribute* clone() const = 0; // Polymorphic copy constructor
	virtual bool has_checked_interactive_consistency() const = 0;
	virtual void print( std::ostream & ) const = 0;
	void print() const { this->print(std::cout); }
	virtual const string & get_attrib_group() const = 0;
	virtual void check_interactive_consistency(const vector <string> & query_columns) = 0;
	virtual unsigned int get_attrib_max_value() const { throw cException_Invalid_Function(get_class_name().c_str());};
	virtual int exact_compare( const cAttribute & rhs ) const { return -1; } // -1 means no exact_compare. 0 = not the same 1= exact same
	virtual const string * add_string( const string & str ) const = 0;

	virtual bool operator < ( const cAttribute & rhs ) const { return this->data < rhs.data;}
	virtual bool is_informative() const {
		if (  data.empty() || data.at(0)->empty() )
			return false;
		return true;
	}
	virtual int clean_attrib_pool() const = 0;
	virtual const cAttribute * reduce_attrib(unsigned int n) const = 0;
	virtual const cAttribute * add_attrib( unsigned int n ) const = 0  ;
	virtual const set < const string *> * get_attrib_set_pointer () const { return NULL; }


};


template <typename Derived>
class cAttribute_Intermediary : public cAttribute {
	friend bool fetch_records_from_sqlite3(list <cRecord> & source, const char * sql_file, const char * statement,
								const map<string, std::pair<string, unsigned int> >& assigneetree);
	friend bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns, const map<string, std::pair<string, unsigned int> >& asgtree);
private:
	static const string class_name;	// an implementation is required for each class in the cpp file.
	static unsigned int column_index_in_query;
	static string column_name_in_query;
	static const string interactive_column_names[];
	static const unsigned int num_of_interactive_columns;
	static vector <unsigned int> interactive_column_indice_in_query;
	static bool bool_interactive_consistency_checked;
	static bool bool_is_enabled;
	static bool bool_comparator_activated;
	static const string attrib_group;	//attribute group used for ratios purpose;
	static set < string > data_pool;

	static map < Derived, int > attrib_pool;

	static pthread_mutex_t attrib_pool_structure_lock;
	static pthread_mutex_t attrib_pool_count_lock;

protected:

public:
	static void clear_data_pool() {data_pool.clear();}
	static void clear_attrib_pool() {attrib_pool.clear();}

	cAttribute_Intermediary(const char * source = NULL )
		:	cAttribute(source) {
		if (! is_enabled() ) 
			throw cException_Attribute_Disabled(class_name.c_str()); 
		//get_interactive_vector().resize(num_of_interactive_columns);
	}
	static const Derived * static_add_attrib( const Derived & d , const unsigned int n ) {
		pthread_mutex_lock(& attrib_pool_structure_lock);
		register typename map < Derived, int >::iterator p = attrib_pool.find( d );
		if ( p == attrib_pool.end() ) {
			p = attrib_pool.insert(std::pair<Derived, int>(d, 0) ).first;
		}
		pthread_mutex_unlock ( & attrib_pool_structure_lock);

		pthread_mutex_lock(& attrib_pool_count_lock);
		p->second += n;
		pthread_mutex_unlock( & attrib_pool_count_lock);
		return &(p->first);
	}

	static const Derived * static_find_attrib ( const Derived & d) {
		pthread_mutex_lock(& attrib_pool_structure_lock);
		register typename map < Derived, int >::iterator p = attrib_pool.find( d );
		if ( p == attrib_pool.end() ) {
			pthread_mutex_unlock ( & attrib_pool_structure_lock);
			return NULL;
		}
		pthread_mutex_unlock ( & attrib_pool_structure_lock);
		return &(p->first);
	}

	static const Derived * static_reduce_attrib( const Derived & d , const unsigned int n) {
		pthread_mutex_lock ( & attrib_pool_structure_lock);
		register typename map < Derived, int >::iterator p = attrib_pool.find( d );
		if ( p == attrib_pool.end() ) {
			d.print(std::cout);
			//const string * t = * ( d.get_attrib_set_pointer()->begin() );

			throw cException_Other("Error: attrib not exist!");
		}
		pthread_mutex_unlock ( & attrib_pool_structure_lock);

		pthread_mutex_lock ( & attrib_pool_count_lock);
		p->second -= n;
		pthread_mutex_unlock ( & attrib_pool_count_lock);

		if ( p->second <= 0 ) {
			pthread_mutex_lock ( & attrib_pool_structure_lock);
			//std::cout << p->second << " : ";
			//d.print(std::cout);
			attrib_pool.erase(p);
			pthread_mutex_unlock ( & attrib_pool_structure_lock);
			return NULL;
		}
		return &(p->first);
	}

    const cAttribute* clone() const {
    	const Derived & alias = dynamic_cast< const Derived & > (*this);
    	return static_add_attrib(alias, 1);

        //return new Derived(dynamic_cast< const Derived & >(*this) );
    }
    const string & get_class_name() const { return class_name;}
    static const string & static_get_class_name() {return class_name;}
   	static void set_column_index_in_query(const unsigned int i ) {column_index_in_query = i;}
   	static void set_column_name_in_query(const string & inputname ) {column_name_in_query = inputname;}
   	//THIS IS THE DEFAULT COMPARISON FUNCTION. ANY ATTRIBUTE THAT HAS REAL COMPARISION FUNCTIONS SHOULD OVERRIDE IT.
   	//ANY ATTRIBUTE THAT HAS NO REAL COMPARISION FUNCTIONS SHOULD JUST LEAVE IT.
   	unsigned int compare(const cAttribute & rhs) const {
   		throw cException_No_Comparision_Function(class_name.c_str());
   	};
   	static const unsigned int get_interactive_column_number() { return num_of_interactive_columns;};
   	static void static_check_interactive_consistency( const vector <string> & query_columns ) {
   		if ( interactive_column_indice_in_query.size() != get_interactive_column_number() )
   			throw cException_Insufficient_Interactives(class_name.c_str());
   		for ( unsigned int i = 0; i < num_of_interactive_columns; ++i) {
   			unsigned int temp_index = find(query_columns.begin(), query_columns.end(), interactive_column_names[i]) - query_columns.begin();
   			if ( temp_index < query_columns.size() )
   				interactive_column_indice_in_query[i] = temp_index;
   			else
   				throw cException_Insufficient_Interactives(class_name.c_str());
   		}
   		bool_interactive_consistency_checked = true;
   	}
	void check_interactive_consistency(const vector <string> & query_columns) { static_check_interactive_consistency(query_columns);}
   	bool has_checked_interactive_consistency() const {return bool_interactive_consistency_checked;}
	static bool is_enabled() { return bool_is_enabled; }
	static void enable() { bool_is_enabled = true;}
	static bool static_is_comparator_activated() {return bool_comparator_activated;}
	bool is_comparator_activated() const { return bool_comparator_activated;}
	static void activate_comparator() {bool_comparator_activated = true; std::cout << static_get_class_name() << " comparison is active now." << std::endl;}
	static void deactivate_comparator() {bool_comparator_activated = false;std::cout << static_get_class_name() << " comparison is deactivated." << std::endl;}
	void print( std::ostream & os ) const {
		//static const char lineend = '\n';
		vector < const string * >::const_iterator p = this->get_data().begin();
		os << class_name << ": ";
		if ( p == this->get_data().end() ) {
			os << "Empty attribute." << std::endl;
			return;
		}
		os << "raw data = " << **p << ", Derivatives = ";
		for ( ++p; p != this->get_data().end(); ++p )
			os << **p << " | ";
		os << std::endl;
	}
	static const string & static_get_attrib_group() { return attrib_group; };
	const string & get_attrib_group() const { return attrib_group;}

	static const string * static_add_string ( const string & str ) {
		register set< string >::iterator p = data_pool.find(str);
		if ( p == data_pool.end() ) {
			p = data_pool.insert(str).first;
			//const string & alias = *p;
			//string & s =  const_cast< string & >(alias);
			//string(s).swap(s);
		}
		return &(*p);
	}
	static const string * static_find_string ( const string & str ) {
		register set< string >::iterator p = data_pool.find(str);
		if ( p == data_pool.end() ) {
			return NULL;
		}
		else
			return &(*p);
	}

	const string * add_string ( const string & str ) const  {
		return static_add_string ( str );
	}

	static const cAttribute * static_clone_by_data( const vector < string > & str ) {
		Derived d;
		vector < const string *> & alias = d.get_data_modifiable();
		alias.clear();
		for ( vector < string > ::const_iterator p = str.begin(); p !=str.end(); ++p ) {
			const string * q = static_add_string(*p);
			alias.push_back(q);
		}
		return static_add_attrib(d, 1);
	}

	static int static_clean_attrib_pool() {
		int cnt = 0;
		for ( typename map < Derived, int> :: iterator p = attrib_pool.begin(); p != attrib_pool.end() ; ) {
			if ( p-> second == 0 ) {
				attrib_pool.erase(p++);
				++p;
				//++cnt;
			}
			else if ( p->second < 0) {
				throw cException_Other("Error in cleaning attrib pool.");
			}
			else
				++p;
		}
		return cnt;
	}

	int clean_attrib_pool() const { return static_clean_attrib_pool(); }

	const cAttribute * reduce_attrib(unsigned int n) const {
		return static_reduce_attrib( dynamic_cast< const Derived &> (*this), n);
	}
	const cAttribute * add_attrib( unsigned int n ) const {
		return static_add_attrib( dynamic_cast< const Derived &> (*this), n);
	}
};


template < typename AttribType >
class cAttribute_Set_Mode : public cAttribute_Intermediary < AttribType > {
protected:
	set < const string * > attrib_set;
	const cAttribute * attrib_merge ( const cAttribute & right_hand_side) const {
		const AttribType & rhs = dynamic_cast< const AttribType & > (right_hand_side);
		set < const string * > temp (this->attrib_set);
		temp.insert(rhs.attrib_set.begin(), rhs.attrib_set.end());
		AttribType tempclass;
		tempclass.attrib_set = temp;
		const AttribType * result = this->static_add_attrib(tempclass, 2);
		static_reduce_attrib( dynamic_cast<const AttribType & >(*this), 1);
		static_reduce_attrib(rhs, 1);
		return result;
	}

public:
	//const vector < const string *> & get_data() const {throw cException_Other ("No vector data. Invalid operation."); return cAttribute::get_data();}
	const set < const string *> * get_attrib_set_pointer() const { return & attrib_set;}
	unsigned int compare(const cAttribute & right_hand_side) const {
		if ( ! this->is_comparator_activated () )
			throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
		try {
			if ( this == & right_hand_side )
				return this->get_attrib_max_value();

			unsigned int res = 0;
			const AttribType & rhs = dynamic_cast< const AttribType & > (right_hand_side);

			const unsigned int mv = this->get_attrib_max_value();
			//this->print(std::cout);
			//rhs.print(std::cout);
			res = num_common_elements (this->attrib_set.begin(), this->attrib_set.end(),
										 rhs.attrib_set.begin(), rhs.attrib_set.end(),
										 mv);

			if ( res > mv )
				res = mv;
			return res;
		}
		catch ( const std::bad_cast & except ) {
			std::cerr << except.what() << std::endl;
			std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
			throw;
		}
	}
	bool split_string(const char* inputdata) {
		try {
			cAttribute::split_string(inputdata);
		}
		catch ( const cException_Vector_Data & except) {
			//std::cout << "cClass allows vector data. This info should be disabled in the real run." << std::endl;
		}
		//const string raw(inputdata);
		this->attrib_set.clear();
		for ( vector < const string *>::const_iterator p = this->get_data().begin(); p != this->get_data().end(); ++p ) {
			if ( (*p)->empty() )
				continue;
			this->attrib_set.insert(*p);
		}
		this->get_data_modifiable().clear();
		//this->get_data_modifiable().insert(this->get_data_modifiable().begin(), this->add_string(raw));
		return true;
	}
	bool operator < ( const cAttribute & rhs ) const { return this->attrib_set < dynamic_cast< const AttribType & >(rhs).attrib_set;}
	void print( std::ostream & os ) const {
		set < const string * >::const_iterator p = attrib_set.begin();
		os << this->get_class_name() << ": ";
		if ( p == attrib_set.end() ) {
			os << "Empty attribute." << std::endl;
			return;
		}
		os << "No raw data. " << ", Derivatives = ";

		const string * qq;
		for ( ; p != attrib_set.end(); ++p ) {
			os << **p ;
			qq = this->static_find_string(**p);
			if ( qq == NULL )
				os << ", data UNAVAILABLE ";
			if ( qq != *p )
				os << ", address UNAVAILABLE ";

			os << " | ";

		}
		os << std::endl;
	}
	bool is_informative() const { return false;}
};


template < typename AttribType >
class cAttribute_Single_Mode : public cAttribute_Intermediary<AttribType> {
public:
	unsigned int compare(const cAttribute & right_hand_side) const {
		if ( ! this->is_comparator_activated () )
			throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
		if ( this == & right_hand_side )
			return this->get_attrib_max_value();

		unsigned int res = 0;
		const AttribType & rhs = dynamic_cast< const AttribType & > (right_hand_side);
		res = jwcmp(* this->get_data().at(1), * rhs.get_data().at(1));
		if ( res > this->get_attrib_max_value() )
			res = this->get_attrib_max_value();
		return res;
	}
	bool split_string(const char* inputdata){
		this->get_data_modifiable().clear();
		string temp(inputdata);
		const string * p = this->add_string(temp);
		this->get_data_modifiable().push_back(p);
		this->get_data_modifiable().push_back(p);
		return true;
	}


};


template < typename AttribType >
class cAttribute_Vector_Mode : public cAttribute_Intermediary<AttribType> {
public:
	unsigned int compare(const cAttribute & right_hand_side) const {
		if ( ! this->is_comparator_activated () )
			throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
		if ( this == & right_hand_side )
			return this->get_attrib_max_value();

		unsigned int res = 0;
		const AttribType & rhs = dynamic_cast< const AttribType & > (right_hand_side);

		for ( vector < const string *>::const_iterator p = this->get_data().begin(); p != this->get_data().end(); ++p ) {
			for ( vector < const string *>::const_iterator q = rhs.get_data().begin(); q != rhs.get_data().end(); ++q ) {
				res += ( (*p == *q )? 1:0 );
			}
		}
		if ( res > this->get_attrib_max_value() )
			res = this->get_attrib_max_value();
		return res;
	}
	bool split_string(const char* inputdata){
		try {
			cAttribute::split_string(inputdata);
		}
		catch ( const cException_Vector_Data & except) {
			//std::cout << "cClass allows vector data. This info should be disabled in the real run." << std::endl;
		}
		return true;
	}


};
//


// Attributes: add all possible attributes here. All detailed attributes inherit from the template cAttribute_Intermediary<>
// THE ONLY NECESSARY COMPONENTS HERE are 1. the CONSTRUCTER. 2. the overriding comparision function.
// Other useful member data and member functions can be added to the class if necessary.

/*
class cFirstname : public cAttribute_Intermediary<cFirstname> {
private:
	static const unsigned int max_value = 4;
public:
	cFirstname(const char * source = NULL)
		: cAttribute_Intermediary<cFirstname>(source){}
	unsigned int compare(const cAttribute & rhs) const;
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this->get_data().at(0) == rhs.get_data().at(0); }
};
*/

class cFirstname : public cAttribute_Single_Mode <cFirstname> {
private:
	static const unsigned int max_value = 4;
public:
	cFirstname(const char * source = NULL) {}
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};


class cLastname : public cAttribute_Single_Mode <cLastname> {
private:
	static const unsigned int max_value = 4;
public:
	cLastname(const char * source = NULL ) {}
	//unsigned int compare(const cAttribute & rhs) const ;
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//int exact_compare( const cAttribute & rhs ) const { return this->get_data().at(0) == rhs.get_data().at(0); }
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cMiddlename : public cAttribute_Single_Mode <cMiddlename> {
private:
	static const unsigned int max_value = 3;
public:
	cMiddlename(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//int exact_compare( const cAttribute & rhs ) const { return this->get_data().at(0) == rhs.get_data().at(0); }
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cLatitude : public cAttribute_Single_Mode <cLatitude> {
private:
	static const unsigned int max_value = 2;
	vector <const cAttribute *> vec_pinteractive;
public:
	cLatitude(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;
	const vector <const cAttribute *> & get_interactive_vector() const {return vec_pinteractive;};
	void reset_interactive (const vector <const cAttribute *> &inputvec ) { vec_pinteractive = inputvec;};
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cLongitude: public cAttribute_Single_Mode <cLongitude> {
private:
public:
	cLongitude(const char * source = NULL ) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};

class cStreet: public cAttribute_Single_Mode <cStreet> {
private:
public:
	cStreet(const char * source = NULL) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE Street IS NOT BEING COMPARED either. IT IS WITH THE LATITUDE COMPARISION.

};

class cCountry: public cAttribute_Single_Mode <cCountry> {
private:
public:
	cCountry(const char * source = NULL ) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE country IS NOT BEING COMPARED either. IT IS WITH THE LATITUDE COMPARISION.
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

#if 0
class cClass : public cAttribute_Intermediary<cClass> {
private:
	static unsigned int const max_value = 4;
	//static const set < const string * > empty_set_classes;
	set < const string * > set_class;
	const cClass * attrib_merge ( const cAttribute & rhs) const;
public:
	cClass(const char * source = NULL )
		: cAttribute_Intermediary<cClass>(source){}
	unsigned int compare(const cAttribute & rhs) const;
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	bool operator < ( const cAttribute & rhs ) const { return this->set_class < dynamic_cast< const cClass & >(rhs).set_class;}
	void print( std::ostream & os ) const {
		set < const string * >::const_iterator p = set_class.begin();
		os << this->get_class_name() << ": ";
		if ( p == set_class.end() ) {
			os << "Empty attribute." << std::endl;
			return;
		}
		os << "No raw data. " << ", Derivatives = ";
		for ( ; p != set_class.end(); ++p )
			os << **p << " | ";
		os << std::endl;
	}
};
#endif

class cClass: public cAttribute_Set_Mode < cClass > {
private:
	static unsigned int const max_value = 4;
public:
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//unsigned int compare(const cAttribute & rhs) const;
};


#if 0
class cCoauthor : public cAttribute_Intermediary<cCoauthor> {
private:
	static list < cCoauthor > attrib_pool;
	static const set < const string * > empty_set_coauthors;
	const set < const string * > * pset;
public:
	static const unsigned int max_value = 6;
	cCoauthor(const char * source = NULL )
		: cAttribute_Intermediary<cCoauthor>(source), pset(&empty_set_coauthors){}
	unsigned int compare(const cAttribute & rhs) const;
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	cAttribute* clone() const {
		attrib_pool.push_back(*this);
		return & attrib_pool.back();
	}

	void reset_pointer(const set < const string *> & s) {	pset = &s ;}
	const set < const string *> * get_set_pointer() const {return pset;}
	bool operator == ( const cCoauthor & rhs) const { return *pset == * rhs.pset;}
	void print( std::ostream & os ) const {
		set < const string * >::const_iterator p = pset->begin();
		os << this->get_class_name() << ": ";
		if ( p == pset->end() ) {
			os << "Empty attribute." << std::endl;
			return;
		}
		os << "No raw data. " << ", Derivatives = ";
		for ( ; p != pset->end(); ++p )
			os << **p << " | ";
		os << std::endl;
	}
	bool operator < ( const cAttribute & rhs ) const {
		throw cException_Other(" operator less than of class cCoauthor should be invalid.");
		const cCoauthor & r = dynamic_cast< const cCoauthor & > (rhs);
		return this->pset < r.pset;
	}
	bool is_informative () const  {
		if ( pset->empty())
			return false;
		return true;
	}
};

#endif


class cCoauthor : public cAttribute_Set_Mode < cCoauthor >  {
	friend class cReconfigurator_Coauthor;
private:
	static unsigned int const max_value = 6;
public:
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//unsigned int compare(const cAttribute & rhs) const;
};

class cAssignee : public cAttribute_Single_Mode <cAssignee> {
private:
	static const unsigned int max_value = 5;
	static const map<string, std::pair<string, unsigned int>  > * assignee_tree_pointer;
public:
	cAssignee(const char * source = NULL ) {}
	//bool split_string(const char*);
	unsigned int compare(const cAttribute & rhs) const;
	static void set_assignee_tree_pointer(const map<string, std::pair<string, unsigned int>  >& asgtree) {assignee_tree_pointer = & asgtree;}
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//int exact_compare( const cAttribute & rhs ) const { return this->get_data().at(0) == rhs.get_data().at(0); }
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cAsgNum : public cAttribute_Single_Mode<cAsgNum> {
private:
public:
	cAsgNum(const char * source = NULL ){}
};

class cUnique_Record_ID : public cAttribute_Single_Mode <cUnique_Record_ID> {
private:
public:
	cUnique_Record_ID(const char * source = NULL ){}
};

class cApplyYear: public cAttribute_Single_Mode<cApplyYear> {
private:
public:
	cApplyYear(const char * source = NULL ){}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};

class cCity: public cAttribute_Single_Mode <cCity> {
private:
public:
	cCity(const char * source = NULL ) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
	bool split_string(const char*);
	//int exact_compare( const cAttribute & rhs ) const { return this->get_data().at(0) == rhs.get_data().at(0); }
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cPatent: public cAttribute_Single_Mode <cPatent> {
private:
public:
	cPatent( const char * source = NULL){};
};

//=======

// template static variables.
// Declaration and default definition.
// Specialization should be implemented in the cpp file.
template <typename Derived> unsigned int cAttribute_Intermediary<Derived>::column_index_in_query;
//template <typename Derived> string cAttribute_Intermediary<Derived>::column_name_in_query;
template <typename Derived> vector <unsigned int> cAttribute_Intermediary<Derived>::interactive_column_indice_in_query;
template <typename Derived> bool cAttribute_Intermediary<Derived>::bool_interactive_consistency_checked = false;
template <typename Derived> bool cAttribute_Intermediary<Derived>::bool_is_enabled = false;
template <typename Derived> bool cAttribute_Intermediary<Derived>::bool_comparator_activated = false;
template <typename Derived> set < string > cAttribute_Intermediary<Derived>:: data_pool;
template <typename Derived> map < Derived, int > cAttribute_Intermediary<Derived>:: attrib_pool;
template <typename Derived> pthread_mutex_t cAttribute_Intermediary<Derived>:: attrib_pool_structure_lock = PTHREAD_MUTEX_INITIALIZER;
template <typename Derived> pthread_mutex_t cAttribute_Intermediary<Derived>:: attrib_pool_count_lock = PTHREAD_MUTEX_INITIALIZER;

//declaration ( not definition ) of specialized template
template <> const string cAttribute_Intermediary<cFirstname>::attrib_group;
template <> const string cAttribute_Intermediary<cLastname>::attrib_group;
template <> const string cAttribute_Intermediary<cMiddlename>::attrib_group;
template <> const string cAttribute_Intermediary<cLatitude>::attrib_group;
template <> const string cAttribute_Intermediary<cAssignee>::attrib_group;
template <> const string cAttribute_Intermediary<cClass>::attrib_group;
template <> const string cAttribute_Intermediary<cCoauthor>::attrib_group;
template <typename Derived> const string cAttribute_Intermediary<Derived>::attrib_group = "None";

template <> const string cAttribute_Intermediary<cLatitude>::interactive_column_names[];
template <> const string cAttribute_Intermediary<cAssignee>::interactive_column_names[];
template <typename Derived> const string cAttribute_Intermediary<Derived>::interactive_column_names[] = {};


template <> const unsigned int cAttribute_Intermediary<cLatitude>::num_of_interactive_columns;
template <> const unsigned int cAttribute_Intermediary<cAssignee>::num_of_interactive_columns;
template <typename Derived> const unsigned int cAttribute_Intermediary<Derived>::num_of_interactive_columns = 0;

#endif /* DISAMBIGLIB_HPP_ */

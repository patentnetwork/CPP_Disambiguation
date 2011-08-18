/*
 * DisambigDefs.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Ye (Edward) Sun.  Harvard University. sunyedinken2002@hotmail.com
 *
 *
 *
 *  This file contains most of the basic definitions.
 *
 *
 */







#ifndef DISAMBIGLIB_HPP_
#define DISAMBIGLIB_HPP_

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <pthread.h>
#include <typeinfo>
#include <memory>

#define INERT_ATTRIB_GROUP_IDENTIFIER "NONE"			// the attribute group specifier that is not the component of similarity profiles


using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

class cRecord;
class cRecord_Reconfigurator;
void cRecord_update_active_similarity_names();
class cAttribute;
class cRecord_Reconfigurator;
const cRecord_Reconfigurator * generate_interactive_reconfigurator( const cAttribute * pAttrib);
void reconfigure_interactives ( const cRecord_Reconfigurator * pc, const cRecord * pRec);



class cAttribute;
//======================================================================================

/*
 *  cAbstract_Exception
 *  	-- cException_Diff_Attribute
 *  	-- cException_No_Comparision_Function
 *  	-- cException_ColumnName_Not_Found
 *  	-- cException_Insufficient_Interactives
 *  	-- cException_No_Interactives
 *  	-- cException_Invalid_Function
 *  	-- cException_Interactive_Misalignment
 *  	-- cException_File_Not_Found
 *  	-- cException_Assignee_Not_In_Tree
 *  	-- cException_Invalid_Attribute_For_Sort
 *  	-- cException_Invalid_Probability
 *  	-- cException_Vector_Data
 *  	-- cException_Attribute_Not_In_Tree
 *  	-- cException_Duplicate_Attribute_In_Tree
 *  	-- cException_Unknown_Similarity_Profile
 *  	-- cException_Attribute_Disabled
 *  	-- cException_Other
 *  	-- cException_Blocking_Disabled
 *
 *  To create a new exception class, simply inherit the cAbstract_Exception class, and write a constructor.
 *
 */


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


//=============================================

/*
 * cSimilarity_Compare:
 * This is the functor to sort similarity profiles. Used only in std::map and std::set of similarity profiles.
 * Overloading the operator () for comparison between similarity profiles by their references or pointers.
 */

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

/*
 * 0 .cAttribute
 * 	1. template <> cAttribute_Basic
 * 		2. template <> cAttribute_Intermediary
 * 			3. template <> cAttribute_Set_Intermediary
 * 				4. template <> cAttribute_Set_Mode
 * 					5. cClass
 * 					5. cCoauthor
 * 		2. template <> cAttribute_Vector_Intermediary
 * 			3. template <> cAttribute_Vector_Mode
 * 			3. template <> cAttribute_Single_Mode
 * 				4. cFirstname
 * 				4. cMiddlename
 * 				4. cLastname
 * 				4. cLongitude
 * 				4. cCity
 * 				4. cCountry
 * 				4. cAsgNum
 * 				4. cUnique_Record_ID
 * 				4. cPatent
 * 				4. cApplyYear
 *				4. cStreet
 *				4. cLatitude_Data
 *				4. cLongitude_Data
 *				4. cAssignee_Data
 *		2. template<> cAttribute_Interactive_Mode
 *			3. cLatitude
 *			3. cLongitude
 *			3. cAssignee
 *
 *	This is the base abstract class of all concrete attributes. Interfaces are designed, but the implementations are generally left for child classes.
 *	Usually the pointer of cAttribute class is used, in order to achieve customized behaviors for each concrete classes ( Polymorphism ).
 *	The hierarchy of inheritance is shown above. Any newly introduced concrete class should choose one mode to inherit.
 *  Most concrete attributes fall into the template classes, so it is convenient to simply choose one of the mode and inherit it when a
 *  concrete class is introduced.
 *	All the concrete classes are declared in the file "DisambigCustomizedDefs.h" and implemented in "DisambigCustomizedDefs.cpp".
 *
 *
 *	Member Functions:
 *	Protected:
 *		1. virtual vector < const string * > & get_data_modifiable() = 0: get the data. should only be used within derived classes. Implemented in child classes.
 *		2. virtual const cAttribute * attrib_merge ( const cAttribute & rhs) const: get the attribute pointer into which the two attributes are supposed to merge
 *	Public:
 *		3. virtual unsigned int compare(const cAttribute & rhs) const = 0 ; Comparison function between two attributes to get a similarity score. Should be inplemented by child classes.
 *		4. virtual bool split_string(const char* ); This function reads external data and stores into the "data". Each template has a default implementation, but should be overidden if necessary.
 *		5. virtual bool operator == ( const cAttribute & rhs) const: exact comparison between two attributes. Has a default implementation but overidable.
 *		6. void reset_data(const char * inputstring): reset the attribute based on the inputstring. calls the polymorphic "split_string" function.
 *		7. virtual void config_interactive (const vector <const cAttribute *> &inputvec ): handles the attribute with other linked ones. Returns the pointer of a configured attribute. Default implementation is throwing an error. Overide if necessary.
 *		8. virtual const vector <const string*> & get_data() const = 0: The most commonly used function. Get the vector of string pointers. Implemented in child classes.
 *		9. virtual const vector <const cAttribute *> & get_interactive_vector(): expected to be implemented in the child class to accommodate access to data that has interactions with the attribute.
 *		10. virtual const string & get_class_name() const : To get the identifier of the class. must be implemented by template child class.
 *		11. virtual bool is_comparator_activated(): To check if the comparison of the attribute is enabled. also must be implemented by template child class.
 *		12. virtual ~cAttribute(): polymophic destructor. no need to change.
 *		13. virtual const cAttribute* clone() const: polymorphic copy constructor. This should be the way to create a copy of the attribute.
 *		14. virtual bool has_checked_interactive_consistency() const: To check if the pointers of interactive attributes are stored correctly. Must be called when loading a source data file.
 *		15. virtual unsigned int get_attrib_max_value() const: To get the maximum attribute score. The scores are determined in child classes, so this function has to be overridden in child class.
 *		16. virtual int exact_compare( const cAttribute & rhs ): To check if this attribute is exactly the same as rhs. -1 means disabled. 0 = not exact match. 1 = exact match.
 *		17. virtual const string * add_string( const string & str ) const: To add the string data into the internal static string data pooling system, and return the pointer to the pooled string. Extremely important.
 *		18. virtual bool operator < ( const cAttribute & rhs ) const: defined the less operator for the class. Used in the internal attribute pooling system. Not expected for user use.
 *				HOWEVER, IT IS EXTREMLY IMPORTANT TO OVERRIDE THE LESS THAN OPERATOR IF DIFFERENT OBJECTS ARE COMPARED NOT ONLY BY cAttribute::data. i.e., interactive data, or set_mode.
 *		19. virtual bool is_informative() const: to check if the attribute data vector is empty, or the string pointer in the vector points to an empty string. Override if necessary.
 *		20. virtual int clean_attrib_pool() const: to clean the attribute pool in the template child class. Must be overridden in child classes.
 *		21. virtual const cAttribute * reduce_attrib(unsigned int n) const: deduct the counting reference of "this" attribute by n, and return the pointer to the attribute. Pooling is in the subclass. So has to be overridden.
 *		22. virtual const cAttribute * add_attrib( unsigned int n ) const:  add the counting reference of "this" attribute by n, and return the pointer to the attribute. Pooling is in the subclass. So has to be overridden.
 *		23. virtual const set < const string *> * get_attrib_set_pointer () const: to get the Set_Mode data, instead of the default vector mode data. An interface for Set_Mode classes.
 *
 *  ATTENTION:
 *  	NEVER EDIT AN EXISTING ATTRIBUTE OBJECT, BECAUSING IT MAY BE USED BY DIFFERENT RECORDS!
 *  	ONLY ADD NEW OBJECTS AND CHANGE THE POINTER THROUGH EXISTING INTERFACE, SINCE REFERENCE-COUNTING IS IMPLEMENTED.
 */

//====================================================================
class cAttribute {
private:
	friend class cRecord;
	friend void attrib_merge ( list < const cAttribute **> & l1, list < const cAttribute **> & l2 );
	static vector <string> Derived_Class_Name_Registry;
	virtual void reconfigure_for_interactives( const cRecord * pRec) const {};
protected:
	virtual vector < const string * > & get_data_modifiable() = 0;
	virtual const cAttribute * attrib_merge ( const cAttribute & rhs) const { return NULL;};
public:
	virtual unsigned int compare(const cAttribute & rhs) const = 0 ;
	virtual bool split_string(const char* );	//can be overridden if necessary.
	cAttribute (const char * inputstring ) {}
	virtual bool operator == ( const cAttribute & rhs) const { return this == &rhs ;}
	void reset_data(const char * inputstring) { get_data_modifiable().clear(); /*data_count.clear(); */ split_string(inputstring);}
	virtual const cAttribute*  config_interactive (const vector <const cAttribute *> &inputvec ) const { throw cException_No_Interactives(get_class_name().c_str()); return NULL;};
	virtual const vector <const string*> & get_data() const = 0;
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

	virtual bool operator < ( const cAttribute & rhs ) const = 0;
	virtual bool is_informative() const {
		if (  get_data().empty() || get_data().at(0)->empty() )
			return false;
		return true;
	}

	virtual int clean_attrib_pool() const = 0;
	virtual const cAttribute * reduce_attrib(unsigned int n) const = 0;
	virtual const cAttribute * add_attrib( unsigned int n ) const = 0  ;
	virtual const set < const string *> * get_attrib_set_pointer () const { return NULL; }
	static void register_class_names( const vector < string > &);
	static int position_in_registry( const string & );
	virtual vector < string > get_interactive_class_names() const = 0;
	virtual void activate_comparator() const = 0;
	virtual void deactivate_comparator() const = 0;
	virtual const cAttribute *  get_effective_pointer() const = 0;	//specifically useful in the interactive mode
};



/*
 * template <Concreate Class Name> cAttribute_Intermediary and cAttribute_Basic.
 * This is the first/second layer child class of cAttribute, implementing data pooling and other fundamental concrete class specific methods.
 * Data pooling is implemented using binary trees in STL, i.e., std::set and std::map, in order for fast search, insertion and deletion.
 * If any other second layer abstract or concrete attribute class is added in the future, it is supposed to inherit from this cAttribute_Intermediary class.
 * See examples below.
 *
 *
 *
 *
 * Private:
 * 		static const string class_name: the name specifier of each concrete attribute class.
 * 		static unsigned int column_index_in_query: the location of each attribute in accordance with the reading sequence.
 * 		static const string interactive_column_names[]: column names that have interaction with the current attribute type. Pay attention to the sequence!!!!
 * 		static const unsigned int num_of_interactive_columns; number of columns that have interaction with the current attribute type.
 * 		static vector <unsigned int> interactive_column_indice_in_query: indice of the attributes that have interaction with the current attributes, as displayed while being read.
 * 		static bool bool_interactive_consistency_checked: whether the interactive attributes are read the same sequence as assigned in the interactive_column_names variable.
 * 		static bool bool_is_enabled: whether the attribute is enabled for reading. Maybe not useful now.
 * 		static bool bool_comparator_activated: whether the attribute type will be compared.
 * 		static const string attrib_group: the attribute group of the concrete attribute class.
 * 		static set < string > data_pool: pooling system for the data that are used in THIS certain entire concrete attribute CLASS ONLY.
 * 		static map < Derived, int > attrib_pool: pooling system for the attribute objects that are used in the entire attribute class only, with reference-counting.
 * 		static pthread_rwlock_t attrib_pool_structure_lock: read-write lock of the attrib_pool that synchronize deletion and insertion of new attribute objects.
 * 		static pthread_mutex_t attrib_pool_count_lock: mutex of attrib_pool that synchronize modification of reference-counting.
 *
 *
 *
 *	Protected:
 *	Public:
 *		static void clear_data_pool(): clear the data pool. Only use it when a whole class will be rebuilt.
 *		static void clear_attrib_pool(): clear the attribute object pool. Only use it when a whole class will be rebuilt.
 *		cAttribute_Intermediary(const char * source = NULL ): default constructor.
 *		static const Derived * static_add_attrib( const Derived & d , const unsigned int n ): add the reference counter of the d attribute object by n. If d does not exists, add d to the pool. Returns the pointer to the newly added object.
 *		static const Derived * static_find_attrib ( const Derived & d): find the d object in the attribute pool. Returns 0 if failure, or the pointer to the object if success.
 *		static const Derived * static_reduce_attrib( const Derived & d , const unsigned int n): deduct the reference counter of the d attribute object by n. If the counter = 0, delete d from the pool and returns NULL, else returns the pointer to d.
 *		const cAttribute* clone() const: Polymorphic constructor of the concrete class. Should only be called when reading data from source files.
 *		const string & get_class_name() const: to get the concrete class specifier.
 *		static const string & static_get_class_name(): static function, to get the concrete class specifier.
 *		static void set_column_index_in_query(const unsigned int i ): to set the sequence of the concrete class to i, as appearing in the class list.
 *		unsigned int compare(const cAttribute & rhs) const: the default comparison function between two concrete class objects. Override if necessary.
 *		static const unsigned int get_interactive_column_number(): get the number of interactive columns with the current class.
 *		static void static_check_interactive_consistency( const vector <string> & query_columns): check and set the static member interactive_column_indice_in_query;
 *		void check_interactive_consistency(const vector <string> & query_columns): polymorphic version of the above function.
 *		bool has_checked_interactive_consistency(): check if the class interactive consistency is checked.
 *		static bool is_enabled(): check if the class is enabled for use.
 *		static void enable(): enable the class.
 *		static bool static_is_comparator_activated(): check if the comparison function for the class is activated.
 *		bool is_comparator_activated(): polymorphic version of the above function.
 *		static void activate_comparator(): activate the comparison function for the class.
 *		static void deactivate_comparator(): de-activate the comparison function for the class.
 *		void print( std::ostream & os ) const: print the current object to output stream os, usually std::cout.
 *		static const string & static_get_attrib_group(): get the specifier of the attribute group of the class.
 *		const string & get_attrib_group() const: polymorphic version of the above function.
 *		static const string * static_add_string ( const string & str ): copy the string "str" to the data pool, and returns the pointer to the newly added string.
 *		static const string * static_find_string( const string & str) : find the string "str" in the data pool, and returns the pointer to it if success or NULL if failure.
 *		const string * add_string ( const string & str ): polymorphic version of the above static version.
 *		static const cAttribute * static_clone_by_data( const vector < string > & str ): add or create an attribute object by the str data, returns the pointer to the object.
 *		static int static_clean_attrib_pool(): remove the objects whose reference counter = 0, returns the number of removed objects.
 *		int clean_attrib_pool() const: polymorphic version of the above function
 *		const cAttribute * reduce_attrib(unsigned int n) const: deduct the reference counter of this object by n, and returns the pointer to this object. Null if removed.
 *		const cAttribute * add_attrib(unsigned int n) const: add the reference counter of this object by n, and returns the pointer to this object.
 */


template <typename Derived>
class cAttribute_Basic: public cAttribute {
private:
	static const string class_name;	// an implementation is required for each class in the cpp file.
	//static unsigned int column_index_in_query;
	static const string interactive_column_names[];
	static const unsigned int num_of_interactive_columns;
	static vector <unsigned int> interactive_column_indice_in_query;
	static bool bool_interactive_consistency_checked;
	static bool bool_is_enabled;
	static bool bool_comparator_activated;
	static const string attrib_group;	//attribute group used for ratios purpose;
public:

	cAttribute_Basic (const char * source = NULL ): cAttribute(source) {}
    const string & get_class_name() const { return class_name;}
    static const string & static_get_class_name() {return class_name;}
   	//static void set_column_index_in_query(const unsigned int i ) {column_index_in_query = i;}
   	//THIS IS THE DEFAULT COMPARISON FUNCTION. ANY ATTRIBUTE THAT HAS REAL COMPARISION FUNCTIONS SHOULD OVERRIDE IT.
   	//ANY ATTRIBUTE THAT HAS NO REAL COMPARISION FUNCTIONS SHOULD JUST LEAVE IT.
   	unsigned int compare(const cAttribute & rhs) const {
   		throw cException_No_Comparision_Function(class_name.c_str());
   	};
   	static const unsigned int get_interactive_column_number() { return num_of_interactive_columns;};
   	static void static_check_interactive_consistency( const vector <string> & query_columns ) {
#if 0
   		if ( interactive_column_indice_in_query.size() != get_interactive_column_number() )
   			throw cException_Insufficient_Interactives(class_name.c_str());
   		for ( unsigned int i = 0; i < num_of_interactive_columns; ++i) {
   			unsigned int temp_index = find(query_columns.begin(), query_columns.end(), interactive_column_names[i]) - query_columns.begin();
   			if ( temp_index < query_columns.size() )
   				interactive_column_indice_in_query[i] = temp_index;
   			else
   				throw cException_Insufficient_Interactives(class_name.c_str());
   		}
#endif
   		bool_interactive_consistency_checked = true;
   	}
	void check_interactive_consistency(const vector <string> & query_columns) { static_check_interactive_consistency(query_columns);}
   	bool has_checked_interactive_consistency() const {return bool_interactive_consistency_checked;}
	static bool is_enabled() { return bool_is_enabled; }
	//static void enable() { bool_is_enabled = true;}
	static bool static_is_comparator_activated() {return bool_comparator_activated;}
	bool is_comparator_activated() const { return bool_comparator_activated;}
	static void static_activate_comparator() {
		if ( attrib_group == INERT_ATTRIB_GROUP_IDENTIFIER )
			return;
			//throw cException_No_Comparision_Function ( ( string("Attribute GROUP is not set properly. Attribute = ") + class_name + string(" Group = ") + attrib_group ).c_str());
		bool_comparator_activated = true;
		std::cout << static_get_class_name() << " comparison is active now." << std::endl;
		cRecord_update_active_similarity_names() ;
	}
	static void static_deactivate_comparator() {
		if ( attrib_group == INERT_ATTRIB_GROUP_IDENTIFIER )
			return;
			//throw cException_No_Comparision_Function ( ( string("Attribute GROUP is not set properly. Attribute = ") + class_name + string(" Group = ") + attrib_group ).c_str());
		bool_comparator_activated = false;
		std::cout << static_get_class_name() << " comparison is deactivated." << std::endl;
		cRecord_update_active_similarity_names() ;
	}

	void activate_comparator() const { this->static_activate_comparator(); }
	void deactivate_comparator() const { this->static_deactivate_comparator();}
	void print( std::ostream & os ) const {
		vector < const string * >::const_iterator p = this->get_data().begin();
		os << class_name << ": ";
		os << "Data Type = " << typeid(*this).name() << ", ";
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
	static vector < string > static_get_interactive_column_names() {
		vector < string > res ( interactive_column_names, interactive_column_names + num_of_interactive_columns);
		return res;
	}
	vector < string > get_interactive_class_names() const {
		return static_get_interactive_column_names();
	}

};


template <typename Derived>
class cAttribute_Intermediary : public cAttribute_Basic < Derived > {
	friend bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns);
private:
	static set < string > data_pool;
	static map < Derived, int > attrib_pool;
	static pthread_rwlock_t attrib_pool_structure_lock;
	static pthread_mutex_t attrib_pool_count_lock;

protected:

public:
	const cAttribute *  get_effective_pointer() const { return this; };

	static void clear_data_pool() {data_pool.clear();}
	static void clear_attrib_pool() {attrib_pool.clear();}


	cAttribute_Intermediary(const char * source = NULL )
		:	cAttribute_Basic<Derived> (source){}
	static const Derived * static_add_attrib( const Derived & d , const unsigned int n ) {
		pthread_rwlock_rdlock(& attrib_pool_structure_lock);
		typename map < Derived, int >::iterator p = attrib_pool.find( d );
		const bool is_end = ( p == attrib_pool.end() );
		pthread_rwlock_unlock(& attrib_pool_structure_lock);

		if ( is_end ) {
			pthread_rwlock_wrlock(& attrib_pool_structure_lock);
			p = attrib_pool.insert(std::pair<Derived, int>(d, 0) ).first;
			pthread_rwlock_unlock(& attrib_pool_structure_lock);
		}

		pthread_mutex_lock(& attrib_pool_count_lock);
		p->second += n;
		pthread_mutex_unlock( & attrib_pool_count_lock);
		return &(p->first);
	}

	static const Derived * static_find_attrib ( const Derived & d) {
		pthread_rwlock_rdlock(& attrib_pool_structure_lock);
		register typename map < Derived, int >::iterator p = attrib_pool.find( d );
		if ( p == attrib_pool.end() ) {
			pthread_rwlock_unlock ( & attrib_pool_structure_lock);
			return NULL;
		}
		pthread_rwlock_unlock ( & attrib_pool_structure_lock);
		return &(p->first);
	}

	static const Derived * static_reduce_attrib( const Derived & d , const unsigned int n) {
		pthread_rwlock_rdlock ( & attrib_pool_structure_lock);
		register typename map < Derived, int >::iterator p = attrib_pool.find( d );
		const bool is_end = ( p == attrib_pool.end() );
		pthread_rwlock_unlock (& attrib_pool_structure_lock);
		if ( is_end ) {
			d.print(std::cout);
			//const string * t = * ( d.get_attrib_set_pointer()->begin() );

			throw cException_Other("Error: attrib not exist!");
		}


		pthread_mutex_lock ( & attrib_pool_count_lock);
		p->second -= n;
		pthread_mutex_unlock ( & attrib_pool_count_lock);

		if ( p->second <= 0 ) {
			pthread_rwlock_wrlock ( & attrib_pool_structure_lock);
			//std::cout << p->second << " : ";
			//d.print(std::cout);
			attrib_pool.erase(p);
			pthread_rwlock_unlock ( & attrib_pool_structure_lock);
			return NULL;
		}
		return &(p->first);
	}

	const cAttribute* clone() const {
		const Derived & alias = dynamic_cast< const Derived & > (*this);
		return static_add_attrib(alias, 1);
	}




	static const string * static_add_string ( const string & str ) {
		register set< string >::iterator p = data_pool.find(str);
		if ( p == data_pool.end() ) {
			p = data_pool.insert(str).first;
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
				++cnt;
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

/*
 * cAttribute_Set_Intermediary:
 * Third layer of the attribute hierarchy. Specifically designed to handle the data storage issue.
 *
 * Private:
 * 		static vector < const string * > temporary_storage: static member temporarily used for data loading.
 * Protected:
 * 		vector < const string * > & get_data_modifiable(): still used for data loading only.
 * Public:
 * 		const vector < const string * > & get_data() const: override the base function and throw an error,
 * 			indicating that this function should be forbidden for this class and its child classes.
 */

template < typename AttribType >
class cAttribute_Set_Intermediary : public cAttribute_Intermediary<AttribType> {
private:
	static vector < const string * > temporary_storage;
protected:
	vector < const string * > & get_data_modifiable() { return temporary_storage; }
public:
	const vector < const string * > & get_data() const { throw cException_Invalid_Function("Function Disabled"); }
};

/*
 * cAttribute_Vector_Intermediary:
 * Third layer of the attribute hierarchy to handle the data storage, too.
 * Private:
 * 		vector < const string * > vector_string_pointers: the real data member.
 * protected:
 * 		vector < const string * > & get_data_modifiable(): for data loading only.
 * Public:
 * 		const vector < const string * > & get_data() const: for external call.
 * 		bool operator < ( const cAttribute & rhs ) const: sorting function used in map/set only. should not call explicitly.
 */



template < typename AttribType >
class cAttribute_Vector_Intermediary: public cAttribute_Intermediary<AttribType> {
	friend class cAttribute;
	friend class cAttribute_Intermediary<AttribType> ;
	template < typename T1, typename T2 > friend  class cAttribute_Interactive_Mode;

private:
	vector < const string * > vector_string_pointers;
protected:
	vector < const string * > & get_data_modifiable() { return vector_string_pointers;}
public:
	const vector < const string * > & get_data() const { return vector_string_pointers;}
	bool operator < ( const cAttribute & rhs ) const { return this->get_data() < rhs.get_data(); }
};





/*
 * template < Concreate Class Name > cAttribute_Set_Mode
 * This is the second layer of abstract class, implementing more detailed certain class behaviors.
 * The set_mode class is especially for attributes like "co-authors" and "patent classes", which usually have multiple values in one record, whose
 * comparison behavior is to find the number of common elements between two records, and whose merging behavior is grouping all distinct values together
 * for each cluster ( or disambiguated and grouped records ).
 *
 * Protected:
 * 		set < const string * > attrib_set: this is the actual data member that will be used in the storage and comparison of its concrete subclasses.
 *											Instead, the data member in the base cAttribute class, "data", should not be used unless necessary.
 *		const cAttribute * attrib_merge ( const cAttribute & right_hand_side) const: the polymorphic attribute merge function customized for set mode. Override in the child class if necessary.
 *
 * Private:
 * Public:
 * 		const set < const string *> * get_attrib_set_pointer() const: to get the pointer to the std::set, which stores all the data for this mode. This overrides the function in the base class.
 * 		unsigned int compare(const cAttribute & right_hand_side) const: The default comparison function of the set mode, which returns the number of common elements between two classes.
 * 																		Override it in the child class if other scoring method is used.
 * 		bool split_string(const char* inputdata): polymorphic function to extract data from the input string.
 * 		bool operator < ( const cAttribute & rhs ) const: overloading operator which overrides the base class one. Used only in internal binary tree sort. Do not call explicitly.
 *		void print( std::ostream & os ) const: polymorphic print function. os can be a file stream, or std::cout if outputting to the screen.
 *		bool is_informative() const: polymorphic function, returning false, indicating that this type of class does not support this function.
 */


template < typename AttribType >
class cAttribute_Set_Mode : public cAttribute_Set_Intermediary < AttribType > {
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

private:

	//const vector < const string *> & get_data() const {throw cException_Other ("No vector data. Invalid operation."); return cAttribute::get_data();}
	//do not override the get_data() function because it is used to initially load data from the txt file.
public:

	const set < const string *> * get_attrib_set_pointer() const { return & attrib_set;}
	unsigned int compare(const cAttribute & right_hand_side) const {
		if ( ! this->is_comparator_activated () )
			throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
		try {

			unsigned int res = 0;
			const AttribType & rhs = dynamic_cast< const AttribType & > (right_hand_side);

			const unsigned int mv = this->get_attrib_max_value();
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
		for ( vector < const string *>::const_iterator p = this->get_data_modifiable().begin(); p != this->get_data_modifiable().end(); ++p ) {
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
		os << "No raw data. " << "# of Derivatives = " << attrib_set.size() << ", Derivatives = ";

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


/*
 * template < Concrete Attribute Class Name > cAttribute_Single_Mode:
 * This is the second layer of the inheritance hierarchy, specifically designed for those attributes which have only one useful piece of information,
 * and whose comparison function is only comparing such information between two attributes.
 * The information stored in the class is usually read-only, unless necessary to change.
 * Typical classes of such mode include firtname, lastname, assignee, unique_record_id, city, etc.
 *
 * Public:
 * 		unsigned int compare(const cAttribute & right_hand_side) const: Default Jaro-Winkler comparison between the strings. Scoring is user-defined, so feel free to override.
 * 		bool split_string(const char* inputdata): read input string, do some preparations (edition and pooling) and save in the object.
 */

template < typename AttribType >
class cAttribute_Single_Mode : public cAttribute_Vector_Intermediary<AttribType> {
public:
	unsigned int compare(const cAttribute & right_hand_side) const {
		// ALWAYS CHECK THE ACTIVITY OF COMPARISON FUNCTION !!
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

/*
 * template < Concrete Attribute Class Name > cAttribute_Vector_Mode:
 * This is the second layer of the inheritance hierarchy, specifically designed for those attributes which have several values in one record,
 * whose comparison function aims to find the maximum(minimum) value or the number of common values, and whose contents are usually read-only.
 * No attribute in this project is using this mode yet. This interface is designed as an example of expandability of functionality.
 *
 * Public:
 * 		unsigned int compare(const cAttribute & right_hand_side) const: calculating the aggregate score of common elements. Override if necessary.
 * 		bool split_string(const char* inputdata): read input string, do some preparations (edition and pooling) and save in the object.
 */



template < typename AttribType >
class cAttribute_Vector_Mode : public cAttribute_Vector_Intermediary<AttribType> {
public:
	unsigned int compare(const cAttribute & right_hand_side) const {
		//ALWAYS CHECK THE COMPARATOR!
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

#if 0
/*
 *  template < Concrete Class Name > cAttribute_Single_Interactive_Mode (OBSOLETE NOW) :
 *  This type of class shares the same storage behaviors as the Singe_Mode, however, it stores additional information to allow its interaction with
 *  other classes. A typical example is the Latitude class, which also needs information about longitude, street, country, city, etc.
 *  The concrete child class is responsible for the configuration of the interactive elements, by calling the config_interactive function, during either
 *  initialization or reconfiguration, or both. To see reconfiguration methods, look at the cRecord_Reconfigurator class hierarchy in the "DisambigEngine.h".
 *  The comparison function is a pure virtual function here, forcing concrete child classes to implement.
 *
 *  Private:
 *  	static map < Inter_Vector, unsigned int > Interactive_Pool: another pooling system for interactive_vector data.
 *  	const Inter_Vector * pInteractive: the pointer that points to the interactive vector which is pooled.
 *  	static bool has_reconfiged: static variable to check whether such class is reconfigured. It is necessary because it has interactive members.
 *   	static const Inter_Vector * add_interactive_vector(const Inter_Vector & inputvec, const unsigned int n ):
 *   				add the interactive vector into the pooling system by reference-counting. returns the pointer. For internal use only.
 *   	static const Inter_Vector * reduce_interactive_vector(const Inter_Vector & inputvec, const unsigned int n )
 *   				reduce the interactive vector into the pooling system by reference-counting. returns the pointer. Null if counter=0. For internal use only.
 *
 *  Public:
 *  	cAttribute_Single_Interactive_Mode(const char * source = NULL ): default constructor. Set initial pointer to NULL, forcing reconfiguration.
 *  	const vector <const cAttribute *> & get_interactive_vector() const: Returns the interactive data.
 *  	bool operator < ( const cAttribute & rhs ) const: overridden version of less than operator. Necessary because interactive data are also involved. Do not call explicitly.
 *  	const cAttribute* config_interactive (const vector <const cAttribute *> &inputvec ) const:
 *  			create or find an attribute object such that interactive_data = inputvec, with the same local data. Returns the pointer to the object.
 *		unsigned int compare(const cAttribute & rhs) const = 0: Pure virtual function, forcing child class to implement.
 *		static bool check_if_reconfigured(): check if the class is reconfigured. Throw an error if false.
 *
 *	ATTENTION: WHEN OVERRIDING THE COMPARISON FUNCTION OF THIS CLASS, "check_if_reconfigured()" MUST BE CALLED.
 */


template < typename AttribType >
class cAttribute_Single_Interactive_Mode: public cAttribute_Single_Mode < AttribType > {
	friend class cReconfigurator_Interactives;
	typedef vector<const cAttribute *> Inter_Vector;
private:
	static std::auto_ptr < const cRecord_Reconfigurator > preconfig;
	static void obtain_interactive_reconfigurator( const cAttribute * p) {
		std::auto_ptr < const cRecord_Reconfigurator > tmp_ptr (generate_interactive_reconfigurator(p));
		preconfig = tmp_ptr;
	}

private:
	static map < Inter_Vector, unsigned int > Interactive_Pool;
	static bool has_reconfiged;
	const Inter_Vector * pInteractive;

	static const Inter_Vector * add_interactive_vector(const Inter_Vector & inputvec, const unsigned int n ) {
		map < Inter_Vector, unsigned int> ::iterator p = Interactive_Pool.find(inputvec);
		if ( p == Interactive_Pool.end() )
			p = Interactive_Pool.insert(std::pair<Inter_Vector,unsigned int>(inputvec, n)).first;
		else
			p->second += n;

		return &(p->first);
	}

	static const Inter_Vector * reduce_interactive_vector(const Inter_Vector & inputvec, const unsigned int n ) {
		map < Inter_Vector, unsigned int> ::iterator p = Interactive_Pool.find(inputvec);
		if ( p == Interactive_Pool.end() )
			throw ( cException_Other ("Vector not found"));
		if ( p->second < n )
			throw ( cException_Other ("Too many deductions."));

		p->second -= n;

		if ( p->second == 0 ) {
			Interactive_Pool.erase(p);
			return NULL;
		}
		else
			return &(p->first);
	}

	const cAttribute* config_interactive (const vector <const cAttribute *> &inputvec ) const {
		if ( pInteractive != NULL ) {
			std::cout << "=====REFUSE to reset interactives. Pointer was initialized before." << std::endl;
			return NULL;
		}
		AttribType temp(dynamic_cast< const AttribType &>(*this) );

		temp.cAttribute_Single_Interactive_Mode::pInteractive = temp.cAttribute_Single_Interactive_Mode::add_interactive_vector(inputvec, 1);
		const cAttribute * pattrib_in_pool = temp.add_attrib(1);
		has_reconfiged = true;
		return pattrib_in_pool;
	}
	void reconfigure_for_interactives( const cRecord * pRec) const {
		if ( preconfig.get() == NULL )
			obtain_interactive_reconfigurator(this);
		reconfigure_interactives ( preconfig.get(), pRec);
	}

public:
	cAttribute_Single_Interactive_Mode(const char * source = NULL ) : pInteractive(NULL) {}
	const vector <const cAttribute *> & get_interactive_vector() const {return *pInteractive;}
	bool operator < ( const cAttribute & rhs ) const {
		if ( this->get_data() < rhs.get_data() )
			return true;
		else if ( rhs.get_data() < this->get_data() )
			return false;
		else
			return & (this->get_interactive_vector() ) < & ( rhs.get_interactive_vector() );
	}


	static bool check_if_reconfigured() {
		if ( ! has_reconfiged )
			throw cException_Other("Interactive class has not been reconfigured yet.");
		return has_reconfiged;
	}

	unsigned int compare(const cAttribute & rhs) const = 0;		//forcing child class to implement because it is unknown.
};
#endif

/*
 * cAttribute_Interactive_Mode:
 * This template class is used when a certain class has interaction with other class. To use the mode, a concrete non-interactive
 * class should be defined first, followed by the inheritance of the interactive class from the template which includes the non-interactive
 * data type.
 * Two classes can be defined as interactive with each other.
 * For example.
 * To create a cLatitude class:
 * 	1. cLatitude_Data : public cAttribute_Single_Mode<cLatitude_Data>;
 * 	2. cLatitude : public cAttribute_Interactive_Mode <cLatitude, cLatitude_Data>;
 */

template <typename ConcreteType, typename PooledDataType>
class cAttribute_Interactive_Mode : public cAttribute_Basic < ConcreteType > {
private:
	mutable PooledDataType * pAttrib;
	mutable vector<const cAttribute *> inter_vecs;
	static std::auto_ptr < const cRecord_Reconfigurator > preconfig;
	static list < ConcreteType > attrib_list;
	static bool has_reconfiged;
	static std::auto_ptr < PooledDataType > stat_pdata;

	vector < const string * > & get_data_modifiable() {
		if ( stat_pdata.get() == NULL )
			stat_pdata = std::auto_ptr < PooledDataType > ( new PooledDataType );

		return stat_pdata->get_data_modifiable();
	}

public:
	const cAttribute *  get_effective_pointer() const { return pAttrib;}

	const string * add_string ( const string & str ) const  {
		return cAttribute_Intermediary<ConcreteType>::static_add_string ( str );
	}
	const cAttribute* reduce_attrib(unsigned int n ) const {
		return pAttrib->template reduce_attrib(n);
	}
	const cAttribute* add_attrib(unsigned int n ) const {
		return pAttrib->template add_attrib(n);
	}
	int clean_attrib_pool() const { return 0; }

	cAttribute_Interactive_Mode ( const char * data = NULL ): pAttrib (NULL) {}
	bool split_string(const char* recdata) {
		if ( stat_pdata.get() == NULL )
			stat_pdata = std::auto_ptr < PooledDataType > ( new PooledDataType );
		stat_pdata->split_string(recdata);
		pAttrib = stat_pdata.get();
		return true;
	}

	bool operator < (const cAttribute&) const {throw cException_Other("operator less than. Operation forbidden.");}
	const vector <const cAttribute *> & get_interactive_vector() const {
		return inter_vecs;
	}
	const vector < const string * > & get_data() const { return pAttrib->get_data();}

	const cAttribute* clone() const {
		const PooledDataType  & alias = *this->pAttrib;
		const PooledDataType  * p = dynamic_cast < const PooledDataType  * > (alias.add_attrib(1) );
		if ( p == NULL )
			throw cException_Other("Clone error. Dynamic cast error.");

		pAttrib = const_cast < PooledDataType  * >(p);
		attrib_list.push_back(dynamic_cast<const ConcreteType &>(*this));
		return & attrib_list.back();
	}

	void obtain_interactive_reconfigurator() const {
		std::auto_ptr < const cRecord_Reconfigurator > tmp_ptr (generate_interactive_reconfigurator(this));
		preconfig = tmp_ptr;
	}
	static bool check_if_reconfigured() {
		if ( ! has_reconfiged )
			throw cException_Other("Interactive class has not been reconfigured yet.");
		return has_reconfiged;
	}
	void reconfigure_for_interactives( const cRecord * pRec) const {
		if ( preconfig.get() == NULL )
			obtain_interactive_reconfigurator();
		reconfigure_interactives ( preconfig.get(), pRec);
	}
	const cAttribute* config_interactive (const vector <const cAttribute *> &inputvec ) const {
		inter_vecs = inputvec;
		has_reconfiged = true;
		return this;
	}
	void print( std::ostream & os ) const {
		os << cAttribute_Basic<ConcreteType>::static_get_class_name() << " -- ";
		pAttrib->print(os);
		os << "Interactive attributes are: ";
		for ( vector<const cAttribute *>::const_iterator p = inter_vecs.begin(); p != inter_vecs.end(); ++p )
			os << (*p)->get_class_name() << ", ";
		os << std::endl;
	}
	int exact_compare( const cAttribute & right_hand_side ) const {
		const ConcreteType & rhs = dynamic_cast< const ConcreteType & > (right_hand_side);
		if ( this->get_effective_pointer() != rhs.get_effective_pointer() )
			return 0;

		const unsigned int n = this->get_interactive_vector().size();
		if ( n != rhs.get_interactive_vector().size() )
			throw cException_Other("Different data dimensions.");

		for ( unsigned int i = 0; i < n; ++i ) {
			if ( this->get_interactive_vector().at(i)->get_effective_pointer()
					!= rhs.get_interactive_vector().at(i)->get_effective_pointer() )
			return 0;
		}

		return 1;
	}
};



















//declaration of static member
template < typename AttribType> vector < const string * > cAttribute_Set_Intermediary<AttribType>::temporary_storage;
template <typename ConcreteType, typename PooledDataType> std::auto_ptr < const cRecord_Reconfigurator > cAttribute_Interactive_Mode<ConcreteType, PooledDataType>::preconfig;
template <typename ConcreteType, typename PooledDataType> list < ConcreteType > cAttribute_Interactive_Mode<ConcreteType, PooledDataType>::attrib_list;
template <typename ConcreteType, typename PooledDataType> bool cAttribute_Interactive_Mode<ConcreteType, PooledDataType>::has_reconfiged = false;
template <typename ConcreteType, typename PooledDataType> std::auto_ptr < PooledDataType > cAttribute_Interactive_Mode<ConcreteType, PooledDataType>::stat_pdata;

#endif /* DISAMBIGLIB_HPP_ */

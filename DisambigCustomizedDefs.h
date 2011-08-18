/*
 * DisambigCustomizedDefs.h
 *
 *  Created on: Apr 5, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGCUSTOMIZEDDEFS_H_
#define DISAMBIGCUSTOMIZEDDEFS_H_

#include "DisambigDefs.h"
#include "DisambigComp.h"

/*
 * For each concrete class, if it can be one of the components in the similarity profile, whether its comparator is activated or not,
 * a "STATIC CONST UNSIGNED INT" member, namely max_value, should be declared and defined in the concrete class. At the same time,
 * the virtual function get_attrib_max_value() const has to be overridden, with comparator activity check inside.
 *
 * Each concrete class should also provide a default constructor ( which is actually very simple ).
 */


/*
 * IMPORTANT:
 * FOR EACH CONCRETE CLASS, IF ITS STATIC MEMBER IS DIFFERENT FROM DEFAULT, A TEMPLATE SPECIALIZATION MUST BE DECLARED BEFORE THE DEFAULT IS
 * DECLARED AND DEFINED. AFTER THAT, THE SPECIALIZED DEFINITION SHOULD APPEAR IN THE CPP FILE.
 * For example, the attrib group of cFirstname is "Personal" instead of the default "NONE". In this case, the specialized declaration must be made
 * before the general default declaration. i.e.
 * template <> const string cAttribute_Intermediary<cFirstname>::attrib_group;
 * appears before:
 * template <typename Derived> const string cAttribute_Intermediary<Derived>::attrib_group = INERT_ATTRIB_GROUP_IDENTIFIER;
 * And then in the cpp file, there is:
 * template <> const string cAttribute_Intermediary<cFirstname>::attrib_group = "Personal";
 *
 *
 * See more information and example in the end of this file.
 */


class cFirstname : public cAttribute_Single_Mode <cFirstname> {
private:
  static unsigned int previous_truncation;
  static unsigned int current_truncation;
 public:
  static void set_truncation( const unsigned int prev, const unsigned int cur ) { previous_truncation = prev; current_truncation = cur; }
	//static const unsigned int max_value = Jaro_Wrinkler_Max;
	static const unsigned int max_value = 4;

	cFirstname(const char * source = NULL) {}
	bool split_string(const char*);		//override because some class-specific splitting is involved.
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//override the base class to enable the functionality of the function.
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
	unsigned int compare(const cAttribute & right_hand_side) const ;
};


class cLastname : public cAttribute_Single_Mode <cLastname> {
public:
	static const unsigned int max_value = Jaro_Wrinkler_Max;

	cLastname(const char * source = NULL ) {}
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cMiddlename : public cAttribute_Single_Mode <cMiddlename> {
public:
	static const unsigned int max_value = 3;

	cMiddlename(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;		//override to allow customization.
	bool split_string(const char*);
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cLatitude_Data : public cAttribute_Single_Mode<cLatitude_Data> {};

class cLatitude : public cAttribute_Interactive_Mode <cLatitude, cLatitude_Data> {
private:
	static const unsigned int max_value = 5;
public:
	cLatitude(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;	//override to customize
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
};

class cLongitude_Data : public cAttribute_Single_Mode<cLongitude_Data> {};

class cLongitude: public cAttribute_Interactive_Mode <cLongitude, cLongitude_Data > {
private:
	static const unsigned int max_value = 1;
public:
	cLongitude(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;	//override to customize
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
};

class cStreet: public cAttribute_Single_Mode <cStreet> {
public:
	cStreet(const char * source = NULL) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE Street IS NOT BEING COMPARED either. IT IS WITH THE LATITUDE COMPARISION.

};

class cCountry: public cAttribute_Single_Mode <cCountry> {
public:
	static unsigned int const max_value = 2;
	cCountry(const char * source = NULL ) {}
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
	unsigned int compare(const cAttribute & right_hand_side) const;

};


// cClass and cCoauthor are in set_mode, not single_mode
class cClass: public cAttribute_Set_Mode < cClass > {
public:
	static unsigned int const max_value = 4;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	//unsigned int compare(const cAttribute & rhs) const;
};

//the second way to measure class. literally the same as cClass except for the comparison function.
class cClass_M2 : public cAttribute_Set_Mode < cClass_M2 > {
public:
	static const unsigned int max_value = 4;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	unsigned int compare(const cAttribute & right_hand_side) const;
};


class cCoauthor : public cAttribute_Set_Mode < cCoauthor >  {
	friend class cReconfigurator_Coauthor;
public:
	static unsigned int const max_value = 6;

	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
};

class cAsgNum;
class cAssignee_Data : public cAttribute_Single_Mode < cAssignee_Data > {};

class cAssignee : public cAttribute_Interactive_Mode <cAssignee, cAssignee_Data> {
public:
	static const unsigned int max_value = 6;
private:
	//static const map<string, std::pair<string, unsigned int>  > * assignee_tree_pointer; // this is a static membmer used in the comparison function.
	static map < const cAsgNum*, unsigned int > asgnum2count_tree;
	static bool is_ready;
public:
	cAssignee(const char * source = NULL ) {}
	unsigned int compare(const cAttribute & rhs) const;
	//static void set_assignee_tree_pointer(const map<string, std::pair<string, unsigned int>  >& asgtree) {assignee_tree_pointer = & asgtree;}
	static void configure_assignee( const list <const cRecord *> & );
	unsigned int get_attrib_max_value() const {
		if ( ! is_comparator_activated() )
			cAttribute::get_attrib_max_value();
		return max_value;
	}
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cAsgNum : public cAttribute_Single_Mode<cAsgNum> {
public:
	cAsgNum(const char * source = NULL ){}
};

class cUnique_Record_ID : public cAttribute_Single_Mode <cUnique_Record_ID> {
public:
	cUnique_Record_ID(const char * source = NULL ){}
};

class cApplyYear: public cAttribute_Single_Mode<cApplyYear> {
public:
	cApplyYear(const char * source = NULL ){}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
};

class cCity: public cAttribute_Single_Mode <cCity> {
public:
	cCity(const char * source = NULL ) {}
	//SHOULD NOT OVERRIDE THE COMPARISON FUNCTION SINCE LONGITUDE IS NOT BEING COMPARED. IT IS WITH THE LATITUDE COMPARISION.
	bool split_string(const char*);
	int exact_compare( const cAttribute & rhs ) const { return this == & rhs; }
};

class cPatent: public cAttribute_Single_Mode <cPatent> {
public:
	cPatent( const char * source = NULL){};
};



//=======

// template static variables.
// ALL THE FOLLOWING TEMPLATE MEMBER ARE DEFAULT VALUES.
// Declaration and default definition.
// Specialization should be implemented in the cpp file.
//template <typename Derived> unsigned int cAttribute_Intermediary<Derived>::column_index_in_query;
//template <typename Derived> string cAttribute_Intermediary<Derived>::column_name_in_query;
template <typename Derived> vector <unsigned int> cAttribute_Basic<Derived>::interactive_column_indice_in_query;
template <typename Derived> bool cAttribute_Basic<Derived>::bool_interactive_consistency_checked = false;
template <typename Derived> bool cAttribute_Basic<Derived>::bool_is_enabled = false;
template <typename Derived> bool cAttribute_Basic<Derived>::bool_comparator_activated = false;
template <typename Derived> set < string > cAttribute_Intermediary<Derived>:: data_pool;
template <typename Derived> map < Derived, int > cAttribute_Intermediary<Derived>:: attrib_pool;
template <typename Derived> pthread_rwlock_t cAttribute_Intermediary<Derived>:: attrib_pool_structure_lock = PTHREAD_RWLOCK_INITIALIZER;
template <typename Derived> pthread_mutex_t cAttribute_Intermediary<Derived>:: attrib_pool_count_lock = PTHREAD_MUTEX_INITIALIZER;

//declaration ( not definition ) of specialized template

template <> const string cAttribute_Basic<cFirstname>::attrib_group;
template <> const string cAttribute_Basic<cLastname>::attrib_group;
template <> const string cAttribute_Basic<cMiddlename>::attrib_group;
template <> const string cAttribute_Basic<cLatitude>::attrib_group;
template <> const string cAttribute_Basic<cLongitude>::attrib_group;
template <> const string cAttribute_Basic<cAssignee>::attrib_group;
template <> const string cAttribute_Basic<cClass>::attrib_group;
template <> const string cAttribute_Basic<cCoauthor>::attrib_group;
template <> const string cAttribute_Basic<cClass_M2>::attrib_group;

template <typename Derived> const string cAttribute_Basic<Derived>::attrib_group = INERT_ATTRIB_GROUP_IDENTIFIER;

//template <> const string cAttribute_Intermediary<cLatitude>::interactive_column_names[];
template <> const string cAttribute_Basic<cAssignee>::interactive_column_names[];
template <> const string cAttribute_Basic<cLatitude >::interactive_column_names[];
template <> const string cAttribute_Basic<cLongitude >::interactive_column_names[];
template <typename Derived> const string cAttribute_Basic<Derived>::interactive_column_names[] = {};


//template <> const unsigned int cAttribute_Intermediary<cLatitude>::num_of_interactive_columns;
template <> const unsigned int cAttribute_Basic<cAssignee>::num_of_interactive_columns;
template <> const unsigned int cAttribute_Basic<cLatitude >::num_of_interactive_columns;
template <> const unsigned int cAttribute_Basic<cLongitude >::num_of_interactive_columns;
template <typename Derived> const unsigned int cAttribute_Basic<Derived>::num_of_interactive_columns = 0;

#endif /* DISAMBIGCUSTOMIZEDDEFS_H_ */

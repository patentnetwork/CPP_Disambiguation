/*
 * DisambigCustomizedDefs.cpp
 *
 *  Created on: Apr 5, 2011
 *      Author: ysun
 */

#include "DisambigCustomizedDefs.h"

template <> const string cAttribute_Intermediary<cFirstname>::class_name = "Firstname";
template <> const string cAttribute_Intermediary<cFirstname>::attrib_group = "Personal";


template <> const string cAttribute_Intermediary<cLastname>::class_name = "Lastname";
template <> const string cAttribute_Intermediary<cLastname>::attrib_group = "Personal";

template <> const string cAttribute_Intermediary<cMiddlename>::class_name = "Middlename";
template <> const string cAttribute_Intermediary<cMiddlename>::attrib_group = "Personal";

template <> const string cAttribute_Intermediary<cLatitude>::class_name = "Latitude";
template <> const string cAttribute_Intermediary<cLatitude>::interactive_column_names[] = {"Longitude", "Street", "Country"};
template <> const unsigned int cAttribute_Intermediary<cLatitude>::num_of_interactive_columns = 3;
template <> const string cAttribute_Intermediary<cLatitude>::attrib_group = "Patent";


template <> const string cAttribute_Intermediary<cLongitude>::class_name = "Longitude";

template <> const string cAttribute_Intermediary<cStreet>::class_name = "Street";


template <> const string cAttribute_Intermediary<cCountry>::class_name = "Country";
//template <> const string cAttribute_Intermediary<cCountry>::attrib_group = "Patent";

template <> const string cAttribute_Intermediary<cClass>::class_name = "Class";
template <> const string cAttribute_Intermediary<cClass>::attrib_group = "Patent";

template <> const string cAttribute_Intermediary<cClass_M2>::class_name = "Class_Measure2";
template <> const string cAttribute_Intermediary<cClass_M2>::attrib_group = "Patent";


template <> const string cAttribute_Intermediary<cCoauthor>::class_name = "Coauthor";
template <> const string cAttribute_Intermediary<cCoauthor>::attrib_group = "Patent";

template <> const string cAttribute_Intermediary<cAssignee>::class_name = "Assignee";
template <> const string cAttribute_Intermediary<cAssignee>::interactive_column_names[] = {"AsgNum"};
template <> const unsigned int cAttribute_Intermediary<cAssignee>::num_of_interactive_columns = 1;
//const map<string, std::pair<string, unsigned int>  > * cAssignee::assignee_tree_pointer;
map < const cAsgNum*, unsigned int > cAssignee:: asgnum2count_tree;
bool cAssignee::is_ready = false;
template <> const string cAttribute_Intermediary<cAssignee>::attrib_group = "Patent";

template <> const string cAttribute_Intermediary<cAsgNum>::class_name = "AsgNum";

template <> const string cAttribute_Intermediary<cUnique_Record_ID>::class_name = "Unique_Record_ID";

template <> const string cAttribute_Intermediary<cApplyYear>::class_name = "ApplyYear";

template <> const string cAttribute_Intermediary<cCity>::class_name = "City";

template <> const string cAttribute_Intermediary<cPatent>::class_name = "Patent";


/*
 * Whenever overriding a comparison fucntion, it is extremely important to check if the comparator is activated. i.e.
 * Always keep the following statement in a comparison function:
 *
 * if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
 *
 *
 */


unsigned int cFirstname::previous_truncation = 0;
unsigned int cFirstname::current_truncation = 0;

/*
 * cFirstname::split_string does 3 things:
 * 1. Extract the first name from an input string, which is usually mixed with first name and middle name.
 * 	  i.e. Input string = "JOHN David WILLIAM", extracted string = "JOHN"
 * 2. Keep the original copy of the original input string.
 * 3. Save the first extracted string in data[0], and the second original copy in data[1].
 *
 */


bool cFirstname::split_string(const char *inputdata) {
	static const char delim = ' ';
	cAttribute::split_string(inputdata);
	const string * psource = get_data().at(0);
	const size_t pos = psource->find(delim);
	vector < const string * > & data_alias = get_data_modifiable();
	if ( pos == string::npos )
		data_alias.push_back(psource);
	else {
		string to_push ( psource->begin(), psource->begin() + pos );
		data_alias.push_back(this->add_string( to_push) );
	}
	return true;
}


unsigned int cFirstname::compare(const cAttribute & right_hand_side) const {
	// ALWAYS CHECK THE ACTIVITY OF COMPARISON FUNCTION !!
	if ( ! this->is_comparator_activated () )
		throw cException_No_Comparision_Function(this->static_get_class_name().c_str());
	if ( this == & right_hand_side )
		return this->get_attrib_max_value();

	unsigned int res = 0;
	res = name_compare(* this->get_data().at(1), * right_hand_side.get_data().at(1), previous_truncation, current_truncation);
	if ( res > this->get_attrib_max_value() )
		res = this->get_attrib_max_value();
	return res;
}



/*
 * cMiddlename::split_string does 3 things:
 * 1. Extract the middle name from an input string, which is usually mixed with first name and middle name.
 * 	  i.e. Input string = "JOHN David WILLIAM", extracted string = "David WILLIAM"
 * 		   Input string = "JOHN", extracted string = ""
 *
 * 2. Extract the initials of middle names.
 * 	  i.e. Input string = "JOHN DAVID WILLIAM", middle initials = "DW"
 * 3. Save the first extracted string in data[0], and the second middle initial string in data[1].
 *
 *
 */



bool cMiddlename::split_string(const char *inputdata) {
	cAttribute::split_string(inputdata);
	const string & source = * get_data().at(0);
	size_t pos = source.find(' ');
	string midpart;
	if ( pos == string::npos )
		midpart = "";
	else
		midpart = source.substr( pos + 1 );

	char initials[64];
	extract_initials(initials, midpart.c_str());
	const char * start = initials;
	vector < const string * > & data_alias = get_data_modifiable();
	string temp(start);
	data_alias.clear();
	data_alias.push_back(this->add_string( midpart) );
	data_alias.push_back(this->add_string( temp ) );
	return true;
}


/*
 * cMiddlename::compare:
 * Compare the extracted strings in data[0] to see whether they started with the same letter and whether one contains the other.
 * i.e.
 * "DAVID WILLIAM" vs "DAVID" = 3 ( max score)
 * "DAVID WILLIAM" vs "WILLIAM" = 0 (min score, not starting with the same letters)
 * "DAVID WILLIAM" vs "DAVE" = 0 ( either one does not container the other. )
 * "" vs "" = 2 ( both missing information )
 * "DAVID" vs "" = 1 ( one missing information )
 *
 */


unsigned int cMiddlename::compare(const cAttribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	try {
		const cMiddlename & rhs = dynamic_cast< const cMiddlename & > (right_hand_side);
		unsigned int res = midnamecmp(* this->get_data().at(0), * rhs.get_data().at(0));
		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}



/*
 * cLatitude::compare:
 * Such comparison is complicated because cLatitude is interacted with cLongitude, cCountry , and possibly cStreet
 * One needs to know the structure of how the interactive data are stored, which is actually an assumed knowledge.
 *
 * If the distance calculated by latitude and longitude < 1 mile, score = 4 ( max score)
 * If the distance calculated by latitude and longitude < 10 mile, score = 3
 * If the distance calculated by latitude and longitude < 50 mile, score = 2
 * If the distance calculated by latitude and longitude > 50 mile, score = 1
 * If countries are different, score = 0;
 *
 */

unsigned int cLatitude::compare(const cAttribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	check_if_reconfigured();
	try {
		unsigned int res = 0;
		const cLatitude & rhs = dynamic_cast< const cLatitude & > (right_hand_side);

		const cAttribute* const & this_longitude = this->get_interactive_vector().at(0);
		const cAttribute* const & rhs_longitude = rhs.get_interactive_vector().at(0);
		if ( this->get_data().size() != this_longitude->get_data().size() ) {
			std::cout << "Alignment error in latitude comparison: " << std::endl;
			this->print(std::cout);
			this_longitude->print(std::cout);
			throw cException_Interactive_Misalignment(this->get_class_name().c_str());
		}
		if ( rhs.get_data().size() != rhs_longitude->get_data().size() ) {
			std::cout << "Alignment error in latitude comparison: " << std::endl;
			rhs.print(std::cout);
			rhs_longitude->print(std::cout);
			throw cException_Interactive_Misalignment(this->get_class_name().c_str());
		}


		
		//latitude interacts with		{"Longitude", "Street", "Country"}; the sequence is important.
		//vector <const string* >::const_iterator p, q;

		// Comparing country
		if ( this == &rhs || this->is_informative())
			return max_value;

		unsigned int country_score = 0;
		if ( this->get_interactive_vector().at(2) == rhs.get_interactive_vector().at(2) )
			country_score = 1;

		// Comparing street;
		//unsigned int street_score = 0;

		// Comparing Latitidue and longitude
		unsigned int latlon_score = 0;

		latlon_score = latloncmp ( * this->get_data().at(0), * this->get_interactive_vector().at(0)->get_data().at(0),
									* rhs.get_data().at(0), * rhs.get_interactive_vector().at(0)->get_data().at(0) );

		if ( country_score == 0 )
			res = 0;
		else
			res = latlon_score;

		 #if 0

		//obsolete now.
		const string * p1 = this->get_data().front();
		const string * q1 = this->get_interactive_vector().at(0)->get_data().front();
		const string * p2 = rhs.get_data().front();
		const string * q2 = rhs.get_interactive_vector().at(0)->get_data().front();

		if ( ( ! p1->empty() && !p2->empty() && p1 != p2 ) || ( !q1->empty() && ! q2->empty() && q1 != q2 ) )
			res = 0;
		else if ( ( ! p1->empty() && !p2->empty() && p1 == p2 ) && ( !q1->empty() && ! q2->empty() && q1 == q2 ) )
			res = 2;
		else
			res = 1;
		#endif

		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}


/*
 * cClass_M2::compare
 * A second way to score the "class" attribute.
 * Not in use now.
 */


unsigned int cClass_M2::compare(const cAttribute & right_hand_side) const {
	const cClass_M2 & rhs = dynamic_cast< const cClass_M2 & > (right_hand_side);
	const unsigned int common = this->cAttribute_Set_Mode <cClass_M2>::compare( rhs );
	const unsigned int this_size = this->attrib_set.size();
	const unsigned int rhs_size = rhs.attrib_set.size();

	const double factor = 1.0 * common * common / this_size / rhs_size;
	if ( factor > 0.3 )
		return 4;
	else if ( factor > 0.2 )
		return 3;
	else if ( factor > 0.1 )
		return 2;
	else if ( factor > 0.05 )
		return 1;
	else
		return 0;
}

/*
 * cCountry::compare
 * Not supposed to be used, because country attribute is mixed in the latitude comparison.
 *
 */

unsigned int cCountry::compare(const cAttribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());

	if ( !this->is_informative() || ! right_hand_side.is_informative() )
		return 1;
	if ( this == & right_hand_side )
		return 2;
	else
		return 0;
}



/*
 * cAssignee::compare:
 * Comparison of assignee includes two steps:
 * 1. look up the assignee->asgnum(assignee number) tree, in order to check whether two assignees shared the same number.
 * 		If they do, they are believed to be the same, and the score of 5 is granted. If the assignee has fewer than 100 different patents, an additional 1 point is added.
 * 2. If their assignee numbers are different, they can still be the same assignee.
 *    In this case, a fairly crude jaro-winkler string comparison is used to score.
 *    Refer to the function jwcmp for more scoring information.
 *
 */

unsigned int cAssignee::compare(const cAttribute & right_hand_side) const {
	if ( ! is_comparator_activated () )
		throw cException_No_Comparision_Function(static_get_class_name().c_str());
	if ( ! cAssignee::is_ready )
		throw cException_Other("Trees for assignee comparison are not set up yet. Run cAssignee::configure_assignee first.");
	try {

		const cAssignee & rhs = dynamic_cast< const cAssignee & > (right_hand_side);
		//unsigned int res = asgcmp(this->get_data(), rhs.get_data(), assignee_tree_pointer);
		//unsigned int res = asgcmp ( * this->get_data().at(0), * rhs.get_data().at(0), assignee_tree_pointer);
		unsigned int res = 0;
		const cAsgNum * p = dynamic_cast < const cAsgNum *> (this->get_interactive_vector().at(0));
		if ( ! p )
			throw cException_Other("Cannot dynamic cast to cAsgNum *.");

		const cAsgNum * q = dynamic_cast < const cAsgNum *> (rhs.get_interactive_vector().at(0));
		if ( ! q )
			throw cException_Other("Cannot dynamic cast rhs to cAsgNum *.");

		if ( ! this->is_informative() || ! rhs.is_informative() ) {
			res = 1;
		}
		else if ( p != q ) {
			res = asgcmp(* this->get_data().at(0), * rhs.get_data().at(0));
		}
		else {
			res = 5;
			map < const cAsgNum *, unsigned int>::const_iterator t = cAssignee::asgnum2count_tree.find(p);
			if ( t == cAssignee::asgnum2count_tree.end() )
				throw cException_Other("AsgNum pointer is not in tree.");
			if ( t->second < 100 )
				++res;
		}

		if ( res > max_value )
			res = max_value;
		return res;
	}
	catch ( const std::bad_cast & except ) {
		std::cerr << except.what() << std::endl;
		std::cerr << "Error: " << this->get_class_name() << " is compared to " << right_hand_side.get_class_name() << std::endl;
		throw;
	}
}

/*
 * The reason to override cCity::split_string from its base is because a city's name can include some delimiters in the default base implementation.
 *
 */

bool cCity::split_string(const char* source) {
	string temp (source);
	get_data_modifiable().push_back(this->add_string(temp));
	return true;
}

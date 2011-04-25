/*
 * DisambigEngine.h
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
 */

#ifndef DISAMBIGENGINE_H_
#define DISAMBIGENGINE_H_

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "DisambigCustomizedDefs.h"
#include "Threading.h"
using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

//asgdetail consists of assignee number and its patent counts.
typedef std::pair<string, unsigned int> asgdetail;

// forward diclarations
class cBlocking_Operation_By_Coauthors;


/*
 * definition of Record class.
 * The cRecord class is used to save a real line of record which includes multiple concrete attributes.
 *
 *
 * Private:
 * 		vector <const cAttribute *> vector_pdata: the major data of the class, which stores a vector of pointers of concrete attributes.
 * 		static vector <string> column_names: static member which stores the name of each attributes with sequential information. Very important.
 * 		static vector < string > active_similarity_names: static member which stores the names of attributes whose comparison functions are activated.
 * 		static const cRecord * sample_record_pointer: a pointer of a real record object, allowing some polymorphic static functions.
 *
 * Public:
 * 		cRecord(const vector <const cAttribute *>& input_vec): vector_pdata(input_vec): constructor.
 * 		cRecord(): default constructor.
 *		vector <unsigned int> record_compare(const cRecord & rhs) const: compare (*this) with rhs and get a similarity profile.
 *						NOTE THAT SIMILARITY PROFILES ARE vector <unsigned int>s;
 *			Example: if (Firstname, Lastname, Assignee, class, Coauthor) are activated for comparison, the function will return a
 *						vector < unsigned int > of 5-dimensions, each of which indicating a score for its corresponding column.
 *		vector <unsigned int> record_compare_by_attrib_indice (const cRecord &rhs, const vector < unsigned int > & attrib_indice_to_compare) const:
 *				compare (*this) with rhs only in the columns whose indice are given in "attrib_indice_to_compare", and returns an incomplete similarity profile.
 *			Example: if (Firstname, Lastname, Assignee, class, Coauthor) are activated, and the attrib_indice_to_compare = [ 0, 2, 3],
 *					 then the return value is a vector < unsigned int > = [ Firstname, Assignee, Class];
 *		static unsigned int get_index_by_name(const string & inputstr): get the index of the attribute whose specifier is "inputstr"
 *		const vector <const string * > & get_data_by_index(const unsigned int i) const: get the REAL data of the ith attribute through its index.
 *		const cAttribute * const & get_attrib_pointer_by_index(const unsigned int i) const:
 *				get the reference of the const pointer to a const attribute based on its index. Returning of reference is critical,
 *				because this allows modification of the attribute pointer in the vector.
 *		const vector < const cAttribute*> & get_attrib_vector () const: returns the vector of attribute pointers.
 *		void print( std::ostream & os ) const: output the record to an output stream, like std::cout or ofstream.
 *		void print() const: output the record to std::cout. Used mainly for debug purpose.
 *		static const vector < string > & get_column_names(): get the attribute names vector.
 *		unsigned int informative_attributes() const: count the number of informative attributes ( those containing "something" rather than empty strings ).
 *		unsigned int record_exact_compare(const cRecord & rhs ) const: compare (*this) with rhs and return the number of attributes that are exactly the same.
 *		void set_attrib_pointer_by_index( const cAttribute * pa, const unsigned int i ): modify the attribute vector, setting the ith element to pa
 *		void clean_member_attrib_pool() const: clear all the members static attribute pool. This is EXTREMELY dangerous, unless its functionality is fully understood.
 *		static unsigned int record_size(): returns the number of attributes of a record.
 *		static void update_active_similarity_names(): update the static member "active_similarity_names" by checking the comparator of each attribute.
 *		static const vector < string > & get_similarity_names(): get the names of the active similarity profile attributes.
 *		static unsigned int get_similarity_index_by_name(const string & inputstr): get the index of an attribute in the ACTIVE similarity profile.
 *
 */
class cRecord {
	friend bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns, const map<string, asgdetail>& asgtree);
	friend void clear_records(const list <cRecord> & source);
	friend class cSort_by_attrib;
	friend class cRatioComponent;
private:
	vector <const cAttribute *> vector_pdata;
	static vector <string> column_names;
	static vector < string > active_similarity_names;
	static const cRecord * sample_record_pointer;


public:
	cRecord(const vector <const cAttribute *>& input_vec): vector_pdata(input_vec) {};
	cRecord() {}
	vector <unsigned int> record_compare(const cRecord & rhs) const;
	vector <unsigned int> record_compare_by_attrib_indice (const cRecord &rhs, const vector < unsigned int > & attrib_indice_to_compare) const;
	static unsigned int get_index_by_name(const string & inputstr);
	const vector <const string * > & get_data_by_index(const unsigned int i) const {return vector_pdata.at(i)->get_data();};
	const cAttribute * const & get_attrib_pointer_by_index(const unsigned int i) const {return vector_pdata.at(i);} //return a reference is very important, because the content can be changed.
	const vector < const cAttribute*> & get_attrib_vector () const { return vector_pdata;}
	void print( std::ostream & os ) const;
	void print() const; //{ this->print(std::cout);}
	static const vector < string > & get_column_names() { return column_names;}
	unsigned int informative_attributes() const;
	unsigned int record_exact_compare(const cRecord & rhs ) const;
	void set_attrib_pointer_by_index( const cAttribute * pa, const unsigned int i ) { vector_pdata.at(i) = pa;}
	void clean_member_attrib_pool() const;
	static unsigned int record_size() { return column_names.size();}
	static void update_active_similarity_names();
	static const vector < string > & get_similarity_names() { return active_similarity_names;}
	static unsigned int get_similarity_index_by_name(const string & inputstr);
};

//================================================

/*
 * cSort_by_attrib:
 * A functor for comparison of attributes in associated containers such as map or set.
 * bool operator () (const cRecord * prec1, const cRecord *prec2 ) const: comparison between two records pointers based on their content.
 * 			The conparison content depends on the internal information of the cSort_by_attrib object. i.e. attrib_index;
 *
 * Example:
 * 1. cSort_by_attrib obj1( "Patent" ); this will create an instance obj1 which compares two cRecord pointers based on the "Patent" information column.
 *    map < const cRecord *, VALUE_TYPE, cSort_by_attrib > m1(obj1); will create a map (binary tree) whose key is const cRecord pointer, and
 *    		the way of sorting is through comparison of "Patent" attribute. Binary tree is ideal for fast search, insertion and deletion.
 *
 */

class cSort_by_attrib {
private:
	const unsigned int attrib_index;	//attrib_index is the column index on which the cRecord's comparison depends.
public:
	bool operator () (const cRecord * prec1, const cRecord *prec2 ) const {
		const cAttribute * attr1 = prec1->vector_pdata.at(attrib_index);
		const cAttribute * attr2 = prec2->vector_pdata.at(attrib_index);

		return attr1->get_data().at(0) < attr2->get_data().at(0);
	};
	cSort_by_attrib(const unsigned int i): attrib_index(i) {};
	cSort_by_attrib(const string & attrib_name): attrib_index(cRecord::get_index_by_name(attrib_name) ) {};
};



//=====================================================

/*
 * cString_Manipulator:
 * 	- cString_Remain_Same
 * 	- cString_Remove_Space
 * 	- cString_Truncate
 * 		-- cString_NoSpace_Truncate
 * 	- cExtract_Initials
 * 	- cString_Extract_FirstWord
 *
 * cString_Manipulator is a hierarchy of of string operation functors. The reason to create such hierarchy is to allow polymorphism.
 * Subclasses override the virtual function "string manipulate ( const string & input ) const " with their own implementations.
 *
 * To use, one should create an object of a subclass, and call the "manipulate" function.
 * To add more user defined string manipulators, one should simply create a class that inherits the cString_Manipulator class, and override the manipulate function.
 *
 */

class cString_Manipulator{
public:
	virtual string manipulate(const string & inputstring) const = 0;
	virtual ~cString_Manipulator() {};
};

/*
 * cString_Remain_Same:
 * As is indicated by the name, this class returns the raw string it obtains.
 *
 */

class cString_Remain_Same : public cString_Manipulator {
public:
	string manipulate(const string & inputstring ) const { return inputstring;}
};


/*
 * cString_Remove_Space:
 * This class remove all the white spaces of the input string and returns a cleaned one.
 * i.e. Input = "THIS IS AN   EXAMPLE  ". Return value = "THISISANEXAMPLE".
 *
 */

class cString_Remove_Space : public cString_Manipulator {
private:
	static const char delimiter = ' ';
public:
	string manipulate( const string & inputstring ) const {
		string result = inputstring;
		result.erase(std::remove_if (result.begin(), result.end(), ::isspace), result.end() );
		return result;
	};
	explicit cString_Remove_Space(){};
};

/*
 * cString_Truncate:
 * This class is more often used for string operation. Understanding of the member is desirable.
 *
 * Private:
 * int begin: location of the starting point for truncation. Non-negative value means the position of starting character is from the beginning,
 * 			  and negative value means the position of starting character is from the end of the string.
 * unsigned int nchar: number of characters to extract. if nchar == 0, all the string will be extracted. If nchar > length of string, only the string part will be kept.
 * bool is_forward: indicating the direction to truncation. true = forward ( left to right ), false = backward ( right to left )
 * bool is_usable: internal data of the class. false = the object is not prepared yet. true = ready to use.
 *
 *
 * WARNING: A COMBINATION OF NCHAR = 0 AND IS_FORWARD = FALSE WILL RETURN AN EMPTY STRING.
 *
 *
 * Public:
 * 	explicit cString_Truncate(): default constructor.
 *  void set_truncater( const int inputbegin, const unsigned int inputnchar, const bool inputforward): configure the object.
 *  string manipulate( const string & inputstring ) const: implementation of the extraction operation. Defined in cpp file.
 *
 * Example:
 *  cString_Truncate stobj; //created an instance
 *  stobj.set_truncater(0, 5, true) : starting position = 0 (head of the string), extraction length = 5, direction = forward.
 *  stobj.manipulate ("ERIC") returns "ERIC".
 *  stobj.manipulate ("JOHNSON") returns "JOHNS";
 *  stobj.set_truncater(0, 0, true) : starting position = 0 (head of the string), extraction length = full length, direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "JOHNSON";
 *  stobj.set_truncater(-6, 2, true) : starting position = -6, extraction length = 2, direction = forward.
 *  stobj.manipulate ("JOHNSON") returns "OH";
 *  stobj.set_truncater(-6, 2, false) : starting position = -6, extraction length = 2, direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "HO";
 *  stobj.set_truncater(4, 3, false) : starting position = 4, extraction length = 4, direction = backward.
 *  stobj.manipulate ("JOHNSON") returns "SNH";
 *  stobj.set_truncater(4, 0, false) : starting position = 4, extraction length = 0, direction = backward.  --- read the Warning part.
 *  stobj.manipulate ("JOHNSON") returns "";
 *
 */


class cString_Truncate: public cString_Manipulator {
private:
	int begin;
	unsigned int nchar;
	bool is_forward;
	bool is_usable;

	class cException_String_Truncation: public cAbstract_Exception {
	public:
		cException_String_Truncation (const char * errmsg) : cAbstract_Exception(errmsg) {};
	};

public:
	explicit cString_Truncate(): is_usable (false) {};
	void set_truncater( const int inputbegin, const unsigned int inputnchar, const bool inputforward) {
		begin = inputbegin;
		nchar = inputnchar;
		is_forward = inputforward;
		is_usable = true;
	}
	string manipulate( const string & inputstring ) const;
};

/*
 * cString_NoSpace_Truncate:
 * The functionality of the class is similar to the above cString_Truncate.
 * The only difference is that it removes space first and then truncates as directed.
 *
 */

class cString_NoSpace_Truncate: public cString_Truncate {
private:
	const cString_Remove_Space ns;
public:
	string manipulate ( const string & inputstring ) const {
		string temp = ns.manipulate(inputstring);
		return cString_Truncate::manipulate(temp);
	}
};


/*
 * cExtract_Initials:
 * Extract the initials from the string, starting from the (starting_word)th word.
 *
 * Example:
 * cExtract_Initials eiobj(3);	//create an instance eiobj, setting the starting_word = 3;
 * eiobj.manipulate("THIS IS AN EXAMPLE, YOU KNOW.") returns "EYK".
 *
 */

class cExtract_Initials : public cString_Manipulator {
private:
	const unsigned int starting_word;
	static const char delimiter = ' ';
public:
	string manipulate (const string & inputstring ) const;
	explicit cExtract_Initials (const unsigned int start ): starting_word(start) {};
};


/*
 * cString_Extract_FirstWord:
 * Extract the first word of the input string.
 *
 * cString_Extract_FirstWord sefobj;
 * sefobj.manipulate("THOMAS DAVID ANDERSON") returns "THOMAS"
 */

class cString_Extract_FirstWord : public cString_Manipulator {
private:
	static const char delimiter = ' ';
public:
	string manipulate (const string & inputstring ) const;
};

//==========================================================

/*
 * cBlocking_Operation
 * 	- cBlocking_Operation_Column_Manipulate
 * 	- cBlocking_Operation_Multiple_Column_Manipulate
 *
 * cBlocking_Operation reads data from a const cRecord pointer, and extract information to create a string which represents the blocking id to which the record belongs.
 *
 * Protected:
 * 		string infoless: this string represents the string that has delimiters ONLY. i.e. no useful information at all.
 * 						Usually, the variable is changed by concrete subclasses.
 *
 * Public:
 * 		const string & get_useless_string () const: returns the infoless string.
 * 		static const string delim: the separator between columns. Defined in cpp file. Will be used by other modules.
 * 		virtual string extract_blocking_info(const cRecord *) const = 0: pure virtual function to extract information from a cRecord pointer.
 * 		virtual string extract_column_info ( const cRecord *, unsigned int flag ) const:
 * 				virtual function that extract information from the (flag)th column.
 *  	virtual unsigned int num_involved_columns() const: return number of columns that the extraction operates on.
 *
 * Example:
 * 		cRecord recobj: firstname = "JACK", lastname = "SMITH", patent = "12345A"
 * 		if cBlocking_Operation::delim == "###", then
 * 			extract_blocking_info ( &recobj ) probably returns "J###SMI###" if the blocking operation
 * 											just wants the first char of firstname and 3 chars from lastname.
 * 			extract_column_info ( & recobj, 1 ) probably returns "SMI" if the information of the second column (which is lastname ) is desired.
 * 			num_involved_column() probably returns 2. ( firstname + lastname).
 * 			get_useless_string() returns "######".
 *
 */

class cBlocking_Operation {
protected:
	string infoless;
public:
	const string & get_useless_string () const { return infoless;}
	static const string delim;
	virtual string extract_blocking_info(const cRecord *) const = 0;
	virtual string extract_column_info ( const cRecord *, unsigned int flag ) const { throw cException_Other ("Function not defined yet.");}
	virtual ~cBlocking_Operation() {};
	virtual unsigned int num_involved_columns() const { return 1;}
};

/*
 * cBlocking_Operation_Column_Manipulate:
 * This is a subclass of cBlocking_Operation. Usage of the class is described in its base class.
 *
 * Example:
 * const cString_Truncate stobj (0, 3, true);
 * cBlocking_Operation_Column_Manipulate bocmobj ( stobj, cLastname::get_class_name() );
 * 		// set up a bocmobj instance of cBlocking_Operation_Column_Manipulate, which takes a cString_Truncate object as the string manipulater,
 * 		// and applies the string manipulation on the "Last name" column.
 * Assuming a record recobj: Firstname = "JOHN", lastname = "FLEMING", patent = "1234Q", Country = "CA", ... ...
 * bocmobj.extract_blocking_info( & recobj ) returns "FLE";
 *
 *
 * However, this concrete class is not used often, because the next class (cBlocking_Operation_Multiple_Column_Manipulate ) is more widely used.
 *
 */
class cBlocking_Operation_Column_Manipulate: public cBlocking_Operation {
private:
	const cString_Manipulator & sm;
	const unsigned int column_index;
public:
	explicit cBlocking_Operation_Column_Manipulate(const cString_Manipulator & inputsm, const string & colname)
			: sm(inputsm), column_index(cRecord::get_index_by_name(colname)) { infoless = delim;}
	string extract_blocking_info(const cRecord * p) const {return sm.manipulate( * p->get_data_by_index(column_index).at(0));}
};



/*
 * cBlocking_Operation_Multiple_Column_Manipulate:
 * This is a subclass of cBlocking_Operation, which extracts information from several columns and returns a block specifier string.
 *
 * Private:
 * 		vector < const cString_Manipulator * > vsm: the vector of string manipulator pointers.
 * 		vector < unsigned int > indice: the vector of column indice from which the manipulators extract information, respectively.
 * 		vector < const unsigned int * > pdata_indice: the vector of the const interger pointers indicating the positions of data in the data vector in cAttribute on which the extractions take place.
 *
 * Public:
 * 	cBlocking_Operation_Multiple_Column_Manipulate (const vector < const cString_Manipulator * > & inputvsm, const vector<string> & columnnames, const vector < unsigned int > & di )
 * 		: This is a constructor of the class. inputvsm = the vector of string manipulator pointers
 * 			columnnames = the vector of strings which reprensents the name of columns that the extractions will be applied on, respectively.
 * 			di = the vector of indice in the cAttribute::data, which will serve as the source string pointers.
 * 			NOTE: di SHOULD NOT BE DESTRUCTED BEFORE THE cBlocking_Operation_Multiple_Column_Manipulate object is discarded.
 * cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col ):
 * 			This is another constructor of the class object. pinputvsm = pointer of the string manipulator pointers array
 * 			pcolumnnames = pointer of the strings array. pdi = pointer of the cAttribute::data indice array. num_col = number of involed columns.
 * 			NOTE: pdi SHOULD NOT BE DESTRUCTED BEFORE THE cBlocking_Operation_Multiple_Column_Manipulate object is discarded.
 *	string extract_blocking_info(const cRecord * p) const: extracts information and returns a string.
 *	string extract_column_info ( const cRecord * p, unsigned int flag ) const: extracts information from specified column and returns a string
 *	unsigned int num_involved_columns() const : return number of involved columns.
 *
 *
 *
 */




class cBlocking_Operation_Multiple_Column_Manipulate : public cBlocking_Operation {
private:
	vector < const cString_Manipulator * > vsm;
	vector < unsigned int > indice;
	vector < const unsigned int * > pdata_indice;

public:
	cBlocking_Operation_Multiple_Column_Manipulate (const vector < const cString_Manipulator * > & inputvsm, const vector<string> & columnnames, const vector < unsigned int > & di )
		:vsm(inputvsm) {
		if ( inputvsm.size() != columnnames.size() )
			throw cException_Other("Critical Error in cBlocking_Operation_Multiple_Column_Manipulate: size of string manipulaters is different from size of columns");
		for ( unsigned int i = 0; i < columnnames.size(); ++i ) {
			indice.push_back(cRecord::get_index_by_name( columnnames.at(i)));
			infoless += delim;
			pdata_indice.push_back(& di.at(i));
		}
	}

	cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int  * pdi, const unsigned int num_col ) {
		for ( unsigned int i = 0; i < num_col; ++i ) {
			vsm.push_back(*pinputvsm++);
			indice.push_back(cRecord::get_index_by_name(*pcolumnnames++));
			infoless += delim;
			pdata_indice.push_back(pdi ++);
		}
	}

	string extract_blocking_info(const cRecord * p) const {
		string temp;
		for ( unsigned int i = 0; i < vsm.size(); ++i ) {
			temp += vsm[i]->manipulate(* p->get_data_by_index(indice[i]).at( * pdata_indice.at(i)));
			temp += delim;
		}
		return temp;
	};

	string extract_column_info ( const cRecord * p, unsigned int flag ) const {
		if ( flag >= indice.size() )
			throw cException_Other("Flag index error.");
		return vsm[flag]->manipulate( * p->get_data_by_index(indice[flag]).at( *pdata_indice.at(flag)) );
	}

	unsigned int num_involved_columns() const { return vsm.size();}

};

class cCluster_Info;
class cBlocking_Operation_By_Coauthors : public cBlocking_Operation {
private:
	map < const cRecord *, cGroup_Value, cSort_by_attrib > patent_tree;
	map < const cRecord *, const cRecord * > uid2uinv_tree;
	map < const cRecord *, unsigned int > uinv2count_tree;

	const unsigned int num_coauthors;

	void build_patent_tree(const list < const cRecord * > & allrec);
	cGroup_Value get_topN_coauthors( const cRecord *, const unsigned int topN) const;

public:
	cBlocking_Operation_By_Coauthors(const list < const cRecord * > & allrec, const cCluster_Info& cluster, const unsigned int coauthors);
	cBlocking_Operation_By_Coauthors(const list < const cRecord * > & allrec, const unsigned int num_coauthors);
	string extract_blocking_info(const cRecord *) const;
	void build_uid2uinv_tree( const cCluster_Info &);
	const map < const cRecord *, cGroup_Value, cSort_by_attrib > & get_patent_tree() const {return patent_tree;}
	map < const cRecord *, const cRecord * > & get_uid2uinv_tree() { return uid2uinv_tree;}

};



class cRecord_Reconfigurator {
public:
	virtual void reconfigure ( const cRecord * ) const = 0;
	void operator () ( cRecord & r ) const  {this->reconfigure(&r);}
	void operator () ( const cRecord * p) const { this->reconfigure(p); }
	virtual ~cRecord_Reconfigurator(){}
};

class cReconfigurator_AsianNames : public cRecord_Reconfigurator {
private:
	const unsigned int country_index;
	const unsigned int firstname_index;
	const unsigned int middlename_index;
	const unsigned int lastname_index;
	vector < string > east_asian;
	const cString_Remove_Space rmvsp;
public:
	cReconfigurator_AsianNames(): country_index(cRecord::get_index_by_name(cCountry::static_get_class_name())),
									firstname_index(cRecord::get_index_by_name(cFirstname::static_get_class_name())),
									middlename_index(cRecord::get_index_by_name(cMiddlename::static_get_class_name())),
									lastname_index(cRecord::get_index_by_name(cLastname::static_get_class_name())){
		east_asian.push_back(string("KR"));
		east_asian.push_back(string("CN"));
		east_asian.push_back(string("TW"));
	}
	void reconfigure( const cRecord * ) const;
};

class cReconfigurator_Latitude_Interactives: public cRecord_Reconfigurator {
private:
	//watch out the sequence! It is not important here but watch out in the reconfiugre function.
	const unsigned int latitude_index;
	const unsigned int longitude_index;
	const unsigned int street_index;
	const unsigned int country_index;

public:
	cReconfigurator_Latitude_Interactives(): latitude_index(cRecord::get_index_by_name(cLatitude::static_get_class_name())),
										longitude_index(cRecord::get_index_by_name(cLongitude::static_get_class_name())),
										street_index(cRecord::get_index_by_name(cStreet::static_get_class_name())),
										country_index(cRecord::get_index_by_name(cCountry::static_get_class_name())) {}
	void reconfigure( const cRecord * ) const;
};

class cReconfigurator_Coauthor : public cRecord_Reconfigurator {
private:
	const map < const cRecord *, cGroup_Value, cSort_by_attrib > * reference_pointer;
	const unsigned int coauthor_index;
public:
	cReconfigurator_Coauthor ( const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_authors) :
			reference_pointer ( & patent_authors), coauthor_index ( cRecord::get_index_by_name(cCoauthor::static_get_class_name())) {
		cCoauthor::clear_data_pool();
		cCoauthor::clear_attrib_pool();
	}
	void reconfigure ( const cRecord * ) const;
};






//=========================================

class cCluster_Info;
class cBlocking {
	friend class cCluster_Info;
private:
	cBlocking ( const cBlocking &);
protected:
	class cException_Blocking: public cAbstract_Exception {
	public:
		cException_Blocking (const char * errmsg) : cAbstract_Exception(errmsg) {};
	};
	
	class cString_Pointer_Compare{
	public:
		bool operator() ( const string * p1, const string *p2 ) const {
			return (*p1) < (*p2);
		}
	};
	
	class cException_Tree_Key_Mismatch : public cAbstract_Exception {
	public:
		cException_Tree_Key_Mismatch(const char* errmsg): cAbstract_Exception(errmsg) {};
	};

	map<string, cGroup_Value > blocking_data;
	map < const cRecord *, const string *> record2blockingstring;
	const vector <string> blocking_column_names;
	const vector<const cString_Manipulator*> string_manipulator_pointers;

public:
	explicit cBlocking(const list<const cRecord *> & psource, const vector<string> & blocking_column_names, const vector<const cString_Manipulator*>& pmanipulators, const string & unique_identifier );
	const map<string, cGroup_Value > & get_block_map() const {return blocking_data;}
};

class cBlocking_For_Disambiguation : public cBlocking {	
	friend class cWorker_For_Disambiguation;
private:
	map<const string *, double, cBlocking::cString_Pointer_Compare> prior_data;
	void config_prior();
public:
	class cGroup_Key {
	private:
		const cRecord * m_precord;
		unsigned int m_compare_sequence;
	public:
		bool operator < ( const cGroup_Key & rhs) const { return m_compare_sequence < rhs.m_compare_sequence; }
		const cRecord * get_precord() const {return m_precord;}
		unsigned int get_sequence() const { return m_compare_sequence;};
		void set_precord(const cRecord * p) {m_precord = p;}
		explicit cGroup_Key( const cRecord * p_record, unsigned int comp_seq ): m_precord(p_record), m_compare_sequence(comp_seq) {};
	};
	
	
	typedef map< cGroup_Key, cGroup_Value> cRecGroup;
	explicit cBlocking_For_Disambiguation (const list<const cRecord*> & psource, const vector<string> & blocking_column_names, const vector<const cString_Manipulator*>& pmanipulators, const string & unique_identifier );

	void disambiguate(cCluster_Info & match, cCluster_Info & nonmatch, const map < vector < unsigned int >, double, cSimilarity_Compare > & ratiosmap);

};






//===========================================================


class cRatios;

std::pair<const cRecord *, double> disambiguate_by_set (const cRecord * key1, const cGroup_Value & match1, const double cohesion1,
									 const cRecord * key2, const cGroup_Value & match2, const double cohesion2,
									 const double prior, 
									 const cRatios & ratio,  const double ) ;




void disambiguate_by_block ( cBlocking_For_Disambiguation::cRecGroup & to_be_disambiged_group, 
							cCluster_Info & match, cCluster_Info & nonmatch, const double prior_value,
							const map < vector < unsigned int >, double, cSimilarity_Compare > & ratiosmap, const string * const block_id );

void copyfile(const char * target, const char * source);



#endif /* DISAMBIGENGINE_H_ */

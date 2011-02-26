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
#include "DisambigDefs.h"
#include "Threading.h"
using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;
typedef std::pair<string, unsigned int> asgdetail;


class cAttribute;
class cBlocking_Operation_By_Coauthors;

class cRecord {
	friend bool fetch_records_from_sqlite3(list <cRecord> & source, const char * sql_file, const char * statement, const map<string, asgdetail>& asgtree);
	friend bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns, const map<string, asgdetail>& asgtree);
	friend void clear_records(const list <cRecord> & source);
	friend class cSort_by_attrib;
	friend class cRatioComponent;
private:
	vector <const cAttribute *> vector_pdata;
	static vector <string> column_names;
public:
	cRecord(const vector <const cAttribute *>& input_vec): vector_pdata(input_vec) {};
	vector <unsigned int> record_compare(const cRecord & rhs) const;
	vector <unsigned int> record_compare_by_attrib_indice (const cRecord &rhs, const vector < unsigned int > & attrib_indice_to_compare) const;
	static unsigned int get_index_by_name(const string & inputstr);
	const vector <const string * > & get_data_by_index(const unsigned int i) const {return vector_pdata.at(i)->get_data();};
	const cAttribute * get_attrib_pointer_by_index(const unsigned int i) const {return vector_pdata.at(i);}
	const vector < const cAttribute*> & get_attrib_vector () const { return vector_pdata;}
	void print( std::ostream & os ) const;
	void print() const; //{ this->print(std::cout);}
	static const vector < string > & get_column_names() { return column_names;}
	void reset_coauthors( const cBlocking_Operation_By_Coauthors &, const unsigned int topN) const;
	unsigned int informative_attributes() const;
	unsigned int exact_compare(const cRecord & rhs ) const;
	void set_attrib_pointer_by_index( const cAttribute * pa, const unsigned int i ) { vector_pdata.at(i) = pa;}
};

//================================================


class cSort_by_attrib {
private:
	const unsigned int attrib_index;
public:
	bool operator () (const cRecord * prec1, const cRecord *prec2 ) const {
		const cAttribute * attr1 = prec1->vector_pdata.at(attrib_index);
		const cAttribute * attr2 = prec2->vector_pdata.at(attrib_index);
		//if ( attr1->get_data().size() != 1 )
		//	throw cException_Invalid_Attribute_For_Sort(attr1->get_class_name().c_str());
		//if ( attr2->get_data().size() != 1 )
		//	throw cException_Invalid_Attribute_For_Sort(attr2->get_class_name().c_str());

		return attr1->get_data().at(0) < attr2->get_data().at(0);
	};
	cSort_by_attrib(const unsigned int i): attrib_index(i) {};
	cSort_by_attrib(const string & attrib_name): attrib_index(cRecord::get_index_by_name(attrib_name) ) {};
};



//=====================================================

class cString_Manipulator{
public:
	virtual string manipulate(const string & inputstring) const = 0;
	virtual ~cString_Manipulator() {};
};

class cString_Remain_Same : public cString_Manipulator {
public:
	string manipulate(const string & inputstring ) const { return inputstring;}
};

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

class cString_NoSpace_Truncate: public cString_Truncate {
private:
	const cString_Remove_Space ns;
public:
	string manipulate ( const string & inputstring ) const {
		string temp = ns.manipulate(inputstring);
		return cString_Truncate::manipulate(temp);
	}
};


class cExtract_Initials : public cString_Manipulator {
private:
	const unsigned int starting_word;
	static const char delimiter = ' ';
public:
	string manipulate (const string & inputstring ) const;
	explicit cExtract_Initials (const unsigned int start ): starting_word(start) {};
};

class cString_Extract_FirstWord : public cString_Manipulator {
private:
	static const char delimiter = ' ';
public:
	string manipulate (const string & inputstring ) const;
};

//==========================================================

class cBlocking_Operation {
protected:
	string infoless;
public:
	const string & get_useless_string () const { return infoless;}
	static const string delim;
	virtual string extract_blocking_info(const cRecord *) const = 0;
	virtual ~cBlocking_Operation() {};
};

#if 0
class cBlocking_Operation_FmLnName_NoSpace: public cBlocking_Operation {
private:
	cString_NoSpace_Truncate m_ftruncater;
	cString_NoSpace_Truncate n_ltruncater;
	const unsigned int firstname_index;
	const unsigned int lastname_index;
public:
	void set_firstname_truncater(const int begin, const unsigned int length, const bool is_forward) {m_ftruncater.set_truncater(begin, length, is_forward);}
	void set_lastname_truncater(const int begin, const unsigned int length, const bool is_forward) {n_ltruncater.set_truncater(begin, length, is_forward);}
	cBlocking_Operation_FmLnName_NoSpace(): firstname_index(cRecord::get_index_by_name(cFirstname::static_get_class_name())),
											lastname_index(cRecord::get_index_by_name(cLastname::static_get_class_name())) {}
	string extract_blocking_info(const cRecord *) const;
};
#endif

class cBlocking_Operation_Column_Manipulate: public cBlocking_Operation {
private:
	const cString_Manipulator & sm;
	const unsigned int column_index;
public:
	explicit cBlocking_Operation_Column_Manipulate(const cString_Manipulator & inputsm, const string & colname)
			: sm(inputsm), column_index(cRecord::get_index_by_name(colname)) { infoless = delim;}
	string extract_blocking_info(const cRecord * p) const {return sm.manipulate( * p->get_data_by_index(column_index).at(0));}
};

class cBlocking_Operation_Multiple_Column_Manipulate : public cBlocking_Operation {
private:
	vector < const cString_Manipulator * > vsm;
	vector < unsigned int > indice;
public:
	cBlocking_Operation_Multiple_Column_Manipulate (const vector < const cString_Manipulator * > & inputvsm, const vector<string> & columnnames )
		:vsm(inputvsm) {
		if ( inputvsm.size() != columnnames.size() )
			throw cException_Other("Critical Error in cBlocking_Operation_Multiple_Column_Manipulate: size of string manipulaters is different from size of columns");
		for ( vector<string>::const_iterator p = columnnames.begin(); p != columnnames.end(); ++p ) {
			indice.push_back(cRecord::get_index_by_name(*p));
			infoless += delim;
		}
	}

	cBlocking_Operation_Multiple_Column_Manipulate (const cString_Manipulator * const* pinputvsm, const string * pcolumnnames, const unsigned int num_col ) {
		for ( unsigned int i = 0; i < num_col; ++i ) {
			vsm.push_back(*pinputvsm++);
			indice.push_back(cRecord::get_index_by_name(*pcolumnnames++));
			infoless += delim;
		}
	}

	string extract_blocking_info(const cRecord * p) const {
		string temp;
		for ( unsigned int i = 0; i < vsm.size(); ++i ) {
			temp += vsm[i]->manipulate(* p->get_data_by_index(indice[i]).at(0));
			temp += delim;
		}
		return temp;
	};
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



class cCluster_Info;
class cBlocking_Operation_By_Coauthors : public cBlocking_Operation {
	friend void cRecord::reset_coauthors(const cBlocking_Operation_By_Coauthors & bb, const unsigned int topN) const;
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

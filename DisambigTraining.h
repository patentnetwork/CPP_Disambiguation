/*
 * DisambigTraining.h
 *
 *  Created on: Jan 6, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGTRAINING_H_
#define DISAMBIGTRAINING_H_

#include "DisambigDefs.h"
#include "DisambigEngine.h"
#include "Threading.h"

typedef std::pair< const cRecord *, const cRecord *> pointer_pairs;

struct cPrint_Pair {
private:
	vector < unsigned int > indice;
	unsigned int single_index;
	std::ostream & myos;

	const char * primary_delim;
	const char * secondary_delim;
	const bool is_vector;

public:
	cPrint_Pair(std::ostream & os, const vector < string > & vec_columnnames)
		: myos(os), primary_delim(","), secondary_delim( " | "), is_vector(true) {
		for ( vector<string>::const_iterator p = vec_columnnames.begin(); p != vec_columnnames.end() ; ++p )
			indice.push_back(cRecord::get_index_by_name(*p));
	}

	cPrint_Pair(std::ostream & os, const string & col_name)
			: myos(os), primary_delim(","), secondary_delim( " | "), is_vector(false) {
		single_index = cRecord::get_index_by_name(col_name);
	}

	void operator() (const pointer_pairs & source) {
		if ( is_vector) {
			for ( vector< unsigned int>::const_iterator p = indice.begin(); p != indice.end(); ++p )
				myos << * source.first->get_data_by_index(*p).at(0) << secondary_delim;
			myos << primary_delim;
			for ( vector< unsigned int>::const_iterator p = indice.begin(); p != indice.end(); ++p )
				myos << * source.second->get_data_by_index(*p).at(0) << secondary_delim;
			myos << '\n';
		}
		else {
			myos << * source.first->get_data_by_index(single_index).at(0) << primary_delim
					<< * source.second->get_data_by_index(single_index).at(0)<<'\n';
		}
	}
};


class cException_Reach_Limit: public cAbstract_Exception {
public:
	cException_Reach_Limit(const char* errmsg): cAbstract_Exception(errmsg) {};
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



class cBlocking_For_Training : public cBlocking {

private:
	map<const string *, unsigned int, cBlocking::cString_Pointer_Compare> quota_map;
	map<const string *, unsigned int, cBlocking::cString_Pointer_Compare> used_quota_map;
	map<const string *, cGroup_Value::const_iterator, cBlocking::cString_Pointer_Compare> outer_cursor_map;
	map<const string *, cGroup_Value::const_iterator, cBlocking::cString_Pointer_Compare> inner_cursor_map;
	const unsigned int total_quota;
	unsigned int quota_left;
	bool was_used;

	list <pointer_pairs> chosen_pairs;

	bool move_cursor( cGroup_Value:: const_iterator & outer, cGroup_Value:: const_iterator & inner, const cGroup_Value & datarange);
	bool cursor_ok( const cGroup_Value:: const_iterator & outer, const cGroup_Value:: const_iterator & inner, const cGroup_Value & datarange ) const {
		return  ( outer != datarange.end() && inner != datarange.end() &&  inner != outer );
	}
public:
	typedef unsigned int(cBlocking_For_Training::*pFunc)(const string & block_id, const vector <unsigned int> & equal_indice, const vector<const cString_Manipulator*>& pmanipulators_equal,
															const vector <unsigned int> &nonequal_indice, const vector<const cString_Manipulator*>& pmanipulators_nonequal, const bool is_firstround);

	explicit cBlocking_For_Training( const list < const cRecord *> & source, const vector<string> & blocking_column_names,
									const vector<const cString_Manipulator*>& pmanipulators, const string & unique_identifier, const unsigned int qt);
	unsigned int create_xset01_on_block(const string & block_id, const vector <unsigned int> & equal_indice, const vector<const cString_Manipulator*>& pmanipulators_equal,
										const vector <unsigned int> &nonequal_indice, const vector<const cString_Manipulator*>& pmanipulators_nonequal, const bool is_firstround);
	unsigned int create_tset05_on_block(const string & block_id, const vector <unsigned int> & equal_indice, const vector<const cString_Manipulator*>& pmanipulators_equal,
											const vector <unsigned int> &nonequal_indice, const vector<const cString_Manipulator*>& pmanipulators_nonequal, const bool is_firstround);
	unsigned int create_xset03_on_block(const string & block_id, const vector <unsigned int> & equal_indice, const vector<const cString_Manipulator*>& pmanipulators_equal,
											const vector <unsigned int> &nonequal_indice, const vector<const cString_Manipulator*>& pmanipulators_nonequal, const bool is_firstround);
	unsigned int create_set(pFunc mf, const vector <string> & equal_indice_names, const vector<const cString_Manipulator*>& pmanipulators_equal,
									const vector <string> & nonequal_indice_names, const vector<const cString_Manipulator*>& pmanipulators_nonequal );
	void print (std::ostream & os, const string & unique_record_id_name ) const;
	void reset(const unsigned int num_cols);

};

class cWorker_For_Training : public Thread {
private:
  map<string, cGroup_Value> ::iterator *piter;
  cBlocking_For_Training::pFunc func;
  const vector < string > & m_equal_indice_names;
  const vector < const cString_Manipulator * > & m_pstringcontrol_equal;
  const vector < string > & m_non_equal_indice_names;
  const vector < const cString_Manipulator * > & m_pstringcontrol_nonequal;

  static pthread_mutex_t iter_mutex;
 public:
  explicit cWorker_For_Training ( map < string, cGroup_Value>::iterator *inputiter, const cBlocking_For_Training::pFunc inputfun,
				  const vector < string > & equal_indice_names, const vector < const cString_Manipulator * > & pmanipulators_equal, 
				  const vector < string > & nonequal_indice_names, const vector < const cString_Manipulator * > & pmanipulators_nonequal )
    : piter( inputiter ), func(inputfun), m_equal_indice_names(equal_indice_names), m_pstringcontrol_equal(pmanipulators_equal),
    m_non_equal_indice_names( nonequal_indice_names ), m_pstringcontrol_nonequal(pmanipulators_nonequal) {};
  ~cWorker_For_Training() {};
  void run();


};




void find_rare_names_v2(const vector < cGroup_Value * > &vec_pdest, const list< const cRecord* > & source );
unsigned int create_tset02(list <pointer_pairs> &results, 	const list <const cRecord*> & reclist,
							const vector <string> & column_names, const vector < const cGroup_Value * > & vec_prare_names, const unsigned int limit );
unsigned int create_xset03(list <pointer_pairs> &results, 	const list <const cRecord*> & reclist,
							const vector < const cGroup_Value * > & vec_prare_names, const unsigned int limit );
unsigned int create_xset01(list <pointer_pairs> &results, const list <const cRecord *> & source,  const unsigned int limit );

#endif /* DISAMBIGTRAINING_H_ */

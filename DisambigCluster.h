/*
 * DisambigCluster.h
 *
 *  Created on: Jan 12, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGCLUSTER_H_
#define DISAMBIGCLUSTER_H_

#include <iostream>
#include <map>
#include <set>
#include "DisambigDefs.h"
#include <fstream>
#include <pthread.h>
#include "Threading.h"
#include "DisambigNewCluster.h"

using std::string;
using std::set;
using std::map;

class cRecord;
class cBlocking;
class cBlocking_Operation;
class cRatios;



class cCluster_Info {
	friend class cBlocking_For_Disambiguation;
	friend class cWorker_For_Disambiguation;
	typedef set<const cRecord *> recordset;

	/*
	class cGroup_Key {
	private:
		const cRecord * m_precord;
		unsigned int m_compare_sequence;
	public:
		bool operator < ( const cGroup_Key & rhs) const { return m_compare_sequence < rhs.m_compare_sequence; }
		const cRecord * get_precord() const {return m_precord;}
		unsigned int get_sequence() const { return m_compare_sequence;};
		void set_precord(const cRecord * p) {m_precord = p;}
		cGroup_Key( const cRecord * p_record, unsigned int comp_seq ): m_precord(p_record), m_compare_sequence(comp_seq) {};
	};
	*/

	typedef const cRecord * cGroup_Key;
	//typedef map< cGroup_Key, cGroup_Value cRecGroup;
	//typedef list<const cRecord*> cGroup_Value;
	//typedef map < cGroup_Key, cGroup_Value > cRecGroup;
	//typedef list < std::pair< cGroup_Key, cGroup_Value > > cRecGroup;
	typedef list < cCluster > cRecGroup;

	friend bool disambiguate_wrapper(const map<string, cCluster_Info::cRecGroup>::iterator & p,cCluster_Info & cluster,
								const cRatios & ratiosmap );

private:
	//map < const cRecord*, set <const cRecord*> > past_comparision;
	//map < const cRecord*, double > cohesion_map;
	//const char * const past_comparision_file;
	//const string unique_id_name;
	const map <string, const cRecord*> * const uid2record_pointer;
	const bool is_matching;
	unsigned int total_num;

	map < string, cRecGroup > cluster_by_block;
	//map < const string *, map < const cRecord *, double > > cohesion_map_by_block;
	//map < string, unsigned int > firstname_stat;
	//map < string, unsigned int > lastname_stat;
	//map < unsigned int, unsigned int > firstname_dist;
	//map < unsigned int, unsigned int > lastname_dist;
	vector < map < string, unsigned int > > column_stat;
	map < const string *, double > prior_data;
	map < const string *, bool > debug_activity;
	vector < unsigned int > max_occurrence;

	string useless;
	const bool frequency_adjust_mode;
	const bool debug_mode;


	class cException_Cluster_Error: public cAbstract_Exception {
	public:
		cException_Cluster_Error(const char* errmsg): cAbstract_Exception(errmsg){};
	};

	void config_prior();

	unsigned int disambiguate_by_block ( cRecGroup & to_be_disambiged_group,  double & prior_value,
											const cRatios & ratiosmap, const string * const bid ) ;
	void retrieve_last_comparision_info ( const cBlocking_Operation & blocker, const char * const past_comparision_file);
public:

	static const char * primary_delim;
	static const char * secondary_delim;

	void output_current_comparision_info(const char * const outputfile ) const;
	void output_list ( list<const cRecord *> & target) const;
	void print(std::ostream & os) const;

	unsigned int reset_debug_activity( const char * filename );
	void reset_blocking(const cBlocking_Operation & blocker, const char * const past_comparision_file);
	void preliminary_consolidation(const cBlocking_Operation & blocker, const list < const cRecord *> & all_rec_list);
	cCluster_Info(const map <string, const cRecord*> & input_uid2record, const bool input_is_matching , const bool aum = false, const bool debug = false )
			: uid2record_pointer(&input_uid2record), is_matching(input_is_matching), frequency_adjust_mode(aum), debug_mode(debug) {
		std::cout << "A cluster information class is set up." << std::endl;
		std::cout << "FREQUENCY_ADJUST_PRIOR_MODE: " << (frequency_adjust_mode ? "ON" : "OFF")
					<< "       DEBUG MODE: " << (debug_mode ? "ON" : "OFF") << std::endl;
	} ;
	void disambiguate(const cRatios & ratiosmap, const unsigned int num_threads, const char * const debug_block_file);
	void disambig_assignee(const char * outputfile) const;
	const string & get_useless_string() const {return useless;}
	~cCluster_Info() {}

	double get_prior_value( const string & block_identifier, const list <cCluster> & rg );
	const map < string, cRecGroup> & get_cluster_map () const {return cluster_by_block;}
	const map < const string *, double > & get_prior_map() const {return prior_data;}
	map < const string*, double > & get_prior_map() {return prior_data;};
	const cRecGroup & get_comparision_map(const string* bid) const {
		map < string, cRecGroup >::const_iterator q = cluster_by_block.find(*bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		return q->second;
	}
	/*
	const map < const cRecord *, double > & get_cohesion_map(const string * bid) const {
		map < const string*, map < const cRecord*, double> >::const_iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		return q->second;
	}
	*/

	cRecGroup & get_comparision_map(const string* bid) {
		map < string, cRecGroup >::iterator q = cluster_by_block.find(*bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		return q->second;
	}
	/*
	map < const cRecord *, double > & get_cohesion_map(const string * bid) {
		map < const string*, map < const cRecord*, double> >::iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		return q->second;
	}
	*/
#if 0
	//const map < const cRecord *, double > & get_cohesion_map() const { return cohesion_map;};
/*
	map < const cRecord*, set <const cRecord*> >::iterator find_comparision(const cRecord * target, const string * bid) {
		map < const string *, rep_map >::iterator q = cluster_by_block.find(bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, set <const cRecord*> >::iterator p = q->second.find(target);
		if ( p == q->second.end())
			throw cException_Attribute_Not_In_Tree("Record pointer not in tree");
		return p;
	}
	map < const cRecord*, set <const cRecord*> >::const_iterator find_comparision(const cRecord * target, const string * bid) const {
		map < const string *, rep_map >::const_iterator q = cluster_by_block.find(bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, set <const cRecord*> >::const_iterator p = q->second.find(target);
		if ( p == q->second.end())
			throw cException_Attribute_Not_In_Tree("Record pointer not in tree");
		return p;
	}

	map < const cRecord*, double >::iterator find_cohesion(const cRecord * target, const string* bid) {
		map < const string*, map < const cRecord*, double> >::iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, double >::iterator p = q->second.find(target);
		if ( p == q->second.end())
			throw cException_Attribute_Not_In_Tree("Record pointer not in tree");
		return p;
	}
	map < const cRecord*, double >::const_iterator find_cohesion(const cRecord * target, const string * bid) const {
		map < const string*, map < const cRecord*, double> >::const_iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, double >::const_iterator p = q->second.find(target);
		if ( p == q->second.end())
			throw cException_Attribute_Not_In_Tree("Record pointer not in tree");
		return p;
	}

	void comparision_insert(const cRecord* key, const set< const cRecord * > & set_to_add, const string* bid) {
		map < const string *, rep_map >::iterator q = cluster_by_block.find(bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, set<const cRecord *> > & alias = q->second;
		map < const cRecord*, set <const cRecord*> >::iterator p = alias.find(key);
		if ( p == alias.end())
			alias.insert(std::pair< const cRecord *, cGroup_Value(key, set_to_add) );
			//p = past_comparision.insert(std::pair< const cRecord *, cGroup_Value(key, empty_set) ).first;
		else
			p->second.insert(set_to_add.begin(), set_to_add.end() );
	}

	void comparision_insert(const cRecord* key, const cRecord* one_more_value, const string* bid);

	void cohesion_update( const cRecord * key, const double cohesion_value, const string * bid ) {
		map < const string *, map<const cRecord*, double> >::iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		map < const cRecord*, double > & alias = q->second;
		map < const cRecord*, double >::iterator p = alias.find(key);
		if ( p == alias.end() )
			alias.insert(std::pair<const cRecord*, double>(key, cohesion_value));
		else
			p->second = cohesion_value;
	}

	void comparision_erase(const cRecord * key, const string * bid) {
		map < const string *, rep_map >::iterator q = cluster_by_block.find(bid);
		if ( q == cluster_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		q->second.erase(key);
	}
	void cohesion_erase(const cRecord * key, const string * bid) {
		map < const string *, map < const cRecord*, double> >::iterator q = cohesion_map_by_block.find(bid);
		if ( q == cohesion_map_by_block.end())
			throw cException_Attribute_Not_In_Tree(bid->c_str());
		q->second.erase(key);
	}

	void update_comparision( const cRecord* key_to_add, const set< const cRecord * > & set_to_add, const cRecord * key_to_delete, const double cohesion_to_update, const string * bid);
*/
#endif

	unsigned int current_size() const;
	const bool is_matching_cluster() const {return is_matching;};
	bool is_consistent() const;
};



class cWorker_For_Disambiguation : public Thread {
private:
	map < string, cCluster_Info::cRecGroup >::iterator * ppdisambiged;
	const cRatios * pratios;
	cCluster_Info & cluster_ref;

	static pthread_mutex_t iter_lock;
	static unsigned int count;
public:
	explicit cWorker_For_Disambiguation( map < string, cCluster_Info::cRecGroup >::iterator & input_pdisambiged,
			const cRatios & ratiosmap,
			cCluster_Info & inputcluster
	) : ppdisambiged(&input_pdisambiged), pratios(&ratiosmap), cluster_ref(inputcluster) {}

	~cWorker_For_Disambiguation() {}
	static void zero_count() { count = 0; }
	static unsigned int get_count() { return count;}

	void run();
};


#endif /* DISAMBIGCLUSTER_H_ */

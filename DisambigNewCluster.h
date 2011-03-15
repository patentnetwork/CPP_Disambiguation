/*
 * DisambigNewCluster.h
 *
 *  Created on: Feb 7, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGNEWCLUSTER_H_
#define DISAMBIGNEWCLUSTER_H_

#include "DisambigEngine.h"

class cCluster_Head {
public:
	const cRecord * m_delegate;
	double m_cohesion;
	cCluster_Head(const cRecord * const p, const double c): m_delegate(p), m_cohesion(c) {};
	cCluster_Head ( const cCluster_Head & rhs): m_delegate(rhs.m_delegate), m_cohesion(rhs.m_cohesion) {}
};

class cCluster {
private:
	cCluster_Head m_info;
	cGroup_Value m_fellows;
	bool m_mergeable;
	//set < const string* > coauthor_list;
	bool m_usable;

	static unsigned int coauthor_index;
	static const cRatios * pratio;
	static const map < const cRecord *, cGroup_Value, cSort_by_attrib > * reference_pointer;
	static const cString_Extract_FirstWord firstname_extracter;
	static const cString_Remove_Space lastname_extracter;
	cCluster & operator = ( const cCluster &);
public:
	cCluster(const cCluster_Head & info, const cGroup_Value & fellows);
	~cCluster();
	cCluster ( const cCluster & rhs );
	void merge( cCluster & mergee, const cCluster_Head & info);
	vector < unsigned int > compare( const cCluster & rhs) const;
	unsigned int reset_coauthor_list(const map < const cRecord *, cGroup_Value, cSort_by_attrib > & reference_patent_tree) { return 0;};
	static void set_coauthor_index( const unsigned int idx) { coauthor_index = idx; }
	void reset_cCoauthor_pointer(const set< const string * > & coauthor_set) {};
	cCluster_Head disambiguate(const cCluster & rhs, const double prior, const double mutual_threshold) const;
	static void set_ratiomap_pointer( const cRatios & r) {pratio = &r;}
	const cGroup_Value & get_fellows() const {return m_fellows;}
	const cCluster_Head & get_cluster_head () const {return m_info;};
	const set < const string * > & get_coauthor_set() const {return * (m_info.m_delegate->get_attrib_pointer_by_index(coauthor_index)->get_attrib_set_pointer());}
	void insert_elem( const cRecord *);
	void reset_clusterhead(const double prior);
	void split ( const cBlocking_Operation & blocker, list<cCluster >& clusterlist ) const;
	unsigned int connect_by_coauthors( const cCluster & rhs )const;
	void self_repair();
	static void set_reference_patent_tree_pointer(const map < const cRecord *, cGroup_Value, cSort_by_attrib > & reference_patent_tree ) { reference_pointer = & reference_patent_tree;}
};

class cException_Empty_Cluster : public cAbstract_Exception {
public:
	cException_Empty_Cluster(const char* errmsg): cAbstract_Exception(errmsg) {};
};













#endif /* DISAMBIGNEWCLUSTER_H_ */

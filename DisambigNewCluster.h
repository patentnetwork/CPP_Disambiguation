/*
 * DisambigNewCluster.h
 *
 *  Created on: Feb 7, 2011
 *      Author: ysun
 *
 *
 *  This file contains definitions of cCluster, or a group of unique record id that actually ( not necessarily true ) belong
 *  to a unique inventor.
 *
 */

#ifndef DISAMBIGNEWCLUSTER_H_
#define DISAMBIGNEWCLUSTER_H_

#include "DisambigEngine.h"

/*
 * cCluster_Head:
 * This class contains two pieces of information about a cluster: its delegate and its cohesion.
 * Public:
 * 		const cRecord * m_delegate: the delegate (representative) of a cluster. Usually this pointer contains the most frequently occurring information.
 * 		double m_cohesion: the cohesion of a cluster, meaning the probability for the members of the cluster to be of the same inventor.
 *
 * 		cCluster_Head(const cRecord * const p, const double c): constructor
 * 		cCluster_Head ( const cCluster_Head & rhs): copy constructor
 *
 */

class cCluster_Head {
public:
	const cRecord * m_delegate;
	double m_cohesion;
	cCluster_Head(const cRecord * const p, const double c): m_delegate(p), m_cohesion(c) {};
	cCluster_Head ( const cCluster_Head & rhs): m_delegate(rhs.m_delegate), m_cohesion(rhs.m_cohesion) {}
};

/*
 * cCluster:
 * This class objects are the molecules of disambiguation, while cRecord objects are atoms of disambiguation.
 * Each cluster contains a cluster_head, a list of members, and some other
 * information. The aim of disambiguation is reorganize clusters so that some can probably compound to bigger ones. Disambiguation
 * starts from smallest clusters that contain only one record, and ends with clusters that contain some amount of records.
 *
 * Private:
 *		cCluster_Head m_info: cluster head of the cluster, including the delegate and the cohesion of the cluster.
 *		cGroup_Value m_fellows: the list of members of the cluster.
 *		bool m_mergeable: a boolean, indicating "*this" cluster has been merged into others or not.
 *		bool m_usable: a boolean preventing misuse earlier than fully prepared.
 *
 *		static const cRatios * pratio: a pointer that points to a cRatio object which contains a map of similarity profile to ratio
 *		static const map < const cRecord *, cGroup_Value, cSort_by_attrib > * reference_pointer:
 *					a pointer that points to a patent tree, which can be obtained in a cBlocking_Operation_By_Coauthor object.
 *		cCluster & operator = ( const cCluster &): forbid the assignment operation.
 *		void find_representative():  to sets a component of cluster head to be the record whose columns appear most frequently among the all the members.
 *
 * Public:
 *		cCluster(const cCluster_Head & info, const cGroup_Value & fellows): constructor
 *		~cCluster() : destructor
 *		cCluster ( const cCluster & rhs ): copy constructor
 *		void merge( cCluster & mergee, const cCluster_Head & info): merge the "mergee" cluster into "*this", and set
 *				the cluster head of the new cluster to be info.
 *		cCluster_Head disambiguate(const cCluster & rhs, const double prior, const double mutual_threshold) const:
 *				dsiambiguate "*this" cluster with rhs cluster, with the prior and mutual_threshold information.
 *				Returns a cCluster_Head to tell whether the two clusters should be merged or not, and if yes, the cohesion of the new one.
 *		static void set_ratiomap_pointer( const cRatios & r): set the ratio map pointer to a good one.
 *		const cGroup_Value & get_fellows() const: get the members ( actually it is reference to const )of the cluster.
 *		const cCluster_Head & get_cluster_head () const: get the cluster head ( const reference ) of the cluster.
 *		void insert_elem( const cRecord *): insert a new member into the member list. This could potentially change the cluster head.
 *		void self_repair(): call this if insertion of elements is done manually, usually for a batch of record objects (not recommended).
 *		static void set_reference_patent_tree_pointer(const map < const cRecord *, cGroup_Value, cSort_by_attrib > & reference_patent_tree ): set the patent tree pointer.
 *		void change_mid_name(): to change pointers to abbreviated middle names to full middle names.
 *					This step is controversial, as it actually changed the raw data. Or more correctly, it changed the pointers of the raw data.
 *
 */

class cCluster {
private:
	static const unsigned int invalid_year = 0;
	cCluster_Head m_info;
	cGroup_Value m_fellows;
	bool m_mergeable;
	bool m_usable;

	static const cRatios * pratio;
	static const map < const cRecord *, cGroup_Value, cSort_by_attrib > * reference_pointer;

	cCluster & operator = ( const cCluster &);
	void find_representative();


	unsigned int first_patent_year;
	unsigned int last_patent_year;
	set < const cLatitude * > locs;

	void update_locations();
	void update_year_range();
	unsigned int patents_gap( const cCluster & rhs) const;
	bool is_valid_year() const;
public:
	cCluster(const cCluster_Head & info, const cGroup_Value & fellows);
	~cCluster();
	cCluster ( const cCluster & rhs );
	void merge( cCluster & mergee, const cCluster_Head & info);
	cCluster_Head disambiguate(const cCluster & rhs, const double prior, const double mutual_threshold) const;
	static void set_ratiomap_pointer( const cRatios & r) {pratio = &r;}
	const cGroup_Value & get_fellows() const {return m_fellows;}
	const cCluster_Head & get_cluster_head () const {return m_info;};
	void insert_elem( const cRecord *);

	void self_repair();
	static void set_reference_patent_tree_pointer(const map < const cRecord *, cGroup_Value, cSort_by_attrib > & reference_patent_tree ) { reference_pointer = & reference_patent_tree;}

	void change_mid_name();
	void add_uid2uinv( map < const cRecord *, const cRecord *> & uid2uinv ) const;
};

/*
 * cException_Empty_Cluster: an exception that may be used.
 */
class cException_Empty_Cluster : public cAbstract_Exception {
public:
	cException_Empty_Cluster(const char* errmsg): cAbstract_Exception(errmsg) {};
};













#endif /* DISAMBIGNEWCLUSTER_H_ */

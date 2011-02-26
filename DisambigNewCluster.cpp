/*
 * DisambigNewCluster.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: ysun
 */

#include "DisambigNewCluster.h"
#include <limits>

unsigned int cCluster::coauthor_index = std::numeric_limits<unsigned int >::max() ;
const cRatios * cCluster::pratio = NULL;
const map < const cRecord *, cGroup_Value, cSort_by_attrib > * cCluster::reference_pointer = NULL;
const cString_Extract_FirstWord cCluster::firstname_extracter;
const cString_Remove_Space cCluster::lastname_extracter;

cCluster::cCluster(const cCluster_Head & info, const cGroup_Value & fellows)
		: m_info(info), m_fellows(fellows), m_mergeable(true), m_usable(true) {
	if ( coauthor_index == std::numeric_limits<unsigned int >::max() )
		throw cException_Other("Coauthor index is not set properly.");
	if ( NULL == reference_pointer )
		throw cException_Other("Critical Error: reference pointer is not set yet.");
	reset_coauthor_list(*reference_pointer);
	reset_cCoauthor_pointer(coauthor_list);
}


void cCluster::merge( cCluster & mergee, const cCluster_Head & info ) {
	if ( this->m_mergeable == false )
		throw cException_Empty_Cluster("Merging error: merger is empty.");
	if ( mergee.m_mergeable == false )
		throw cException_Empty_Cluster("Merging error: mergEE is empty.");

	this->m_info = info;
	this->m_fellows.insert(m_fellows.end(), mergee.m_fellows.begin(), mergee.m_fellows.end());
	this->coauthor_list.insert(mergee.coauthor_list.begin(), mergee.coauthor_list.end());
	mergee.reset_cCoauthor_pointer(this->coauthor_list);
	//mergee.m_fellows.clear();
	mergee.coauthor_list.clear();
	mergee.m_mergeable = false;
}

cCluster::~cCluster() {
	/*
	if ( m_mergeable )
		std::cout << "---------------ERROR: Cluster is not ready to be deleted yet.-----------" << std::endl;
	for (cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cCoauthor * ct = dynamic_cast< const cCoauthor * > ((*p)->get_attrib_pointer_by_index(cCluster::coauthor_index));
		if ( NULL == ct )
			throw cException_Other("Critical Error: cannot convert pointer to cCoauthor *.");
		if ( ct->get_set_pointer() == & this->coauthor_list )
			std::cout << "---------------ERROR: Pointers in the Cluster will point to invalid addresses.-----------" << std::endl;
	}
	*/
}

cCluster::cCluster( const cCluster & rhs ) : m_info(rhs.m_info), m_fellows(rhs.m_fellows), m_mergeable(true), coauthor_list(rhs.coauthor_list){
	if ( rhs.m_mergeable == false )
		throw cException_Other("cCluster Copy Constructor error.");
	//rhs.m_mergeable = false;
	this->reset_cCoauthor_pointer(this->coauthor_list);
}


cCluster_Head cCluster::disambiguate( const cCluster & rhs, const double prior, const double mutual_threshold) const {
	if ( pratio == NULL)
		throw cException_Other("Critical: ratios map is not set yet.");
	if ( this->m_mergeable == false ) {
		throw cException_Empty_Cluster("Comparison error: lhs is empty.");
	}
	if ( rhs.m_mergeable == false ) {
		throw cException_Empty_Cluster("Comparison error: rhs is empty.");
	}

	std::pair<const cRecord *, double > ans ( disambiguate_by_set ( this->m_info.m_delegate, this->m_fellows, this->m_info.m_cohesion,
												rhs.m_info.m_delegate, rhs.m_fellows, rhs.m_info.m_cohesion,
													prior, *pratio, mutual_threshold) );

	return cCluster_Head(ans.first, ans.second);

}

unsigned int cCluster::reset_coauthor_list(const map < const cRecord *, cGroup_Value, cSort_by_attrib > & reference_patent_tree) {
	this->coauthor_list.clear();
	static const string dot = ".";
	const unsigned int firstnameindex = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int lastnameindex = cRecord::get_index_by_name(cLastname::static_get_class_name());
	map < const cRecord *, cGroup_Value, cSort_by_attrib >::const_iterator cpm;
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		cpm = reference_patent_tree.find(*p);
		if ( cpm == reference_patent_tree.end())
			throw cException_Other("Missing patent data.");
		const cGroup_Value & patent_coauthors = cpm->second;
		for ( cGroup_Value::const_iterator q = patent_coauthors.begin(); q != patent_coauthors.end(); ++q ) {
			cGroup_Value::const_iterator ti = find( m_fellows.begin(), m_fellows.end(), *q );
			if ( ti != m_fellows.end())
				continue;
			string fullname = firstname_extracter.manipulate( * (*q)->get_data_by_index(firstnameindex).at(0) ) + dot
								+ lastname_extracter.manipulate( * (*q)->get_data_by_index(lastnameindex).at(0) );
			this->coauthor_list.insert(cCoauthor::static_add_string (fullname) );
		}
	}
	return coauthor_list.size();
}

void cCluster::reset_cCoauthor_pointer(const set< const string *> & s) {
	for (cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cCoauthor * ct = dynamic_cast< const cCoauthor * > ((*p)->get_attrib_pointer_by_index(cCluster::coauthor_index));
		if ( NULL == ct )
			throw cException_Other("Critical Error: cannot convert pointer to cCoauthor *.");
		cCoauthor * c = const_cast<cCoauthor*> (ct);
		c->reset_pointer(s);
	}
}


void cCluster::split ( const cBlocking_Operation & blocker, list<cCluster >& clusterlist ) const {
	map < string, cGroup_Value > m;
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		string label = blocker.extract_blocking_info(*p);
		map < string, cGroup_Value>::iterator q = m.find(label);
		if ( q == m.end() ) {
			m.insert(std::pair<string, cGroup_Value>(label, cGroup_Value(1, *p)));
		}
		else
			q->second.push_back(*p);
	}
	for ( map<string, cGroup_Value>::const_iterator cq = m.begin(); cq != m.end(); ++ cq) {
		const cGroup_Value & alias = cq->second;
		cCluster_Head th ( alias.front(), 1 );
		cCluster tc( th, alias);
		tc.reset_clusterhead(0.5);
		clusterlist.push_back(tc);
	}
}

void cCluster::reset_clusterhead(const double prior) {
	cCluster_Head th( NULL, 1);
	cGroup_Value tv;
	cCluster tc(th, tv);
	cCluster_Head newhead = this->disambiguate(tc, prior, 0.95);
	this->m_info = newhead;
	m_usable = true;
}

void cCluster::insert_elem( const cRecord * more_elem) {
	this->m_fellows.push_back(more_elem);
	m_usable = false;
}

void cCluster::self_repair() {
	reset_coauthor_list(*reference_pointer);
	reset_cCoauthor_pointer(coauthor_list);
	m_usable = true;
}


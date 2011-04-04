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
	//reset_coauthor_list(*reference_pointer);
	//reset_cCoauthor_pointer(coauthor_list);
}


void cCluster::merge( cCluster & mergee, const cCluster_Head & info ) {
	if ( this->m_mergeable == false )
		throw cException_Empty_Cluster("Merging error: merger is empty.");
	if ( mergee.m_mergeable == false )
		throw cException_Empty_Cluster("Merging error: mergEE is empty.");

	static const unsigned int rec_size = cRecord::record_size();

	for ( unsigned int i = 0 ; i < rec_size; ++i ) {
		list < const cAttribute ** > l1;
		for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
			l1.push_back( const_cast < const cAttribute ** > ( &(*p)->get_attrib_pointer_by_index(i)  )   );
		}

		list < const cAttribute ** > l2;
		for ( cGroup_Value::const_iterator p = mergee.m_fellows.begin(); p != mergee.m_fellows.end(); ++p ) {
			l2.push_back( const_cast < const cAttribute ** > ( &(*p)->get_attrib_pointer_by_index(i)  )   );
		}
		attrib_merge(l1, l2);
	}

	this->m_info = info;
	this->m_fellows.insert(m_fellows.end(), mergee.m_fellows.begin(), mergee.m_fellows.end());
	mergee.m_fellows.clear();

	// The folowing step actually changes the raw data. Changes the abbreviated middlename to a longer one if possible.
	static const unsigned int midname_index = cRecord::get_index_by_name(cMiddlename::static_get_class_name());
	static const unsigned int lastname_index = cRecord::get_index_by_name(cLastname::static_get_class_name());
	map < const cAttribute *, const cAttribute *> last2mid;
	map < const cAttribute *, const cAttribute * >::iterator q;
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cAttribute * pl = (*p)->get_attrib_pointer_by_index(lastname_index);
		const cAttribute * pm = (*p)->get_attrib_pointer_by_index(midname_index);
		q = last2mid.find(pl);
		if ( q == last2mid.end() )  {
			if ( pm->is_informative() ) {
				last2mid.insert( std::pair< const cAttribute *, const cAttribute *> (pl, pm) );
			}
		}
		else {
			const unsigned int old_size = q->second->get_data().at(0)->size();
			const unsigned int new_size = pm->get_data().at(0)->size();
			if ( new_size > old_size )
				q->second = pm;
		}
	}

	map < const cAttribute *, const cAttribute * >::const_iterator cq;
	for ( cGroup_Value::iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cAttribute * pl = (*p)->get_attrib_pointer_by_index(lastname_index);
		const cAttribute * const & pm = (*p)->get_attrib_pointer_by_index(midname_index);
		cq = last2mid.find(pl);
		if ( pm->is_informative() && pm != cq->second ) {
			const cAttribute* & rpm = const_cast < const cAttribute* & > (pm);
			rpm = cq->second;
		}
	}
	// end of modification



	this->find_representative();
	//this->coauthor_list.insert(mergee.coauthor_list.begin(), mergee.coauthor_list.end());
	//mergee.reset_cCoauthor_pointer(this->coauthor_list);
	//mergee.coauthor_list.clear();
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

cCluster::cCluster( const cCluster & rhs ) : m_info(rhs.m_info), m_fellows(rhs.m_fellows), m_mergeable(true) //, coauthor_list(rhs.coauthor_list){
{	if ( rhs.m_mergeable == false )
		throw cException_Other("cCluster Copy Constructor error.");
	//rhs.m_mergeable = false;
	//this->reset_cCoauthor_pointer(this->coauthor_list);
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

#if 0
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
#endif

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
	//reset_coauthor_list(*reference_pointer);
	//reset_cCoauthor_pointer(coauthor_list);
	const unsigned int rec_size = cRecord::record_size();
	for ( unsigned int i = 0 ; i < rec_size; ++i ) {
		list < const cAttribute ** > l1;
		list < const cAttribute ** > l2;
		cGroup_Value::const_iterator p1 = this->m_fellows.begin();
		if ( p1 == this->m_fellows.end() )
			break;
		cGroup_Value::const_iterator q2 = p1;
		++q2;
		l2.push_back( const_cast < const cAttribute ** > ( &(*p1)->get_attrib_pointer_by_index(i)  )   );
		while ( q2 != this->m_fellows.end() ) {
			l1.push_back( const_cast < const cAttribute ** > ( &(*p1)->get_attrib_pointer_by_index(i)  )   );
			l2.pop_front();
			l2.push_back( const_cast < const cAttribute ** > ( &(*q2)->get_attrib_pointer_by_index(i)  )   );
			attrib_merge(l1, l2);
			++p1;
			++q2;
		}
	}
	m_usable = true;
}

void cCluster::find_representative()  {
	static const string useful_columns[] = { cFirstname::static_get_class_name(), cMiddlename::static_get_class_name(), cLastname::static_get_class_name(),
											cLatitude::static_get_class_name(), cAssignee::static_get_class_name(), cCity::static_get_class_name(), cCountry::static_get_class_name()};
	static const unsigned int nc = sizeof(useful_columns)/sizeof(string);
	vector < map < const cAttribute *, unsigned int > > tracer( nc );
	vector < unsigned int > indice;
	for ( unsigned int i = 0; i < nc; ++i )
		indice.push_back ( cRecord::get_index_by_name( useful_columns[i]));

	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		for ( unsigned int i = 0 ; i < nc; ++i ) {
			const cAttribute * pA = (*p)->get_attrib_pointer_by_index(indice.at(i));
			++ tracer.at(i)[pA];
		}
	}
	vector < const cAttribute * > most;
	for ( unsigned int i = 0; i < nc ; ++i ) {
		const cAttribute * most_pA = NULL;
		unsigned int most_cnt = 0;
		for ( map < const cAttribute *, unsigned int >::const_iterator p = tracer.at(i).begin(); p != tracer.at(i).end(); ++p ) {
			if ( p->second > most_cnt ) {
				most_cnt = p->second;
				most_pA = p->first;
			}
		}
		most.push_back( most_pA );
	}

	unsigned int m_cnt = 0;
	const cRecord * mp = NULL;
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		unsigned int c = 0;
		for ( unsigned int i = 0 ; i < nc; ++i ) {
			const cAttribute * pA = (*p)->get_attrib_pointer_by_index(indice.at(i));
			if ( pA == most.at(i) )
				++c;
		}
		if ( c > m_cnt ) {
			m_cnt = c;
			mp = *p;
		}
	}

	this->m_info.m_delegate = mp;

}


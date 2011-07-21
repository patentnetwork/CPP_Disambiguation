/*
 * DisambigNewCluster.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: ysun
 */

#include "DisambigNewCluster.h"

//static members initialization.
const cRatios * cCluster::pratio = NULL;
const map < const cRecord *, cGroup_Value, cSort_by_attrib > * cCluster::reference_pointer = NULL;


/*
 * Aim: constructor of cCluster objects.
 */

cCluster::cCluster(const cCluster_Head & info, const cGroup_Value & fellows)
		: m_info(info), m_fellows(fellows), m_mergeable(true), m_usable(true) {
	if ( NULL == reference_pointer )
		throw cException_Other("Critical Error: Patent tree reference pointer is not set yet.");
	this->first_patent_year = invalid_year;
	this->last_patent_year = invalid_year;
	this->update_year_range();
	this->update_locations();
}

/*
 * Aim: merge the mergee object into "*this", with the new cluster head = info ( Actually only the cohesion is used,
 * 		because the delegate will be reset by find_representative).
 * Algorithm: put the mergee's members into "*this" object, and set mergee's signal to false.
 * 				And then call find_representative.
 */

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
	mergee.locs.clear();

	this->find_representative();
	this->update_year_range();
	this->update_locations();
	mergee.m_mergeable = false;
}

/*
 * Aim: To change the pointer to abbreviated middle names to the full names.
 * Algorithm: build a binary tree: unique last name -> longest middle name pointer. Then traverse the whole record members to
 * 			update the binary tree. Finally, traverse the record members again, look up their last names and update their
 * 			middle name pointers to the corresponding pointer.
 * ATTENTION: for records without middle names (ie. the pointer of middle name points to an empty string ), the modification will
 * 			NOT take place. This is because it is very risky, as we have no information about the real middle name.
 */

void cCluster::change_mid_name()  {
	// The folowing step actually changes the raw data. Changes the abbreviated middlename to a longer one if possible.
	if ( ! cMiddlename::is_enabled() )
		return;
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
		//skip empty middle names.
		if ( pm->is_informative() && pm != cq->second ) {
			cq->second->add_attrib(1);
			pm->reduce_attrib(1);
			const cAttribute* & rpm = const_cast < const cAttribute* & > (pm);
			rpm = cq->second;
		}
	}
	// end of modification
}



//destructor.
cCluster::~cCluster() {}


//copy constructor
cCluster::cCluster( const cCluster & rhs ) : m_info(rhs.m_info), m_fellows(rhs.m_fellows), m_mergeable(true),
		first_patent_year ( rhs.first_patent_year ), last_patent_year ( rhs.last_patent_year ){
	if ( rhs.m_mergeable == false )
		throw cException_Other("cCluster Copy Constructor error.");
}

/*
 * Aim: to compare "*this" with rhs, and check whether the clusters should merge. If it is a merge, the returned
 * 		cluster head consists of a non-NULL cRecord pointer and a cohesion value; if it is nor a merge, the returned cluster
 * 		head is composed of a NULL cRecord pointer, and the cohesion value is useless then.
 * Algorithm: Check if some special cases should be dealt first. Otherwise, call disambiguate_by_set in DisambigEngine.cpp
 *
 */

cCluster_Head cCluster::disambiguate( const cCluster & rhs, const double prior, const double mutual_threshold) const {
	static const unsigned int country_index = cRecord::get_index_by_name(cCountry::static_get_class_name());
	static const string asian_countries[] = {"JP"};
	static const double asian_threshold = 0.99;
	static const double max_threshold = 0.999;


	if ( pratio == NULL)
		throw cException_Other("Critical: ratios map is not set yet.");
	if ( this->m_mergeable == false ) {
		throw cException_Empty_Cluster("Comparison error: lhs is empty.");
	}
	if ( rhs.m_mergeable == false ) {
		throw cException_Empty_Cluster("Comparison error: rhs is empty.");
	}

	double threshold = mutual_threshold;
	const cAttribute * this_country = this->m_info.m_delegate->get_attrib_pointer_by_index(country_index);
	const cAttribute * rhs_country = rhs.m_info.m_delegate->get_attrib_pointer_by_index(country_index);

	for ( unsigned int i = 0; i < sizeof(asian_countries)/sizeof(string); ++i ) {
		if ( this_country == rhs_country && * this_country->get_data().at(0) == asian_countries[i] ) {
			threshold = asian_threshold > mutual_threshold ? asian_threshold : mutual_threshold;
			break;
		}
	}

	unsigned int gap = this->patents_gap(rhs);
	bool location_penalize = false;
	const unsigned int common_locs = num_common_elements(this->locs.begin(), this->locs.end(), rhs.locs.begin(), rhs.locs.end(), 1 );
	if ( gap == 0 && common_locs == 0 )
		location_penalize = true;
	static const unsigned int max_gap = 20;
	if ( gap > max_gap )
		gap = max_gap;

	double prior_to_use = prior;
	double threshold_to_use = threshold;

	threshold_to_use = threshold + ( max_threshold - threshold ) * gap / max_gap;

	if ( prior_to_use == 0 )
		prior_to_use = 0.01;

	if ( location_penalize ) {
		const double t = threshold_to_use + ( max_threshold - threshold_to_use ) / 2;
		threshold_to_use = t;
	}
	if ( threshold_to_use > max_threshold )
		threshold_to_use = max_threshold;

	std::pair<const cRecord *, double > ans ( disambiguate_by_set ( this->m_info.m_delegate, this->m_fellows, this->m_info.m_cohesion,
												rhs.m_info.m_delegate, rhs.m_fellows, rhs.m_info.m_cohesion,
													prior_to_use, *pratio, threshold_to_use) );

	return cCluster_Head(ans.first, ans.second);

}

/*
 * Aim: insert a new record pointer to the member list.
 */

void cCluster::insert_elem( const cRecord * more_elem) {
	this->m_fellows.push_back(more_elem);
	m_usable = false;
}

/*
 * Aim: to repair a cluster.
 */

void cCluster::self_repair() {
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
	//if it has not been merged before and m_usable is false, reset to usable.
	this->update_year_range();
	this->update_locations();
	if ( m_usable == false && ! m_fellows.empty())
		m_usable = true;
}


/*
 * Aim: to find a representative/delegate for a cluster.
 * Algorithm: for each specified column, build a binary map of const cAttribute pointer -> unsigned int ( as a counter).
 * 			Then traverse the whole cluster and fill in the counter. Finally, get the most frequent.
 *
 */
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

void cCluster::update_year_range() {
	static const unsigned int appyearindex = cRecord::get_index_by_name(cApplyYear::static_get_class_name());
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cAttribute * pAttribYear = (*p)->get_attrib_pointer_by_index(appyearindex);
		const string * py = pAttribYear->get_data().at(0);
		unsigned int year = atoi ( py->c_str());
		if ( year > 2100 || year < 1500 ) {
			//(*p)->print();
			//throw cException_Other("Application year error.");
			continue;
		}

		if ( this->is_valid_year() ) {
			if ( year > this->last_patent_year )
				this->last_patent_year = year;
			if ( year < this->first_patent_year )
				this->first_patent_year = year;
		}
		else {
			this->first_patent_year = year;
			this->last_patent_year = year;
		}
	}

}

bool cCluster::is_valid_year() const {
	if (this->first_patent_year == invalid_year || this->last_patent_year == invalid_year )
		return false;
	else
		return true;
}

unsigned int cCluster::patents_gap( const cCluster & rhs) const {
	if ( ! this->is_valid_year() || ! rhs.is_valid_year() )
		return 0;
	unsigned int x = 0;
	if ( this->first_patent_year > rhs.last_patent_year )
		x = this->first_patent_year - rhs.last_patent_year ;
	else if  ( this->last_patent_year < rhs.first_patent_year )
		x = rhs.first_patent_year - this->last_patent_year ;

	if ( x > 500 )
		throw cException_Other("Patent gap error.");
	return x;
}


void cCluster::update_locations() {
	locs.clear();
	static const unsigned int latindex = cRecord::get_index_by_name(cLatitude::static_get_class_name());
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != this->m_fellows.end(); ++p ) {
		const cAttribute * pA = (*p)->get_attrib_pointer_by_index(latindex);
		const cLatitude * pAttribLat = dynamic_cast< const cLatitude *> ( pA );
		if ( pAttribLat == 0 ) {
			(*p)->print();
			std::cerr << "Data type is " << typeid(*pA).name() << std::endl;
			throw cException_Other("bad cast from cAttrib to cLatitude. cCluster::update_location error.");
		}
		if ( pAttribLat->is_informative() )
			locs.insert(pAttribLat);
	}
}

void cCluster::add_uid2uinv( map < const cRecord *, const cRecord *> & uid2uinv ) const {
	map < const cRecord *, const cRecord *>::iterator q;
	for ( cGroup_Value::const_iterator p = this->m_fellows.begin(); p != m_fellows.end(); ++p ) {
		q = uid2uinv.find(*p);
		if ( q != uid2uinv.end() )
			throw cException_Other("Add uid: already exists.");
		else
			uid2uinv.insert(std::pair<const cRecord *, const cRecord *>(*p, this->m_info.m_delegate));
	}
}

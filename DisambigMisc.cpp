/*
 * DisambigMisc.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: ysun
 */

#include "DisambigMisc.h"

map <const cRecord *, const cRecord *>::iterator cTracer::get_end_node( const cRecord * target) {
	map <const cRecord *, const cRecord *>::iterator p;
	while ( true ) {
		p = one2one_mapping.find(target);
		if ( p == one2one_mapping.end() )
			throw cException_Tracer("Tracer Error.");
		if ( p->first == p->second )
			break;
		else
			target = p->second;
	}
	return p;
}


map <const cRecord *, const cRecord *>::const_iterator cTracer::get_end_node( const cRecord * target) const {
	map <const cRecord *, const cRecord *>::const_iterator p;
	while ( true ) {
		p = one2one_mapping.find(target);
		if ( p == one2one_mapping.end() )
			throw cException_Tracer("Tracer Error.");
		if ( p->first == p->second )
			break;
		else
			target = p->second;
	}
	return p;
}

cTracer::cTracer(const map <const cRecord*, cGroup_Value >& m ) {
	convert_from_cluster(m);
}

cTracer::cTracer( const cSimplified_Cluster & c) {
	convert_from_cluster(c.get_map());
}

void cTracer::convert_from_cluster(const map <const cRecord*, cGroup_Value >& m ) {
	one2one_mapping.clear();
	for (  map <const cRecord*, cGroup_Value >::const_iterator p = m.begin(); p != m.end(); ++p ) {
		const cGroup_Value & alias = p->second;
		for ( cGroup_Value::const_iterator q = alias.begin(); q != alias.end(); ++q ) {
			one2one_mapping.insert(std::pair<const cRecord*, const cRecord *>(*q, p->first));
		}
	}
}

void cTracer::convert_to_cluster( cSimplified_Cluster & c) const {
	map < const cRecord * , cGroup_Value> & alias = c.get_map();
	alias.clear();
	cGroup_Value temp;
	map <const cRecord*, const cRecord * >::const_iterator q;
	for (  map <const cRecord*, const cRecord * >::const_iterator p = one2one_mapping.begin(); p != one2one_mapping.end(); ++p ) {
		q = p;
		while ( q->first != q->second )
			q = one2one_mapping.find(q->second);

		map < const cRecord * , cGroup_Value>::iterator x = alias.find(q->second);
		if ( x == alias.end()) {
			x = alias.insert(std::pair<const cRecord *, cGroup_Value>(q->second, temp)).first;
		}
		x->second.push_back(p->first);
	}
}

void cData_For_Clustering::merge( const cRecord * virtual_k1, const cRecord * virtual_k2) {
	const cRecord * k1 = m_trace.get_end_node_pointer(virtual_k1);
	const cRecord * k2 = m_trace.get_end_node_pointer(virtual_k2);

	cGroup_Value & v1 = m_cluster.get_map().find(k1)->second;
	cGroup_Value & v2 = m_cluster.get_map().find(k2)->second;

	if ( m_size_comparator(k1, k2) ) {
		m_trace.update_end_node(k1, k2, m_size_comparator);
		v2.insert(v2.end(), v1.begin(), v1.end());
		v1.clear();
	}
	else {
		m_trace.update_end_node(k1, k2, m_size_comparator);
		v1.insert(v1.end(), v2.begin(), v2.end());
		v2.clear();
	}
}










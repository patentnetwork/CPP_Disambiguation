/*
 * DisambigMisc.h
 *
 *  Created on: Feb 5, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGMISC_H_
#define DISAMBIGMISC_H_

#include "DisambigDefs.h"


class cRecord;
class cException_Tracer : public cAbstract_Exception {
public:
	cException_Tracer(const char* errmsg): cAbstract_Exception(errmsg) {};
};

using std::map;

class cSimplified_Cluster;

class cTracer {
private:
	map <const cRecord *, const cRecord *> one2one_mapping;
public:
	map <const cRecord *, const cRecord *>::iterator get_end_node( const cRecord * target);
	map <const cRecord *, const cRecord *>::const_iterator get_end_node( const cRecord * target) const ;
	const cRecord * get_end_node_pointer( const cRecord * target ) const {
		return get_end_node(target)->first;
	}

	template <typename Functor>
	void update_end_node (const cRecord * key, const cRecord * value, const Functor & func ) {
		typedef map <const cRecord *, const cRecord *>::iterator Iter;
		Iter pp = one2one_mapping.find(key);
		if ( pp == one2one_mapping.end())
			one2one_mapping.insert(std::pair<const cRecord *, const cRecord *>(key, key));
		Iter qq = one2one_mapping.find(value);
		if ( qq == one2one_mapping.end())
			one2one_mapping.insert(std::pair<const cRecord *, const cRecord *>(value, value));
		pp = get_end_node(key);
		qq = get_end_node(value);
		// if pp < qq, set pp to qq
		if ( func(pp->second, qq->second )) {
			pp->second = value;
		}
		else {
			qq->second = key ;
		}
	}
	cTracer(const map <const cRecord*, cGroup_Value >& m );
	cTracer( const cSimplified_Cluster & m);
	void convert_from_cluster(const map <const cRecord*, cGroup_Value >& m );
	void convert_to_cluster(cSimplified_Cluster & c) const;

};



class cSimplified_Cluster {
private:
	map < const cRecord *, cGroup_Value > cluster_map;
public:
	cSimplified_Cluster( const map <const cRecord*, cGroup_Value >& m): cluster_map(m) {}
	map < const cRecord *, cGroup_Value > & get_map() {return cluster_map;}
	const map< const cRecord*, cGroup_Value> & get_map() const {return cluster_map;}
};


class cGroup_Size_Comp {
private:
	const map < const cRecord*, cGroup_Value > * pm;
public:
	bool operator() ( const cRecord * p1, const cRecord * p2) const {
		map < const cRecord *, cGroup_Value >::const_iterator p = pm->find(p1);
		if ( p == pm->end() )
			throw cException_Other("Group size comp find error.");
		const cGroup_Value & v1 = p ->second;
		p = pm->find(p2);
		if ( p == pm->end() )
			throw cException_Other("Group size comp find error.");
		const cGroup_Value & v2 = p ->second;

		return v1.size() < v2.size();
	}
	explicit cGroup_Size_Comp( const cSimplified_Cluster & m): pm (&m.get_map()) {}
};









class cData_For_Clustering {
private:
	cSimplified_Cluster m_cluster;
	cTracer m_trace;
	const cGroup_Size_Comp m_size_comparator;
public:
	void merge( const cRecord * virtual_k1, const cRecord * virtual_k2);
	cData_For_Clustering(const map <const cRecord*, cGroup_Value >& m): m_cluster(m), m_trace(m_cluster), m_size_comparator(m_cluster) {}
};












#endif /* DISAMBIGMISC_H_ */

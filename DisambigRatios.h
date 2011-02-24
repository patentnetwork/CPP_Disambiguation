/*
 *  DisambigRatios.h
 *  mydisambiguation
 *
 *  Created by Katelin on 10-12-27.
 *  Copyright 2010 Dartmouth College. All rights reserved.
 *
 */

#ifndef _DISAMBIG_RATIOS_H_
#define _DISAMBIG_RATIOS_H_
#include <iostream>
#include <map>
#include <set>
#include "DisambigDefs.h"
#include <fstream>

using std::string;
using std::set;
using std::map;



typedef vector < unsigned int > cSimilarity_Profile;

struct cMonotonic_Similarity_Compare {
private:
	unsigned int compare_entry;
public:
	bool operator() ( const cSimilarity_Profile * p1, const cSimilarity_Profile * p2 ) const {
		return p1->at(compare_entry) < p2->at(compare_entry);
	}
	cMonotonic_Similarity_Compare( const unsigned int entry) : compare_entry(entry) {};
	void reset_entry( const unsigned int entry) { compare_entry = entry;}
};

typedef set< const cSimilarity_Profile *, cMonotonic_Similarity_Compare> monotonic_set;


struct cSimilarity_With_Monotonicity_Dimension {
private:
	const cSimilarity_Profile * psim;
	unsigned int monotonic_dimension;
	const unsigned int size;

	bool compare_without_primary( const cSimilarity_Profile * p1, const cSimilarity_Profile * p2 ) const {
		unsigned int i;
		for (  i = 0; i < size; ++i ) {
			if ( i == monotonic_dimension )
				continue;
			if ( p2->at(i) < p1->at(i) )
				return false;
			else if ( p1->at(i) < p2->at(i) )
				return true;
		}
		return ( i != size );
	}

public:
	bool operator < ( const cSimilarity_With_Monotonicity_Dimension & rhs) const {
		if ( this->monotonic_dimension < rhs.monotonic_dimension )
			return false;
		else if ( rhs.monotonic_dimension < this->monotonic_dimension )
			return true;
		else
			return compare_without_primary( psim, rhs.psim );
	}
	const unsigned int get_monotonic_dimension() const {return monotonic_dimension;}
	explicit cSimilarity_With_Monotonicity_Dimension( const cSimilarity_Profile * p, const unsigned int dm )
		: psim ( p ), monotonic_dimension(dm), size ( p->size()) {}
};




//=================================================
class cRatioComponent {
private:	
	static const unsigned int laplace_base;
	class cException_Partial_SP_Missing : public cAbstract_Exception {
	public:
		cException_Partial_SP_Missing(const char* errmsg): cAbstract_Exception(errmsg){};
	};
	
	
	map < vector <unsigned int>, double, cSimilarity_Compare > ratio_map;
	vector < unsigned int > positions_in_ratios;
	vector < unsigned int > positions_in_record;
	const list <cRecord> * psource;
	//const string unique_identifier;
	const string attrib_group;
	const map < string, const cRecord *> * puid_tree;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > similarity_map;
	vector < string > attrib_names;
	bool is_ready;
	map < vector < unsigned int > , unsigned int, cSimilarity_Compare > x_counts, m_counts;
	void sp_stats (const list<std::pair<string, string> > & trainpairs, 
				   map < vector < unsigned int > , unsigned int, cSimilarity_Compare > & sp_counts ) const;
	void read_train_pairs(list<std::pair<string, string> > & trainpairs, const char * txt_file) const;
	void get_similarity_info();
public:
	class cException_Ratios_Not_Ready : public cAbstract_Exception {
	public:
		cException_Ratios_Not_Ready(const char* errmsg): cAbstract_Exception(errmsg){};
	};
	
	explicit cRatioComponent( const list <cRecord> & source, const map < string, const cRecord * > & uid_tree, const string & groupname);
	
	void prepare(const char* x_flie, const char * m_file);
	const map < vector < unsigned int >, double, cSimilarity_Compare > & get_ratios_map() const {
		if ( is_ready )
			return ratio_map;
		else {
			throw cException_Ratios_Not_Ready("Ratio component map is not ready.");
		}
	}
	const map < vector < unsigned int >, unsigned int, cSimilarity_Compare > & get_x_counts() const {return x_counts;}
	const map < vector < unsigned int >, unsigned int, cSimilarity_Compare > & get_m_counts() const {return m_counts;}
	const vector < unsigned int > & get_component_positions_in_ratios() const {return positions_in_ratios;};
	const vector < unsigned int > & get_component_positions_in_record() const { return positions_in_record;};
	void smooth();
	void stats_output( const char * ) const;
	const vector < string > & get_attrib_names() const { return attrib_names;}
};


class cRatios {
private:
	map < vector <unsigned int>, double, cSimilarity_Compare > final_ratios;
	vector < string > attrib_names;
	unsigned int ratio_size;
	//vector <double> coeffs;
	//unsigned int final_root_order;
	map < vector < unsigned int > , unsigned int, cSimilarity_Compare > x_counts, m_counts;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > similarity_map;




	void More_Components( const cRatioComponent & additional_component);
	void Get_Coefficients();
	static const char * primary_delim;
	static const char * secondary_delim;

public:
	cRatios(const vector < const cRatioComponent *> & component_vector, const char * filename, const cRecord & rec);
	cRatios( const char *filename);
	const map < vector < unsigned int >, double, cSimilarity_Compare > & get_ratios_map() const {
		return final_ratios;
	}
	//const vector <double> & get_coefficients_vector() const { return coeffs;}
	void read_ratios_file(const char * filename);
	void write_ratios_file(const char * filename) const;
	//unsigned int get_final_order () const {return final_root_order;}
	void inter_extra_polation( const vector < unsigned int >& max_similarity, const vector < unsigned int > & min_similarity);
	void smooth();
	vector < unsigned int > get_max_similarity( const cRecord & rec) const ;
	const vector < string > & get_attrib_names() const { return attrib_names;}
};

//====================



#endif

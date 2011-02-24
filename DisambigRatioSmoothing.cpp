/*
 * DisambigClusterSmoothing.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: ysun
 */

#include "DisambigRatios.h"
#include "DisambigEngine.h"

#include <limits>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "QuadProg++.h"


#include "QpGenData.h"
#include "QpGenVars.h"
#include "QpGenResiduals.h"
#include "GondzioSolver.h"
#include "QpGenSparseMa27.h"


static const bool should_do_name_range_check = true;

/*
 File main.cc

 This file contains just an example on how to set-up the matrices for using with
 the solve_quadprog() function.

 The test problem is the following:

 Given:
 G =  4 -2   g0^T = [6 0]
     -2  4

 Solve:
 min f(x) = 1/2 x G x + g0 x
 s.t.
   x_1 + x_2 = 3
   x_1 >= 0
   x_2 >= 0
   x_1 + x_2 >= 2

 The solution is x^T = [1 2] and f(x) = 12

 Author: Luca Di Gaspero
 DIEGM - University of Udine, Italy
 l.digaspero@uniud.it
 http://www.diegm.uniud.it/digaspero/

 LICENSE

 This file is part of QuadProg++: a C++ library implementing
 the algorithm of Goldfarb and Idnani for the solution of a (convex)
 Quadratic Programming problem by means of an active-set dual method.
 Copyright (C) 2007-2009 Luca Di Gaspero.
 Copyright (C) 2009 Eric Moyer.

 QuadProg++ is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 QuadProg++ is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with QuadProg++. If not, see <http://www.gnu.org/licenses/>.
*/


/*
 * In our problem, f(r) = sum( w(ri) * ( r - ri )^2 )
 * s.t. r > 0, monotonicity of r (explain later) to give the inequality matrix.
 *
 * So G = diag( 2*w(r1), 2*w(r2), ... , 2*w(rn) )
 * 		g0 = [ -2*w(r1), -2*w(r2), ... , -2*w(rn) ]
 */





unsigned int coord2index ( const vector < unsigned int > & coord, const vector < unsigned int > & data_count) {
	if ( coord.size() != data_count.size() )
		throw cException_Other("Convertion error in smoothing.");
	unsigned int index = 0;
	for ( unsigned int i = 0; i < coord.size(); ++i ) {
		if ( coord.at(i) > data_count.at(i) )
			throw cException_Other("Index range error.");
		if ( 1 > data_count.at(i) )
			throw cException_Other("Index range error.");
		index *= data_count.at(i);
		index += coord.at(i);
	}
	return index;
}

vector < unsigned int > index2coord ( unsigned int index, const vector < unsigned int > & data_count ) {
	vector < unsigned int > coord(data_count.size(), 0);
	for ( unsigned int i = data_count.size() - 1 ; i >= 0 ; --i ) {
		if ( 1 > data_count.at(i) )
			throw cException_Other("Index range error.");
		coord.at(i) = index % data_count.at(i);
		index /= data_count.at(i);
	}
	if ( index >=1 )
		throw cException_Other("Index range error.");
	return coord;
}

template < typename Tp >
vector < int > compare_entry( const vector < Tp > & v1, const vector < Tp > & v2 ) {
	const unsigned int size = v1.size();
	if ( v2.size() != size )
		throw cException_Other("Different dimension.");
	vector < int > result ( size, 0);
	for ( unsigned int i = 0; i < size; ++i ) {
		int & temp = result.at(i);

		if ( v1.at(i) < v2.at(i) )
			temp = 1;
		else if ( v1.at(i) == v2.at(i))
			temp = 0;
		else
			temp = 1;
	}
	return result;
}

int is_monotonic( const vector < int > & entry_comparison ) {
	int res = 0;
	for ( unsigned int i = 0; i < entry_comparison.size(); ++i ) {
		int temp;
		if ( entry_comparison.at(i) == 0 )
			continue;
		else if ( entry_comparison.at(i) < 0 )
			temp = -1;
		else
			temp = 1;

		if ( res == 0 )
			res = temp;
		else if ( res != temp )
			return 0;	// cannot compare
	}
	return res;
}


template < typename Tp >
int is_monotonic( const vector < Tp > & v1, const vector < Tp > & v2 ) {
	// one dimensional monotonicity.
	const unsigned int size = v1.size();
	if ( v2.size() != size )
		throw cException_Other("Different dimension.");
	int result = 0;
	for ( unsigned int i = 0; i < size; ++i ) {
		int temp;

		if ( v1.at(i) < v2.at(i) )
			temp = -i;
		else if ( v1.at(i) == v2.at(i))
			continue;
		else
			temp = i;

		if ( result == 0 )
			result = temp;
		else if ( result != temp )
			return 0;	// cannot compare
	}
	return result;
}



void smoothing(map < cSimilarity_Profile, double,  cSimilarity_Compare  >& ratio_map,
				map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > & similarity_map,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & x_counts,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & m_counts,
				const vector < string > & attribute_names, const bool name_range_check ) {
	typedef set< const cSimilarity_Profile *, cMonotonic_Similarity_Compare> cMonotonic_set;

	const double delta = 0.00001;
	if ( ratio_map.empty() )
		return;
	//const unsigned int n = data_range.size();
	const unsigned int n = ratio_map.begin()->first.size();
	if ( n != ratio_map.begin()->first.size())
		throw cException_Other("Critical Error: the size of similarity profile is not the same as that of the provided data range vector! ");


	bool is_ooqp_success = true;
	//bool is_quadprog_success = true;

	// now building matrix G;
	//G = diag( 2*w(r1), 2*w(r2), ... , 2*w(rn) )
	// 		g0 = [ -2*w(r1), -2*w(r2), ... , -2*w(rn) ]

	const unsigned int sz = ratio_map.size();

	map < const cSimilarity_Profile*, unsigned int > similarity_sequence;
	unsigned int sequence_count = 0;
	for ( map < cSimilarity_Profile, double,  cSimilarity_Compare  >::const_iterator prm = ratio_map.begin(); prm != ratio_map.end(); ++prm ) {
		similarity_sequence.insert(std::pair< const cSimilarity_Profile *, unsigned int > (&prm->first, sequence_count));
		++ sequence_count;
	}

	//vector < double> row ( sz, 0 );
	//vector < vector < double> > G (sz , row );
	//QuadProgPP::Matrix<double> G(0.0, sz, sz);
	//vector < double > g0 ( row );
	//QuadProgPP::Vector<double> g0(0.0, sz);
	map < vector <unsigned int>, double, cSimilarity_Compare >::const_iterator pratio = ratio_map.begin();
/*
	for ( unsigned int i = 0; i < sz ; ++i ) {
		const vector < unsigned int > & sp = pratio->first;
		unsigned int wt = x_counts.find(sp)->second + m_counts.find(sp)->second ;
		G[i][i] = 2.0 * wt;
		g0[i] = (-2.0) * wt * pratio->second;	// in the same r's sequence
		++pratio;
	}
	*/
	//now using monotunicity to get the restrictive matrix

	//vector < double > largerow ( sz * (sz - 1), 0 );
	//vector < vector < double > > CI ( sz * ( sz - 1 ), largerow );
	//QuadProgPP::Matrix<double> CI ( 0.0, sz*(sz-1), sz*(sz - 1) );


	//map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > similarity_map;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::iterator psimmap;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::const_iterator cpsimmap;

	vector < cMonotonic_Similarity_Compare > msc_pool( n, 0 );
	for ( unsigned int i = 0; i < n; ++i )
		msc_pool.at(i).reset_entry(i);

	// this will generate a list of monotonic vectors. ie. v(2,1,1) > v(1,1,1) > v(0,1,1) > delta;
	for ( pratio = ratio_map.begin(); pratio !=ratio_map.end(); ++pratio ) {
		for ( unsigned int i = 0; i < n ; ++i ) {
			cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, i);
			psimmap = similarity_map.find(smd);
			if ( psimmap == similarity_map.end() ) {
				monotonic_set ms( msc_pool.at(i) );
				ms.insert( &pratio->first);
				similarity_map.insert(std::pair < cSimilarity_With_Monotonicity_Dimension, monotonic_set > (smd, ms ) );
			}
		}

		map < vector <unsigned int>, double, cSimilarity_Compare >::const_iterator qratio = pratio;
		for ( ++qratio; qratio != ratio_map.end(); ++qratio ) {
			int monotonicity_check = is_monotonic <unsigned int> (pratio->first, qratio->first);
			if ( monotonicity_check != 0 ) {
				cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, abs( monotonicity_check));
				similarity_map.find(smd)->second.insert(&qratio->first);
			}
		}
	}

	// edit matrix here;
	// possible monotonicity relation size =  ( na - 1 ) * ( nb - 1 ) * ( nc - 1 ) * ( nd - 1 ) * ( ne - 1 ) * ( nf - 1 )
	// size of matrix = na*nb*nc*nd*ne*nf.	so should have enough space for any single ( 1, -1 ) pair in the matrix;



	int count = 0;
	// ci0's size, m, is the sum of all the inequality relations in the similarity map.
	unsigned int m = 0;
	set< const cSimilarity_Profile *, cSimilarity_Compare > min_sp;
	for ( cpsimmap = similarity_map.begin(); cpsimmap != similarity_map.end(); ++ cpsimmap ) {
		const unsigned int set_size = cpsimmap->second.size();
		min_sp.insert(*cpsimmap->second.begin());
		m += set_size - 1 ;
	}





	m += min_sp.size();



	const double midname_upper = 1.0;
	const double lastname_upper = 1.0;
	const double firstname_upper = 1.0;
	const double too_big = 1e10;

	bool midname_range_check = name_range_check;
	const unsigned int midname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cMiddlename::static_get_class_name()));
	if ( midname_location == attribute_names.size() )
		midname_range_check = false;

	if ( midname_range_check )
		std::cout << "Middle name ratio adjustment activated." << std::endl;


	bool firstname_range_check = name_range_check;
	const unsigned int firstname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cFirstname::static_get_class_name()));
	if ( firstname_location == attribute_names.size() )
		firstname_range_check = false;

	if ( firstname_range_check )
		std::cout << "First name ratio adjustment activated." << std::endl;

	bool lastname_range_check = true;
	const unsigned int lastname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cLastname::static_get_class_name()));
	if ( lastname_location == attribute_names.size() )
		lastname_range_check = false;

	if ( lastname_range_check )
		std::cout << "Last name ratio adjustment activated." << std::endl;






	if ( is_ooqp_success ) {

		//==========================
		///USING OOQP NOW.

		const bool show_ooqp_process = true;

		const int nx = sz;
		double * const c = new double [nx];
		double * const xlow = new double [nx];
		char * const ixlow = new char[nx];
		double * const xupp = new double [nx];
		char * const ixupp = new char[nx];

		const int nnzQ = nx;
		int * const irowQ = new int [nnzQ];
		int * const jcolQ = new int [nnzQ];
		double * const dQ = new double [nnzQ];

		const int my = 0; // equality constraints.
		double * const b     = 0;
		const int nnzA       = 0;
		int * const irowA    = 0;
		int * const jcolA    = 0;
		double * const dA    = 0;

		const int mz = m - min_sp.size(); //number of inequality
		double * const clow = new double [mz];
		char * const iclow = new char [mz];
		double * const cupp = new double [ mz ];
		char * const icupp = new char [mz];

		const int nnzC = mz * 2;
		int * const irowC = new int [nnzC];
		int * const jcolC = new int [nnzC];
		double * const dC = new double [nnzC];

		std::cout << "---------MATRIX SIZE = " << mz <<" x " << nx << " -----------" << std::endl;

		pratio = ratio_map.begin();



		for ( unsigned int i = 0; i < sz ; ++i ) {
			const vector < unsigned int > & sp = pratio->first;
			unsigned int wt = x_counts.find(sp)->second + m_counts.find(sp)->second ;
			dQ[i] = 2.0 * wt;
			c[i] = (-2.0) * wt * pratio->second;	// in the same r's sequence

			xlow[i] = delta;
			ixlow[i] = 1;


			xupp[i] = too_big;
			ixupp[i] = 0;
			if ( midname_range_check && pratio->first.at(midname_location) == 0 ) {
				if ( xupp[i] > midname_upper )
					xupp[i] = midname_upper;
				ixupp[i] = 1;
			}
			if ( lastname_range_check && pratio->first.at(lastname_location) == 0 ) {
				if ( xupp[i] > lastname_upper )
					xupp[i] = lastname_upper;
				ixupp[i] = 1;
			}
			if ( firstname_range_check && pratio->first.at(firstname_location) == 0 ) {
				if ( xupp[i] > firstname_upper )
					xupp[i] = firstname_upper;
				ixupp[i] = 1;
			}
			if ( xupp[i] == too_big ){
				xupp[i] = 0;
				//ixupp[i] = 0;
			}

			irowQ[i] = i;
			jcolQ[i] = i;

			++pratio;
		}

		count = 0;
		for ( cpsimmap = similarity_map.begin(); cpsimmap != similarity_map.end(); ++ cpsimmap ) {
			const monotonic_set & set_alias = cpsimmap->second;
			monotonic_set::const_iterator pset = set_alias.begin();
			//ci0[count] = -delta;
			//unsigned int seq1 = similarity_sequence.find( *pset )->second;
			//CI[seq1][count] = 1;
			//++count;
			monotonic_set::const_iterator qset = pset;
			for ( ++qset; qset != set_alias.end(); ++qset ) {
				if ( count >= mz )
					throw cException_Other("OOQP error.");
				clow[count] = 0;
				iclow[count] = 1;
				cupp[count] = 0;
				icupp[count] = 0;
				unsigned int seq1 = similarity_sequence.find( *pset )->second;
				unsigned int seq2 = similarity_sequence.find( *qset )->second;
				irowC[2 * count] = count;
				jcolC[2 * count] = seq1;
				dC[ 2 * count ] = -1;
				irowC[2 * count + 1 ] = count;
				jcolC[2 * count + 1 ] = seq2;
				dC[ 2 * count + 1 ] = 1;

				++pset;
				++count;
			}
		}


		QpGenSparseMa27 * qp = new QpGenSparseMa27( nx, my, mz, nnzQ, nnzA, nnzC );

		QpGenData      * prob = (QpGenData * ) qp->copyDataFromSparseTriple(
			c,      irowQ,  nnzQ,   jcolQ,  dQ,
			xlow,   ixlow,  xupp,   ixupp,
			irowA,  nnzA,   jcolA,  dA,     b,
			irowC,  nnzC,   jcolC,  dC,
			clow,   iclow,  cupp,   icupp );

		QpGenVars      * vars
			  = (QpGenVars *) qp->makeVariables( prob );

		QpGenResiduals * resid
			= (QpGenResiduals *) qp->makeResiduals( prob );

		GondzioSolver  * s     = new GondzioSolver( qp, prob );

		if ( show_ooqp_process )
			  s->monitorSelf();

		int ierr = s->solve(prob,vars, resid);

		std::stringstream ss;
		if( ierr == 0 ) {
			//cout.precision(4);
			//cout << "Solution: \n";
			//vars->x->writefToStream( cout, "x[%{index}] = %{value}" );
			vars->x->writeToStream(ss);

			const string ooqp_result ( ss.str() );

			map < cSimilarity_Profile, double,  cSimilarity_Compare  >::iterator prm = ratio_map.begin();

			string num_str;
			while ( getline(ss, num_str) ) {
				double r = atof(num_str.c_str());
				prm->second = r ;
				++prm;
			}
		}
		else {
			//throw cException_Other( "OOQP: Could not solve the problem.\n" );
			is_ooqp_success = false;
		}


		delete s;
		delete vars;
		delete prob;
		delete resid;
		delete qp;

		delete [] dC;
		delete [] irowC;
		delete [] jcolC;
		delete [] cupp;
		delete [] icupp;
		delete [] clow;
		delete [] iclow;
		delete [] dQ;
		delete [] jcolQ;
		delete [] irowQ;
		delete [] c;
		delete [] xlow;
		delete [] ixlow;
		delete [] xupp;
		delete [] ixupp;

	}

	if ( ! is_ooqp_success )  {
		std::cout << "---------MATRIX SIZE = " << m <<" x " << sz << " -----------" << std::endl;

		QuadProgPP::Matrix<double> G(0.0, sz, sz);
		QuadProgPP::Vector<double> g0(0.0, sz);

		pratio = ratio_map.begin();
		for ( unsigned int i = 0; i < sz ; ++i ) {
			const vector < unsigned int > & sp = pratio->first;

			bool insert_range = false;	//adjust m
			if ( midname_range_check && pratio->first.at(midname_location) == 0 ) {
				insert_range = true;
			}
			else if ( lastname_range_check && pratio->first.at(lastname_location) == 0 ) {
				insert_range = true;
			}
			else if ( firstname_range_check && pratio->first.at(firstname_location) == 0 ) {
				insert_range = true;
			}
			else {
				insert_range = false;
			}

			if ( insert_range )
				++m;	//adjust m

			unsigned int wt = x_counts.find(sp)->second + m_counts.find(sp)->second ;
			G[i][i] = 2.0 * wt;
			g0[i] = (-2.0) * wt * pratio->second;	// in the same r's sequence
			++pratio;
		}


		QuadProgPP::Vector<double> ci0(0.0, m);
		//REMEMBER CI is sz by m ! DO NOT GET CONFUSED WITH CI_TRANSPOSE;
		QuadProgPP::Matrix<double> CI(0.0, sz, m);

		count = 0;
		for ( map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator p = ratio_map.begin(); p != ratio_map.end(); ++p ) {
			if ( min_sp.find(&p->first) != min_sp.end() ) {
				//lower_bound
				ci0[count] = -delta;
				unsigned int seq = similarity_sequence.find(&p->first)->second;
				CI[seq][count] = 1;
				++count;
				//upper_bound

				double xupp= too_big;
				if ( midname_range_check && p->first.at(midname_location) == 0 ) {
					if ( xupp > midname_upper )
						xupp = midname_upper;
				}
				if ( lastname_range_check && p->first.at(lastname_location) == 0 ) {
					if ( xupp > lastname_upper )
						xupp = lastname_upper;
				}
				if ( firstname_range_check && p->first.at(firstname_location) == 0 ) {
					if ( xupp > firstname_upper )
						xupp = firstname_upper;
				}
				if ( xupp != too_big ){
					ci0[count] = xupp;
					CI[seq][count] = -1;
					++count;
				}

			}
		}

		for ( cpsimmap = similarity_map.begin(); cpsimmap != similarity_map.end(); ++ cpsimmap ) {
			const monotonic_set & set_alias = cpsimmap->second;
			monotonic_set::const_iterator pset = set_alias.begin();
			//ci0[count] = -delta;
			//unsigned int seq1 = similarity_sequence.find( *pset )->second;
			//CI[seq1][count] = 1;
			//++count;
			monotonic_set::const_iterator qset = pset;
			for ( ++qset; qset != set_alias.end(); ++qset ) {
				unsigned int seq1 = similarity_sequence.find( *pset )->second;
				unsigned int seq2 = similarity_sequence.find( *qset )->second;
				ci0[count] = 0;
				CI[seq1][count] = -1;
				CI[seq2][count] = 1;
				++pset;
				++count;
			}
		}

		// configure CE and ce0;
		QuadProgPP::Matrix<double> CE(0.0, sz, 0 );
		QuadProgPP::Vector<double> ce0(0.0, 0 );
		QuadProgPP::Vector<double> r(0.0, sz);
		//std::cout << "f: " << solve_quadprog(G, g0, CE, ce0, CI, ci0, r) << std::endl;
		double result = solve_quadprog(G, g0, CE, ce0, CI, ci0, r);
		if ( result == std::numeric_limits<double>::infinity() ) {
			std::cout << " WARNING: ----------->>  SMOOTHING FAILED." << std::endl;
			return;
		}
		//std::cout << "r: " << r << std::endl;

		count = 0;
		for ( map < cSimilarity_Profile, double,  cSimilarity_Compare  >::iterator prm = ratio_map.begin(); prm != ratio_map.end(); ++prm ) {
			prm->second = r[count++];
		}
	}


}


void update_monotonicity_map (const cSimilarity_Profile * psp, map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > & targetmap,
								const vector < cMonotonic_Similarity_Compare > & comparator_pool) {
	const unsigned int dimensions = psp->size();
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::iterator pm;
	for ( unsigned int i = 0; i < dimensions; ++i ) {
		cSimilarity_With_Monotonicity_Dimension smd(psp, i);

		pm = targetmap.find ( smd );
		if ( pm == targetmap.end() ) {
			monotonic_set ms( comparator_pool.at(i) );
			ms.insert( psp );
			targetmap.insert(std::pair < cSimilarity_With_Monotonicity_Dimension, monotonic_set > (smd, ms ) );
		}
		else {
			targetmap.find(smd)->second.insert(psp);
		}
	}
}


unsigned int interpolation( map < cSimilarity_Profile, double, cSimilarity_Compare > & ratio_map,
							map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > & similarity_map) {
	const unsigned int profile_size = ratio_map.begin()->first.size();
	std::cout << "Size of similarity map = " << similarity_map.size() << std::endl;


	//map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > similarity_map;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::iterator psimmap;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::const_iterator cpsimmap;


	vector < cMonotonic_Similarity_Compare > msc_pool( profile_size, 0 );
	for ( unsigned int i = 0; i < profile_size; ++i )
		msc_pool.at(i).reset_entry(i);
	/*
	// this will generate a list of monotonic vectors. ie. v(2,1,1) > v(1,1,1) > v(0,1,1) > delta;
	for ( map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator pratio = ratio_map.begin(); pratio !=ratio_map.end(); ++pratio ) {
		for ( unsigned int i = 0; i < profile_size ; ++i ) {
			cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, i);
			psimmap = similarity_map.find(smd);
			if ( psimmap == similarity_map.end() ) {
				monotonic_set ms( msc_pool.at(i) );
				ms.insert( &pratio->first);
				similarity_map.insert(std::pair < cSimilarity_With_Monotonicity_Dimension, monotonic_set > (smd, ms ) );
			}
		}

		map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator qratio = pratio;
		for ( ++qratio; qratio != ratio_map.end(); ++qratio ) {
			int monotonicity_check = is_monotonic <unsigned int> (pratio->first, qratio->first);
			if ( monotonicity_check != 0 ) {
				cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, abs( monotonicity_check));
				similarity_map.find(smd)->second.insert(&qratio->first);
			}
		}
	}
	*/
	// OK now we have obtained a similarity map;
	// now interpolating. easier way.

	unsigned int count = 0;
	const unsigned int count_base = 10000;
	unsigned int tempcount = count;
	while ( true ) {
		for ( psimmap = similarity_map.begin(); psimmap != similarity_map.end(); ++psimmap ) {
			const unsigned int dim = psimmap->first.get_monotonic_dimension();
			const monotonic_set & alias = psimmap->second;
			monotonic_set::const_iterator p = alias.begin();
			monotonic_set::const_iterator q = p;
			++q;
			while ( q != alias.end()) {
				const unsigned int starting = (*p)->at(dim);
				const unsigned int ending = (*q)->at(dim);
				const unsigned int root_order = ending - starting;
				if ( root_order < 2 ) {
					++q; ++p;
					continue;
				}

				const double larger = ratio_map.find(**q)->second;
				const double smaller = ratio_map.find(**p)->second;
				const double base = pow ( larger / smaller , 1.0/root_order ) ;

				for ( unsigned int i = 1; i < root_order; ++i ) {
					double calc_r = smaller * pow( base, static_cast<int>(i) );
					cSimilarity_Profile sp = **p;
					sp.at(dim) += i;
					map<cSimilarity_Profile, double>::iterator pm = ratio_map.find(sp);
					if ( pm == ratio_map.end()) {
						pm = ratio_map.insert(std::pair<cSimilarity_Profile, double>(sp, calc_r) ).first;
						update_monotonicity_map(&pm->first, similarity_map, msc_pool);
						++count;
						if ( count % count_base == 0 )
							std::cout << "Interpolation: " << count << " points have been interpolated." << std::endl;

					}
				}
				++q;
				++p;
			}
		}
		if ( tempcount == count )
			break;
		else
			tempcount = count;
	}
	return count;
}


#if 0
unsigned int extrapolation( map < cSimilarity_Profile, double, cSimilarity_Compare > & ratio_map,
							map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > & similarity_map,
							const cSimilarity_Profile & Max_Similarity, const cSimilarity_Profile & Min_Similarity ) {
	const double min_ratio = 0.00001;
	const double max_ratio = 100000;
	const unsigned int profile_size = ratio_map.begin()->first.size();
	if ( profile_size != Max_Similarity.size() )
		throw cException_Other("Extrapolation: dimension error -> Max Similarity.");
	if ( profile_size != Min_Similarity.size() )
		throw cException_Other("Extrapolation: dimension error -> Min Similarity.");

	std::cout << "Size of similarity map = " << similarity_map.size() << std::endl;

	//map < cSimilarity_With_Monotonicity_Dimension, monotonic_set > similarity_map;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::iterator psimmap;
	map < cSimilarity_With_Monotonicity_Dimension, monotonic_set >::const_iterator cpsimmap;

	vector < cMonotonic_Similarity_Compare > msc_pool( profile_size, 0 );
	for ( unsigned int i = 0; i < profile_size; ++i )
		msc_pool.at(i).reset_entry(i);

	/*
	// this will generate a list of monotonic vectors. ie. v(2,1,1) > v(1,1,1) > v(0,1,1) > delta;
	for ( map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator pratio = ratio_map.begin(); pratio !=ratio_map.end(); ++pratio ) {
		for ( unsigned int i = 0; i < profile_size ; ++i ) {
			cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, i);
			psimmap = similarity_map.find(smd);
			if ( psimmap == similarity_map.end() ) {
				monotonic_set ms( msc_pool.at(i) );
				ms.insert( &pratio->first);
				similarity_map.insert(std::pair < cSimilarity_With_Monotonicity_Dimension, monotonic_set > (smd, ms ) );
			}
		}

		map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator qratio = pratio;
		for ( ++qratio; qratio != ratio_map.end(); ++qratio ) {
			int monotonicity_check = is_monotonic <unsigned int> (pratio->first, qratio->first);
			if ( monotonicity_check != 0 ) {
				cSimilarity_With_Monotonicity_Dimension smd(&pratio->first, abs( monotonicity_check));
				similarity_map.find(smd)->second.insert(&qratio->first);
			}
		}
	}
	*/

	// now extrapolating.
	unsigned int count = 0;
	const unsigned int count_base = 10000;
	unsigned int temp_count = count;
	while ( true ) {
		for ( psimmap = similarity_map.begin(); psimmap != similarity_map.end(); ++ psimmap ) {
			const unsigned int dim = psimmap->first.get_monotonic_dimension();
			const monotonic_set & alias = psimmap->second;
			if ( alias.size() < 2 )
				continue;
			const unsigned int min_entry = Min_Similarity.at(dim);
			const unsigned int max_entry = Max_Similarity.at(dim);
			monotonic_set::const_iterator p = alias.begin();
			const unsigned int known_min_entry = (*p)->at(dim);
			p = alias.end();
			--p;
			const unsigned int known_max_entry = (*p)->at(dim);

			cSimilarity_Profile sp = **p;
			cSimilarity_Profile sp_1 = sp;
			cSimilarity_Profile sp_2 = sp;
			for ( unsigned int i = known_min_entry - 1; i >= min_entry; --i ) {
				if ( static_cast<int>(i) < 0 )
					break;
				sp.at(dim) = i;
				sp_1.at(dim) = i + 1;
				sp_2.at(dim) = i + 2;
				const double smaller = ratio_map.find(sp_1)->second;
				const double larger = ratio_map.find(sp_2)->second;
				double calc_r = smaller * smaller / larger;
				if ( calc_r > larger )
					calc_r = larger;
				if ( calc_r < min_ratio )
					calc_r = min_ratio;
				else if ( calc_r > max_ratio )
					calc_r = max_ratio;
				map<cSimilarity_Profile, double>::iterator pm = ratio_map.find(sp);
				if ( pm == ratio_map.end()) {
					pm = ratio_map.insert(std::pair<cSimilarity_Profile, double>(sp, calc_r) ).first;
					update_monotonicity_map(&pm->first, similarity_map, msc_pool);
					++count;
					if ( count % count_base == 0 )
						std::cout << "Extrapolation: " << count << " points have been extrapolated." << std::endl;
				}
			}

			for ( unsigned int i = known_max_entry + 1; i <= max_entry; ++i ) {
				sp.at(dim) = i;
				sp_1.at(dim) = i - 1;
				sp_2.at(dim) = i - 2;
				const double larger = ratio_map.find(sp_1)->second;
				const double smaller = ratio_map.find(sp_2)->second;
				double calc_r = larger * larger / smaller;
				if ( calc_r < smaller )
					calc_r = smaller;
				if ( calc_r < min_ratio )
					calc_r = min_ratio;
				else if ( calc_r > max_ratio )
					calc_r = max_ratio;
				map<cSimilarity_Profile, double>::iterator pm = ratio_map.find(sp);
				if ( pm == ratio_map.end()) {
					pm = ratio_map.insert(std::pair<cSimilarity_Profile, double>(sp, calc_r) ).first;
					update_monotonicity_map(&pm->first, similarity_map, msc_pool);
					++count;
					if ( count % count_base == 0 )
						std::cout << "Extrapolation: " << count << " points have been extrapolated." << std::endl;
				}
			}

		}
		if ( temp_count == count )
			break;
		else
			temp_count = count;
	}

	return count;
}
#endif

//=========================================================================================
/*
template <typename Bool_Functor>
unsigned int update_value_by_monotonicity(const cSimilarity_Profile * key, const double value,
					const map < const cSimilarity_Profile * , list< const cSimilarity_Profile *>, cSimilarity_Compare > & associated,
					map < cSimilarity_Profile, double, cSimilarity_Compare > & ratio_map, const Bool_Functor & func ) {
	const unsigned int prof_size = key->size();
	//std::cout << ">>>>>>>> Updating ";
	//for ( unsigned int i = 0; i < prof_size; ++i )
	//	std::cout << key->at(i) << ",";
	//std::cout << std::endl;

	unsigned int count = 0;
	map < cSimilarity_Profile, double, cSimilarity_Compare >::iterator p = ratio_map.find(*key);
	if ( p == ratio_map.end() )
		throw cException_Other("Cannot update value! Value not found! ");

	map < const cSimilarity_Profile * , list< const cSimilarity_Profile *>, cSimilarity_Compare >::const_iterator passo;
	// say func is less than, then if p->second < value, we set p->second = value;
	double new_value = value;
	if ( func( p->second, value ) ) {
		//std::cout << " from " << p->second << " to " << value << std::endl;
		p->second = value;
		++count;
	}
	else {
		new_value = p->second;
		//std::cout << " skipped because " << p->second << " > " << value << std::endl;
	}

	passo = associated.find(key);
	if ( passo == associated.end() )
		throw cException_Other("Cannot update value! Value not found! ");
	const list < const cSimilarity_Profile *> & alias = passo->second;
	for ( list< const cSimilarity_Profile *>::const_iterator q = alias.begin(); q != alias.end(); ++q )
		count += update_value_by_monotonicity( *q, new_value, associated, ratio_map, func);

	//std::cout << "count = "<<count << std::endl;
	//std::cout << " <<<<<<<<<< Ending update ";
	//for ( unsigned int i = 0; i < prof_size; ++i )
	//	std::cout << key->at(i) << ",";
	//std::cout << std::endl;
	return count;
}
*/

template <typename Bool_Functor>
double update_value_by_monotonicity(const cSimilarity_Profile * key,
					const map < const cSimilarity_Profile * , list< const cSimilarity_Profile *>, cSimilarity_Compare > & associated,
					map < const cSimilarity_Profile *, bool, cSimilarity_Compare > & check_map,
					map < cSimilarity_Profile, double, cSimilarity_Compare > & ratio_map, const Bool_Functor & func ) {
	//const unsigned int prof_size = key->size();
	//std::cout << ">>>>>>>> Updating ";
	//for ( unsigned int i = 0; i < prof_size; ++i )
	//	std::cout << key->at(i) << ",";
	//std::cout << std::endl;


	map < cSimilarity_Profile, double, cSimilarity_Compare >::iterator p = ratio_map.find(*key);
	if ( p == ratio_map.end() )
		throw cException_Other("Cannot update value! Value not found! ");
	double value = p->second;

	map < const cSimilarity_Profile * , list< const cSimilarity_Profile *>, cSimilarity_Compare >::const_iterator passo;

	passo = associated.find(key);
	if ( passo == associated.end() )
		throw cException_Other("Cannot update value! Value not found! ");
	const list < const cSimilarity_Profile *> & alias = passo->second;
	double temp = value;
	for ( list< const cSimilarity_Profile *>::const_iterator q = alias.begin(); q != alias.end(); ++q ) {
		map < const cSimilarity_Profile *, bool, cSimilarity_Compare >::iterator pcheck = check_map.find(*q);
		if ( pcheck == check_map.end() )
			throw cException_Other("Error: Check map is incomplete. ");
		if ( pcheck->second ) {
			map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator cp = ratio_map.find(**q);
			if ( cp == ratio_map.end() )
				throw cException_Other("Cannot update value! Value not found! ");
			temp = cp->second;
		}
		else {
			temp = update_value_by_monotonicity( *q, associated, check_map, ratio_map, func);
			pcheck->second = true;
		}
		if ( func ( value, temp ) )
			value = temp;
	}

	//std::cout << " <<<<<<<< Finished updating ";
	//for ( unsigned int i = 0; i < prof_size; ++i )
	//	std::cout << key->at(i) << ",";
	//std::cout << " set " << p->second << " = " << value << std::endl;

	p->second = value;


	return value;

}








void generate_full_similarity_profiles(list< cSimilarity_Profile> & sim, const unsigned int min_back, const unsigned int max_back ) {
	list < cSimilarity_Profile > res;
	for ( list < cSimilarity_Profile >::const_iterator p = sim.begin(); p!= sim.end(); ++p ) {
		for ( unsigned int i = min_back; i <= max_back; ++i ) {
			cSimilarity_Profile temp = *p;
			temp.push_back(i);
			res.push_back(temp);
		}
	}
	sim = res;
}


void check_similarity_range( const cSimilarity_Profile & to_check, const cSimilarity_Profile & max, const cSimilarity_Profile & min) {
	const unsigned int length = to_check.size();
	for ( unsigned int i = 0; i < length; ++i ) {
		if ( to_check.at(i) > max.at(i) )
			std::cout << "Similarity Error: Position " << i << " : " << to_check.at(i) << " > " << max.at(i) << std::endl;
		if ( to_check.at(i) < min.at(i) )
			std::cout << "Similarity Error: Position " << i << " : " << to_check.at(i) << " < " << min.at(i) << std::endl;
	}
}


void extrapolation_v2( map < cSimilarity_Profile, double, cSimilarity_Compare > & ratio_map,
							const cSimilarity_Profile & Max_Similarity, const cSimilarity_Profile & Min_Similarity,
							const vector <string> & attribute_names, const bool name_range_check ) {

	double impossible_ratio ;
	const double delta = 0.1;

	const unsigned int profile_size = ratio_map.begin()->first.size();
	if ( profile_size != Max_Similarity.size() )
		throw cException_Other("Extrapolation: dimension error -> Max Similarity.");
	if ( profile_size != Min_Similarity.size() )
		throw cException_Other("Extrapolation: dimension error -> Min Similarity.");


	cSimilarity_Profile temp;
	list < cSimilarity_Profile > profile_list(1, temp);
	for ( unsigned int i = 0; i < profile_size; ++i ) {
		generate_full_similarity_profiles(profile_list, Min_Similarity.at(i), Max_Similarity.at(i));
	}
	double min_ratio = 100000; // will be updated in the future.
	double max_ratio = 0.00001;
	for ( map < cSimilarity_Profile, double, cSimilarity_Compare >::const_iterator p = ratio_map.begin(); p != ratio_map.end(); ++p ) {
		max_ratio = max_val<double> ( max_ratio, p->second);
		min_ratio = min_val<double>(min_ratio, p->second);
	}

	// first, descending direction.
	impossible_ratio = max_ratio + delta ;
	std::cout << "Full similarity profile size = " << profile_list.size() << std::endl;
	for ( list < cSimilarity_Profile >::const_iterator p = profile_list.begin(); p != profile_list.end(); ++p ) {
		if ( ratio_map.find(*p) == ratio_map.end() )
			ratio_map.insert(std::pair<cSimilarity_Profile, double>(*p, impossible_ratio ));
	}

	map < const cSimilarity_Profile * , list< const cSimilarity_Profile *>, cSimilarity_Compare >  associated;
	map < const cSimilarity_Profile *, bool, cSimilarity_Compare > check_map;
	map < cSimilarity_Profile, double, cSimilarity_Compare >::iterator pratio;

	for ( pratio = ratio_map.begin(); pratio != ratio_map.end(); ++pratio ) {
		list < const cSimilarity_Profile* > plist;
		for ( unsigned int i = 0; i < profile_size; ++i ) {
			if ( pratio->first.at(i) == Max_Similarity.at(i) )
				continue;
			cSimilarity_Profile temp = pratio->first;
			temp.at(i) += 1;
			const cSimilarity_Profile * pp = & ratio_map.find(temp)->first;
			plist.push_back(pp);
		}
		associated.insert(std::pair<const cSimilarity_Profile*, list< const cSimilarity_Profile *> >(& pratio->first, plist));
		bool is_known = ( pratio->second != impossible_ratio );
		check_map.insert(std::pair < const cSimilarity_Profile *, bool > (& pratio->first, is_known));
	}

	update_value_by_monotonicity( & ratio_map.begin()->first, associated, check_map, ratio_map, std::greater<double>() );


	//second, ascending direction.

	for ( map < const cSimilarity_Profile *, bool, cSimilarity_Compare >::iterator p = check_map.begin(); p != check_map.end(); ++p ) {
		pratio = ratio_map.find(* (p->first));
		if ( pratio == ratio_map.end())
			throw cException_Other("Error: ratio not found.");
		p->second = ( pratio->second != impossible_ratio);
	}

	for ( pratio = ratio_map.begin(); pratio != ratio_map.end(); ++pratio ) {
		if ( pratio->second == impossible_ratio )
			pratio->second = min_ratio - delta;
	}
	impossible_ratio = min_ratio - delta;
	associated.clear();


	for ( pratio = ratio_map.begin(); pratio != ratio_map.end(); ++pratio ) {
		list < const cSimilarity_Profile* > plist;
		for ( unsigned int i = 0; i < profile_size; ++i ) {
			if ( pratio->first.at(i) == Min_Similarity.at(i) )
				continue;
			cSimilarity_Profile temp = pratio->first;
			temp.at(i) -= 1;
			const cSimilarity_Profile * pp = & ratio_map.find(temp)->first;
			plist.push_back(pp);
		}
		associated.insert(std::pair<const cSimilarity_Profile*, list< const cSimilarity_Profile *> >(& pratio->first, plist));
	}
	pratio = ratio_map.end();
	--pratio;
	update_value_by_monotonicity( & pratio->first, associated, check_map, ratio_map, std::less<double>() );



	for ( pratio = ratio_map.begin(); pratio != ratio_map.end(); ++pratio ) {
		if ( pratio->second == impossible_ratio ) {
			//std::cout << "----------->>>>>> WARNING: SIMILARITY PROFILE EXCEEDS LIMIT. CHECK THE UPPER, LOWER LIMITS AND THE SCORING CRITERIA!" << std::endl;
			check_similarity_range(pratio->first, Max_Similarity, Min_Similarity);
			pratio->second = min_ratio;
			//throw cException_Other("Extrapolation error.");
		}
	}


	// put upper_bound restrictions here.
	const double midname_upper = 1.0;
	const double lastname_upper = 1.0;
	const double firstname_upper = 1.0;

	bool midname_range_check = name_range_check;
	const unsigned int midname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cMiddlename::static_get_class_name()));
	if ( midname_location == attribute_names.size() )
		midname_range_check = false;

	if ( midname_range_check )
		std::cout << "Middle name ratio adjustment activated." << std::endl;


	bool firstname_range_check = name_range_check;
	const unsigned int firstname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cFirstname::static_get_class_name()));
	if ( firstname_location == attribute_names.size() )
		firstname_range_check = false;

	if ( firstname_range_check )
		std::cout << "First name ratio adjustment activated." << std::endl;

	bool lastname_range_check = name_range_check;
	const unsigned int lastname_location = std::distance( attribute_names.begin(),
											std::find( attribute_names.begin(), attribute_names.end(), cLastname::static_get_class_name()));
	if ( lastname_location == attribute_names.size() )
		lastname_range_check = false;

	if ( lastname_range_check )
		std::cout << "Last name ratio adjustment activated." << std::endl;

	for ( pratio = ratio_map.begin(); pratio != ratio_map.end(); ++pratio ) {
		if ( midname_range_check && pratio->first.at(midname_location) == 0 ) {
			if ( pratio->second > midname_upper )
				pratio->second = midname_upper;
		}
		if ( lastname_range_check && pratio->first.at(lastname_location) == 0 ) {
			if ( pratio->second > lastname_upper )
				pratio->second = lastname_upper;
		}
		if ( firstname_range_check && pratio->first.at(firstname_location) == 0 ) {
			if ( pratio->second > firstname_upper )
				pratio->second = firstname_upper;
		}
	}

}











void cRatioComponent::smooth() {
	std::cout << "Starting data smoothing..." << std::endl;
	smoothing( ratio_map, similarity_map, x_counts, m_counts, this->get_attrib_names(), should_do_name_range_check );
	std::cout << "Smoothing done." << std::endl;
}

void cRatios::inter_extra_polation( const cSimilarity_Profile & max, const cSimilarity_Profile & min) {
	unsigned int total_nodes = 1;
	for ( unsigned int i = 0; i < max.size(); ++i )
		total_nodes *= ( max.at(i) - min.at(i) + 1 );

	std::cout << " Total number of points should be " << total_nodes << std::endl;

	unsigned int inter = 0;
	unsigned int interpolated_nodes_increment;
	do {
		interpolated_nodes_increment = interpolation(final_ratios , similarity_map );
		std::cout << interpolated_nodes_increment << " nodes have been interpolated. " << std::endl;
		//extrapolated_nodes_increment = extrapolation ( final_ratios, similarity_map, max, min);
		extrapolation_v2 ( final_ratios, max, min, this->get_attrib_names(), should_do_name_range_check);
		inter += interpolated_nodes_increment;
	}
	while ( interpolated_nodes_increment != 0 );

	std::cout << "Interpolation and Extrapolation done. " << std::endl
			<< inter << " nodes have been interpolated." << std::endl;
}

void cRatios::smooth() {
	std::cout << "Starting ratios smoothing..." << std::endl;
	smoothing( final_ratios, similarity_map, x_counts, m_counts, this->get_attrib_names(), should_do_name_range_check);
	std::cout << "Ratios smoothing done. " << std::endl;
}

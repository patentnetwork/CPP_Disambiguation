/*
 * DisambigClusterSmoothing.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: ysun
 */

#include "DisambigRatios.h"
#include "DisambigEngine.h"

#include <limits>
#include <climits>
#include <sstream>
#include <cmath>
#include <algorithm>

#include <ilcplex/ilocplex.h>


static const bool should_do_name_range_check = true;

void smoothing_inter_extrapolation_cplex(map < cSimilarity_Profile, double,  cSimilarity_Compare  >& ratio_map,
				const cSimilarity_Profile & min_sp, const cSimilarity_Profile & max_sp,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & x_counts,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & m_counts,
				const vector < string > & attribute_names, const bool name_range_check, const bool backup_quadprog ) ;


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





unsigned int sp2index ( const cSimilarity_Profile & sp, const cSimilarity_Profile & min_sp, const cSimilarity_Profile & max_sp ) {
	if ( sp.size() != min_sp.size() )
		throw cException_Other("Convertion error in smoothing.");
	unsigned int index = 0;
	for ( unsigned int i = 0; i < sp.size(); ++i ) {
		if ( sp.at(i) > max_sp.at(i) )
			throw cException_Other("Index range error. > max.");
		if ( sp.at(i) < min_sp.at(i) )
			throw cException_Other("Index range error. < min.");

		index *= max_sp.at(i) - min_sp.at(i) + 1;
		index += sp.at(i) - min_sp.at(i);
	}
	return index;
}

cSimilarity_Profile index2sp ( unsigned int index, const cSimilarity_Profile & min_sp, const cSimilarity_Profile & max_sp ) {
	static const unsigned int ulimit = 0 - 1;
	cSimilarity_Profile sp ( min_sp );
	for ( unsigned int i = min_sp.size() - 1 ; i  != ulimit ; --i ) {
		if ( max_sp.at(i) < min_sp.at(i))
			throw cException_Other("Index range error.  max < min.");
		unsigned int t = max_sp.at(i) - min_sp.at(i) + 1 ;
		sp.at(i) += index % t;
		index /= t;
	}
	if ( index >=1 )
		throw cException_Other("Index range error.");
	return sp;
}

void cRatioComponent::smooth() {
	std::cout << "Starting data smoothing..." << std::endl;
	std::cout << "This step is skipped for cRatioComponent objects." << std::endl;
	return;

	map < cSimilarity_Profile, double,  cSimilarity_Compare  > temp_map = ratio_map;
	//smoothing( ratio_map, similarity_map, x_counts, m_counts, this->get_attrib_names(), should_do_name_range_check );
	const cSimilarity_Profile max = get_max_similarity (this->attrib_names);
	const cSimilarity_Profile min ( max.size(), 0 );
	smoothing_inter_extrapolation_cplex(temp_map, min, max, x_counts, m_counts,
			this->get_attrib_names(), should_do_name_range_check, true);
	for ( map < cSimilarity_Profile, double,  cSimilarity_Compare  >:: iterator p = ratio_map.begin();
			p != ratio_map.end(); ++p ) {
		p->second = temp_map.find(p->first)->second;
	}
	std::cout << "Smoothing done." << std::endl;
}


void cRatios::smooth() {
	std::cout << "Starting ratios smoothing..." << std::endl;
	//smoothing( final_ratios, similarity_map, x_counts, m_counts, this->get_attrib_names(), should_do_name_range_check);
	const cSimilarity_Profile max = get_max_similarity (this->attrib_names);
	const cSimilarity_Profile min ( max.size(), 0 );
	smoothing_inter_extrapolation_cplex(this->final_ratios, min, max, x_counts, m_counts,
			this->get_attrib_names(), should_do_name_range_check, false);

	std::cout << "Ratios smoothing done. " << std::endl;
}




vector < cSimilarity_Profile > find_lesser_neighbour( const cSimilarity_Profile & sp, const cSimilarity_Profile & min_sp	) {
	vector < cSimilarity_Profile > vs;
	for ( unsigned int i = 0; i < sp.size(); ++i ) {
		if ( min_sp.at(i) > sp.at(i) )
			throw cException_Other("Similarity input error: sp < min.");
		else if ( min_sp.at(i) == sp.at(i))
			continue;
		else {
			cSimilarity_Profile temps (sp);
			temps.at(i) -= 1;
			vs.push_back(temps);
		}
	}
	return vs;
}

vector < cSimilarity_Profile > find_greater_neighbour( const cSimilarity_Profile & sp, const cSimilarity_Profile & max_sp	) {
	vector < cSimilarity_Profile > vs;
	for ( unsigned int i = 0; i < sp.size(); ++i ) {
		if ( max_sp.at(i) < sp.at(i) )
			throw cException_Other("Similarity input error: sp > max.");
		else if ( max_sp.at(i) == sp.at(i))
			continue;
		else {
			cSimilarity_Profile temps (sp);
			temps.at(i) += 1;
			vs.push_back(temps);
		}
	}
	return vs;
}

vector < std::pair < cSimilarity_Profile, cSimilarity_Profile> > find_neighbours( const cSimilarity_Profile & sp,
		const cSimilarity_Profile & min_sp, const cSimilarity_Profile & max_sp	) {
	vector < std::pair < cSimilarity_Profile, cSimilarity_Profile> > vs;
	for ( unsigned int i = 0; i < sp.size(); ++i ) {
		if ( min_sp.at(i) > sp.at(i) )
			throw cException_Other("Similarity input error: sp < min.");
		else if ( max_sp.at(i) < sp.at(i) )
			throw cException_Other("Similarity input error: sp > max.");
		else if ( min_sp.at(i) == sp.at(i) || max_sp.at(i) == sp.at(i) )
			continue;
		else {
			cSimilarity_Profile templess (sp), tempgreat(sp);
			templess.at(i) -= 1;
			tempgreat.at(i) += 1;
			vs.push_back(std::pair<cSimilarity_Profile, cSimilarity_Profile>(templess, tempgreat));
		}
	}
	return vs;
}


double get_weight ( const unsigned int x_count, const unsigned int m_count ) {
	return 1.0 * x_count + 1.0 * m_count;
}



//================================================================

void smoothing_inter_extrapolation_cplex(map < cSimilarity_Profile, double,  cSimilarity_Compare  >& ratio_map,
				const cSimilarity_Profile & min_sp, const cSimilarity_Profile & max_sp,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & x_counts,
				const map < cSimilarity_Profile, unsigned int, cSimilarity_Compare  > & m_counts,
				const vector < string > & attribute_names, const bool name_range_check, const bool backup_quadprog ) {


	if ( x_counts.size() != m_counts.size() )
		throw cException_Other("x_counts and m_counts are not of the same size");
	if ( x_counts.size() != ratio_map.size() )
		throw cException_Other("x_counts and ratio_map are not of the same size");
	//first, build all the possible similarity profiles.
	if ( min_sp.size() != max_sp.size() )
		throw cException_Other("Minimum similarity profile and Maximum similarity profile are not consistent.");
	unsigned int total_nodes = 1;


	const unsigned int overflow_check = 0 - 1;
	for ( unsigned int i = 0; i < min_sp.size(); ++i ) {
		if ( max_sp.at(i) < min_sp.at(i) )
			throw cException_Other("Entry error: max < min.");
		unsigned int t = max_sp.at(i) - min_sp.at(i) + 1;
		if ( total_nodes >= overflow_check / t )
			throw cException_Other ("Size of all the similarity profiles exceeds the allowed limit ( unsigned int ).");
		else {
			total_nodes *= t;
		}
	}

	unsigned int total_possible_inequality = 0;
	unsigned int total_equality = 0;
	for ( unsigned int i = 0; i < min_sp.size(); ++i ) {
		unsigned int t = max_sp.at(i) - min_sp.at(i) + 1;
		unsigned int temp = total_nodes - total_nodes/t;

		if ( total_possible_inequality > overflow_check - temp )
			throw cException_Other ("Size of all inequality exceeds the allowed limit ( unsigned int ).");
		else {
			total_possible_inequality += temp;
			if ( temp ) {
				unsigned int temp2 = temp - total_nodes/t;
				total_equality += temp2;
			}
		}
	}



	std::cout << "There are " << total_nodes << " similarity profiles, "
			<< total_equality << " equalities and "
			<< total_possible_inequality << " inequalities in all." << std::endl;

	std::cout << "Starting Quadratic Programming. ( Take the logarithm ) ..." << std::endl;

	//==========================
	///USING cplex NOW.


	IloEnv env;
	const double xmin = log(1e-6);
	const double xmax = log(1e6);
	const double equ_delta = log (5);
	try {
		IloModel model(env);
		IloNumVarArray var(env);
		IloRangeArray con(env);

		//configuring variables
		for ( unsigned int j = 0 ; j < total_nodes; ++j )
			var.add ( IloNumVar(env, xmin, xmax, ILOFLOAT));

		//configuring the object
		IloExpr target(env);
		for ( map < cSimilarity_Profile, double,  cSimilarity_Compare  >::const_iterator cpm = ratio_map.begin();
				cpm != ratio_map.end(); ++cpm ) {
			const double log_val = log ( cpm->second);
			const double wt =  get_weight( x_counts.find (cpm->first)->second , m_counts.find (cpm->first)->second ) ;
			const unsigned int k = sp2index( cpm->first, min_sp, max_sp);
			const double quad_coef = wt;
			target += quad_coef * var[k] * var[k];
			const double line_coef = ( - 2.0 ) * log_val * wt;
			target += line_coef * var[k];
		}
		IloObjective obj ( env, target, IloObjective::Minimize);
		model.add(obj);

		//configuring constraints

		for ( unsigned int i = 0 ; i < total_nodes; ++i ) {
			const cSimilarity_Profile the_sp = index2sp(i, min_sp, max_sp );

			//equality constraints
			vector < std::pair < cSimilarity_Profile, cSimilarity_Profile> > neighbours
				= find_neighbours( the_sp, min_sp, max_sp );
			for ( vector < std::pair < cSimilarity_Profile, cSimilarity_Profile> >::const_iterator ci = neighbours.begin();
					ci != neighbours.end(); ++ci ) {

				const unsigned int lesser = sp2index( ci->first, min_sp, max_sp );
				const unsigned int greater = sp2index ( ci->second, min_sp, max_sp);
				if ( lesser >= i )
					throw cException_Other("Index sequence error. less > this.");
				if ( greater <= i )
					throw cException_Other("Index sequence error. greater < this.");

				IloRange rg ( env, - equ_delta, 2.0 * var[i] - var[lesser] - var[greater] , equ_delta );
				con.add (  rg );

			}

			//inequality constraints
			vector <cSimilarity_Profile> greater_neighbours ( find_greater_neighbour ( the_sp, max_sp));
			for ( vector < cSimilarity_Profile >::const_iterator cc = greater_neighbours.begin();
					cc != greater_neighbours.end(); ++cc ) {
				const unsigned int gg = sp2index ( *cc, min_sp, max_sp );
				if ( gg <= i )
					throw cException_Other("Index sequence error. greater < this.");

				con.add ( var[gg] - var[i] >= 0);
			}
		}
		model.add(con);

		// solve the model
		IloCplex cpl(model);
		cpl.setOut(env.getNullStream());


		if ( ! cpl.solve() ) {
			env.error() << "Failed to optimize LP" << std::endl;
			throw(-1);
		}

	    IloNumArray result(env);
	    env.out() << "Solution status = " << cpl.getStatus() << std::endl;
	    cpl.getValues(result, var);
	    //cpl.exportModel("ratios.lp");

	    //save the results

	    for ( unsigned int i = 0; i < total_nodes; ++i ) {
	    	cSimilarity_Profile sp = index2sp(i, min_sp, max_sp);
	    	const double log_r = result[i];
	    	ratio_map[sp] = exp(log_r);
	    }

		target.end();

	}
	catch (IloException& e) {
	      std::cerr << "Concert exception caught: " << e << std::endl;
	      throw;
	}
	catch ( int ) {
		throw;
	}
	env.end();


}

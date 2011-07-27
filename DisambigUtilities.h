/*
 * DisambigUtilities.h
 *
 *  Created on: May 11, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGUTILITIES_H_
#define DISAMBIGUTILITIES_H_
#include <vector>
#include <list>
#include <string>
#include <map>
using std::vector;
using std::list;
using std::string;
using std::map;

class cRecord;
class cString_Manipulator;
class cCluster_Set;
class cRatios;


bool make_changable_training_sets_by_patent(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
bool make_stable_training_sets_by_personal ( const list <cRecord> & all_records, const unsigned int limit, const vector <string> & training_filenames);
bool make_changable_training_sets_by_assignee(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
int unique_inventors_per_period ( unsigned int starting_year, unsigned int interval, const char * wholedatabase, const char * disambigresult, const char * outputfile);
void one_step_prostprocess( const list < cRecord > & all_records, const char * last_disambig_result, const char * outputfile);
string remove_headtail_space( const string & s );
void out_of_cluster_density( const cCluster_Set & upper, const cCluster_Set & lower, const cRatios & ratio, std::ofstream & ofile );


#endif /* DISAMBIGUTILITIES_H_ */

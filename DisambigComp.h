/*
 * DisambigComp.h
 *
 *  Created on: Dec 9, 2010
 *      Author: ysun
 */

#ifndef DISAMBIGCOMP_H_
#define DISAMBIGCOMP_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;

const unsigned int Jaro_Wrinkler_Max = 5;

template <typename Tp>
inline const Tp& max_val(const Tp& arg1, const Tp &arg2) {
	return ( arg1 < arg2 )? arg2 : arg1;
}

template <typename Tp>
inline const Tp& min_val(const Tp& arg1, const Tp &arg2) {
	return ( arg1 < arg2 )? arg1 : arg2;
}

template <typename Tp, typename Functor>
vector <Tp> Longest_Common_Subsequence(const vector <Tp> & s1, const vector <Tp> &s2, const Functor & func);


char * extract_initials(char * dest, const char * source) ;
int nospacecmp(const char* str1, const char* str2);
int jwcmp(const string & str1, const string& str2);
int midnamecmp(const string & str1, const string & str2 );
int countrycmp(const string & country1, const string & country2 );
int streetcmp(const string& inputstreet1, const string& inputstreet2);
int latloncmp(const string & inputlat1, const string & inputlon1, const string & inputlat2, const string & inputlon2 );
int classcmp(const string &class1, const string& class2 );
int coauthorcmp(const string &coauthor1, const string& coauthor2 );
int asgcmp(const string & asg1, const string & asg2, const map<string, std::pair<string, unsigned int> > * const asg_table_pointer);
int asgcmp ( const string & s1, const string &s2) ;
int asgcmp_to_test(const vector <string> & asg1, const vector <string> & asg2,
		   const map<string, std::pair<string, unsigned int> > * const asg_table_pointer);
int name_compare( const string & s1, const string & s2, const unsigned int prev, const unsigned int cur);


class cSentence_JWComparator {
private:
	const double threshold;
public:
	bool operator() (const string * s1, const string * s2) const;
	explicit cSentence_JWComparator(const double inputthreshold): threshold(inputthreshold){};
};


template < typename Iter1, typename Iter2 >
unsigned int num_common_elements ( const Iter1 & p1begin, const Iter1 & p1e ,
									const Iter2 & p2begin, const Iter2 & p2e, const unsigned int max) {// containers must be sorted before use.
	// it has to be a sorted version container, like set, or sorted vector or list
	unsigned int cnt = 0;
	Iter1 p1b = p1begin;
	Iter2 p2b = p2begin;
	while ( p1b != p1e && p2b != p2e ) {
		if ( *p1b < *p2b ) {
			++p1b;
		}
		else if ( *p2b  < *p1b  ) {
			++p2b;
		}
		else {
			++cnt;
			++p1b;
			++p2b;
		}

		if ( cnt == max && max != 0 )
			break;
	}
	return cnt;
}

#endif /* DISAMBIGCOMP_H_ */

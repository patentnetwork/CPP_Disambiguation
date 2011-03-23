/*
 * DisambigComp.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: ysun
 */

#include "DisambigComp.h"
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <list>
#include <functional>
#include <stdexcept>

extern "C" {
	#include "strcmp95.h"
}

using std::list;

//this function is to get the incontinuous longest common subsequence of two vectors.
//for example, the mid name comparision uses the function | or the following continuous function.
template <typename Tp, typename Functor>
vector <Tp> Longest_Common_Subsequence_Incontinuous(const vector <Tp> & s1, const vector <Tp> &s2, const Functor & func)
{
    
	static const vector < Tp > emptyresult;
    if(s1.empty()||s2.empty())
        return emptyresult;
    const int m=s1.size()+1;
    const int n=s2.size()+1;
	vector <int> row(n, 0);
	vector < vector <int> > lcs(m, row);
    //int lcs[100][100];
    int i,j;
    for(i=0;i<m;i++)
        for(j=0;j<n;j++)
            lcs[i][j]=0;
	
	
    for(i=1;i<m;i++) {
        for(j=1;j<n;j++)
        {
            //if(s1[i-1]==s2[j-1])
			if ( func( s1[i-1], s2[j-1] ) )
                lcs[i][j]=lcs[i-1][j-1]+1;
            else
                lcs[i][j]=lcs[i-1][j]>=lcs[i][j-1]?lcs[i-1][j]:lcs[i][j-1];//get the upper or lefter max value
        }
    }
	i=m-2;
	j=n-2;
	list < Tp > ss;
	while(i!=-1 && j!=-1)
	{
		//if(s1[i]==s2[j])
		if ( func( s1[i], s2[j] ) )
		{
			ss.push_front(s1[i]);
			i--;
			j--;
		}
		else
		{
			if(lcs[i+1][j+1]==lcs[i][j])
			{
				i--;
				j--;
			}
			else
			{
				if(lcs[i][j+1]>=lcs[i+1][j])
					i--;
				else
					j--;
			}
		}
	}
	
    vector < Tp > ans (ss.begin(), ss.end());
    return ans;
}

template <typename Tp, typename Functor>
vector <Tp> Longest_Common_Subsequence_Continuous(const vector <Tp> & s1, const vector <Tp> &s2, const Functor & func) {
	static const vector < Tp > emptyresult;
    if (s1.empty() || s2.empty() )
        return emptyresult;

    const int m = s1.size();
    const int n = s2.size();

    vector < int > c(m, 0);
    int max, maxj,i,j;
    maxj = 0 ;
    max = 0;
    for( i = 0; i < n ; ++i )   {
		  for( j = m - 1 ; j >= 0 ; --j )   {
			  if( func (s2[i], s1[j] ) )   {
				  if ( i == 0 || j == 0 )
					  c[j] = 1;
				  else
					  c[j] = c[j-1] + 1;
			  }
			  else
				  c[j]=0;
			  if( c[j] > max )   {
				  max = c[j];
				  maxj = j;
			  }
		  }
    }

    if( max == 0 )
    	return emptyresult;
    vector <Tp> ss ( emptyresult);
	for( j = maxj - max + 1; j <= maxj ; ++j )
		ss.push_back( s1[j] );
	return ss;
}
















inline bool cSentence_JWComparator:: operator()(const string * ps1, const string * ps2) const {
	const double compres = strcmp95_modified(ps1->c_str(), ps2->c_str());
	return compres > threshold;
};




char * extract_initials(char * dest, const char * source) {
	if ( source == NULL || dest == NULL )
		return NULL;
	char * ret = dest;
	static const char delim = ' ';
	while ( *source != '\0') {
		while ( *source == delim )
			++source;
		*dest++ = *source;
		while ( *source != delim && *source != '\0' )
			++source;
	}
	*dest = '\0';
	return ret;
};

int nospacecmp(const char* str1, const char* str2){
    const char *c1, *c2;
    const char delim = ' ';
    for(c1 = str1, c2=str2; (*c1 != '\0') && (*c2 != '\0'); ++c1, ++c2 ){
        while (*c1 == delim ) ++c1;
        while (*c2 == delim ) ++c2;
        if(*c1!=*c2){
            return -1 + (*c1 > *c2)*2;
        }
    }
    return  ( *c1!='\0' ) - ( *c2!='\0' );
}

int jwcmp_old(const string & str1, const string& str2){
    const char *delim= " ";
    const unsigned int delim_size = strlen(delim);
    const double threshold  = 0.95;

    int tok_len1, tok_len2, num_tok1, num_tok2;
    double tok_score, score = 0;
    if( 0 == str1.size() || 0 == str2.size() )
        //missing!
        return cJWscore::JWMISSING;

    if( ( str1 == str2 ) && 0==nospacecmp( str1.c_str(), str2.c_str() )){
        return cJWscore::JW100; //JW100
    }
    size_t pos1, prev_pos1, pos2, prev_pos2;
    pos1 = prev_pos1 = 0;
    num_tok1 = 0;
	do {
		tok_score = 0;
		pos1 = str1.find(delim, prev_pos1);
		string temp1 = str1.substr(prev_pos1, pos1 - prev_pos1);
		tok_len1 = temp1.size();
		num_tok1 += (tok_len1 > 1);

		pos2 = prev_pos2 = 0;
		num_tok2 = 0;
		do {
			pos2 = str2.find(delim, prev_pos2);
			string temp2 = str2.substr(prev_pos2, pos2 - prev_pos2);
			tok_len2 = temp2.size();
			num_tok2 += (tok_len2 > 1);
			tok_score = max_val<int>(tok_score,
					((min_val<int>(tok_len1, tok_len2) <= 1) ? 0 : strcmp95_modified(temp1.c_str(), temp2.c_str())));

			prev_pos2 = pos2 + delim_size;
		} while ( pos2!= string::npos);
		score += (tok_score > threshold);

		prev_pos1 = pos1 + delim_size;
	} while ( pos1!= string::npos);

	int min_num_tok = min_val<int>(num_tok1, num_tok2);
	double myres = ( min_num_tok == 0) ? 0 : score/min_num_tok;
	int is_same_len = (num_tok1 == num_tok2) ? 1 : 0;
	return( 2*(myres >= 0.33) + (myres >= 0.66) + (myres > 0.99) + (myres > 0.99 && min_num_tok >= 2) + (myres > 0.99 && is_same_len));

}

int jwcmp(const string & str1, const string& str2) {
	double cmpres = strcmp95_modified(str1.c_str(), str2.c_str());
	register int score = 0;
	if ( cmpres > 0.7 )
		++score;
	if ( cmpres > 0.8 )
		++score;
	if ( cmpres > 0.9 )
		++score;
	if ( cmpres > 0.95 )
		++score;
	if ( cmpres > 0.99 )
		++score;
	
	return score;
}

int midnamecmp_old(const string & str1, const string & str2 ){

	const char * delim = " ";
	const unsigned int delim_size = strlen(delim);
	int num_names_1 = 0, num_names_2 = 0;
	double matches = 0;
	size_t pos1, prev_pos1, pos2, prev_pos2;
	pos1 = prev_pos1 = 0;
	while ( ( pos1 = str1.find(delim, prev_pos1)) != string::npos ) {
		++ num_names_1;
		pos2 = prev_pos2 = 0;
		while ( ( pos2 = str2.find(delim, prev_pos2)) != string::npos ) {
			++num_names_2;
			if ( str1.at(pos1 + delim_size) == str2.at(pos2 + delim_size) )
				matches += 1;
			prev_pos2 = pos2 + delim_size;
		}
		prev_pos1 = pos1 + delim_size;
	}

	int min_num =  min_val<int>(num_names_1, num_names_2);
	int missing = ( min_num == 0 )? 1:0 ;
	double raw = missing? 0: matches/min_num ;
	return (missing + 2*(raw > 0.33) + (raw > 0.67) + (raw > 0.99));
}

int midnamecmp_old2(const string & str1, const string & str2 ){
	
	static std::equal_to<char> char_compare;
	/*
	const char * delim = " ";
	const unsigned int delim_size = strlen(delim);

	size_t pos, prev_pos;
	pos = prev_pos = 0;
	vector < char > vec1, vec2;
	while ( ( pos = str1.find(delim, prev_pos)) != string::npos ) {
		prev_pos = pos + delim_size;
		vec1.push_back(str1.at(prev_pos));
	}
	pos = prev_pos = 0;
	while ( ( pos = str2.find(delim, prev_pos)) != string::npos ) {
		prev_pos = pos + delim_size;
		vec2.push_back(str2.at(prev_pos));
	}
	 */
	const vector < char > vec1(str1.begin(), str1.end() );
	const vector < char > vec2(str2.begin(), str2.end() );
	
	if ( vec1.empty() && vec2.empty() )
		return 2;

	if ( vec1.empty() || vec2.empty() )
		return 1;
	
	int score;
	const int matches = Longest_Common_Subsequence_Continuous<char, std::equal_to<char> >(vec1, vec2, char_compare).size();

	if ( matches == min_val<int>(str1.size(), str2.size() ) )
		score = 3;
	else
		score = 0;
		
	return score;
}

int midnamecmp ( const string & s1, const string & s2) {
	if ( s1.empty() && s2.empty() )
		return 2;

	if ( s1.empty() || s2.empty() )
		return 1;

	const char * p1 = s1.c_str();
	const char * p2 = s2.c_str();

	while ( *p1 != '\0' && *p2 != '\0') {
		if ( *p1++ != * p2++)
			return 0;
	}
	return 3;
}



int distcmp(const string & inputlat1, const string & inputlon1, const string & inputctry1, const char * inputstreet1,
			const string & inputlat2, const string & inputlon2, const string & inputctry2, const char * inputstreet2 ){
	/*
//    printf("DISTCOMP:\n");
    // Extreme points of contiguous 48
    double northernmost=4938;
    double southernmost=2454;
    double easternmost=-6695;
    double westernmost=-12451;

    double dist;
    int streetmatch;

    latlon *ll1 = (latlon*)latlon1, *ll2 = (latlon*)latlon2;
    int missing = ((abs(ll1->lat)<0.0001 && abs(ll1->lon)< 0.0001) || (abs(ll2->lat)<0.0001 && abs(ll2->lon)<0.0001));
    int in_us = ll1->lat < northernmost && ll1->lat > southernmost &&
                ll1->lon < easternmost && ll1->lon > westernmost &&
                ll2->lat < northernmost && ll2->lat > southernmost &&
                ll2->lon < easternmost && ll1->lon > westernmost;
    size = size;

    dist = 3963.0 * acos(sin(ll1->lat/DEG2RAD) * sin(ll2->lat/DEG2RAD) + cos(ll1->lat/DEG2RAD) * cos(ll2->lat/DEG2RAD) *  cos(ll2->lon/DEG2RAD -ll1->lon/DEG2RAD));
    //if(dist > 1){
        //printf("\targs: %f, %f ; %f, %f\n", ll1->lat, ll1->lon, ll2->lat, ll2->lon);
        //printf("\traw: %f\n", dist);
    //}
    streetmatch = (((latlon*)latlon1)->street!=NULL) &&
                  (((latlon*)latlon2)->street!=NULL) &&
                  (((latlon*)latlon1)->street[0] != '\0')&&
                  (((latlon*)latlon2)->street[0] != '\0')&&
                  (strcmp(((latlon*)latlon1)->street, ((latlon*)latlon2)->street)==0);

    return missing +
           in_us ?
               2*(dist < 100) + (dist < 75) + (dist < 50) + 2*(dist < 10) +
                   ((dist < 1) &&  streetmatch):
               2*(dist < 100) + (dist < 75) + (dist < 50) + (dist < 10);
    */

	static const double R = 3963.0; //radius of the earth is 6378.1km = 3963 miles
	static const double DEG2RAD = 5729.58;
    static const double northernmost = 4938;
    static const double southernmost = 2454;
    static const double easternmost = -6695;
    static const double westernmost = -12451;

    const double lat1 = atof(inputlat1.c_str());
    const double lon1 = atof(inputlon1.c_str());
    const double lat2 = atof(inputlat2.c_str());
    const double lon2 = atof(inputlon2.c_str());

    const double missing_val = 0.0001;
    int missing = ( ( fabs(lat1) < missing_val && fabs(lon1) < missing_val ) ||
    				( fabs(lat2) < missing_val && fabs(lon2) < missing_val) ) ? 1 : 0;
    int in_us = ( 	lat1 < northernmost && lat1 > southernmost &&
    				lon1 < easternmost && lon1 > westernmost &&
    				lat2 < northernmost && lat2 > southernmost &&
    				lon2 < easternmost && lon2 > westernmost ) ? 1 : 0;

    const double radlat1 = lat1/DEG2RAD;
    const double radlon1 = lon1/DEG2RAD;
    const double radlat2 = lat2/DEG2RAD;
    const double radlon2 = lon2/DEG2RAD;

    const double cos_lat1 = cos(radlat1);
    const double cos_lat2 = cos(radlat2);
    const double cos_lon1 = cos(radlon1);
    const double cos_lon2 = cos(radlon2);
    const double sin_lon1 = sin(radlon1);
    const double sin_lon2 = sin(radlon2);


    // R=radius, theta = colatitude, phi = longitude
    // Spherical coordinate -> Cartesian coordinate:
    // x=R*sin(theta)*cos(phi) = R*cos(latitude)*cos(longitude)
    // y = R*sin(theta)*sin(phi) = R*cos(latitude)*sin(longitude)
    // z = R*cos(phi) = R * cos(longitude)
    // Cartesion distance = sqrt( ( x1-x2)^2 + (y1-y2)^2 + (z1 - z2)^2 );
    // Spherical distance = arccos( 1 - (Cartesian distance)^2 / (2*R^2) ) * R;

    const double x1 = cos_lat1 * cos_lon1;
    const double x2 = cos_lat2 * cos_lon2;
    const double y1 = cos_lat1 * sin_lon1;
    const double y2 = cos_lat2 * sin_lon2;
    const double z1 = cos_lon1;
    const double z2 = cos_lon2;

    const double cart_dist = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
    const double dist = acos(1 - cart_dist*cart_dist / 2 ) * R;

    int streetmatch = ( strlen(inputstreet1) != 0 && strlen( inputstreet2) != 0 &&
                  (strcmp(inputstreet1, inputstreet2) == 0 )) ? 1 : 0;

    return missing +
           in_us ?
               2*(dist < 100) + (dist < 50) + 2*(dist < 10) +
                   ((dist < 1) &&  streetmatch):
               2*(dist < 100) + (dist < 50) + (dist < 10);

}



int latloncmp(const string & inputlat1, const string & inputlon1,
				const string & inputlat2, const string & inputlon2 ){

	static const double R = 3963.0; //radius of the earth is 6378.1km = 3963 miles
	static const double DEG2RAD = 5729.58;

    const double lat1 = atof(inputlat1.c_str());
    const double lon1 = atof(inputlon1.c_str());
    const double lat2 = atof(inputlat2.c_str());
    const double lon2 = atof(inputlon2.c_str());

    const double missing_val = 0.0001;
    int missing = ( ( fabs(lat1) < missing_val && fabs(lon1) < missing_val ) ||
    				( fabs(lat2) < missing_val && fabs(lon2) < missing_val) ) ? 1 : 0;

    const double radlat1 = lat1/DEG2RAD;
    const double radlon1 = lon1/DEG2RAD;
    const double radlat2 = lat2/DEG2RAD;
    const double radlon2 = lon2/DEG2RAD;

    const double cos_lat1 = cos(radlat1);
    const double cos_lat2 = cos(radlat2);
    const double cos_lon1 = cos(radlon1);
    const double cos_lon2 = cos(radlon2);
    const double sin_lon1 = sin(radlon1);
    const double sin_lon2 = sin(radlon2);


    // R=radius, theta = colatitude, phi = longitude
    // Spherical coordinate -> Cartesian coordinate:
    // x=R*sin(theta)*cos(phi) = R*cos(latitude)*cos(longitude)
    // y = R*sin(theta)*sin(phi) = R*cos(latitude)*sin(longitude)
    // z = R*cos(phi) = R * cos(longitude)
    // Cartesion distance = sqrt( ( x1-x2)^2 + (y1-y2)^2 + (z1 - z2)^2 );
    // Spherical distance = arccos( 1 - (Cartesian distance)^2 / (2*R^2) ) * R;

    const double x1 = cos_lat1 * cos_lon1;
    const double x2 = cos_lat2 * cos_lon2;
    const double y1 = cos_lat1 * sin_lon1;
    const double y2 = cos_lat2 * sin_lon2;
    const double z1 = cos_lon1;
    const double z2 = cos_lon2;

    const double cart_dist = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
    const double dist = acos(1 - cart_dist*cart_dist / 2 ) * R;

    return missing +
               2*(dist < 100) + (dist < 75) + (dist < 50) + (dist < 10);

}

int streetcmp(const string& inputstreet1, const string& inputstreet2) {
    int streetmatch = ( inputstreet1.size() != 0 && inputstreet2.size() != 0 && (inputstreet1 == inputstreet2 )) ? 1 : 0;
    return streetmatch;
}

int countrycmp(const string & country1, const string & country2 ) {
	static const string US_label ("US");
	int score = 0;
	if ( country1 == country2 ) {
		++score;
		if ( country1 == US_label)
			++score;
	}
	return score;
}

int classcmp(const string &class1, const string& class2 ){
    return ( class1 == class2 )? 1 : 0;
}

int coauthorcmp(const string &coauthor1, const string& coauthor2 ){
    return ( coauthor1 == coauthor2 )? 1 : 0;
}

int asgcmp_old(const string & asg1, const string & asg2, const map<string, std::pair<string, unsigned int> > * const asg_table_pointer){
	map<string, std::pair<string, unsigned int> >::const_iterator p1, p2;
	p1 = asg_table_pointer->find(asg1);
	p2 = asg_table_pointer->find(asg2);

	if ( p1 == asg_table_pointer->end() || p2 == asg_table_pointer->end() ) {
		std::cout << "Error: either assignee is not found in the assignee tree. "<< asg1 << " or " << asg2 << std::endl;
		throw std::runtime_error("Assignee comparison error.");
	}
	int score = 0;
	if ( p1->second.first == p2->second.first && p1->second.first.size() > 3 ) {
		score = Jaro_Wrinkler_Max;
		if ( p1->second.second < 100 || p2->second.second < 100)
			score += 1;
		//else if ( p1->second.second < 1000 || p2->second.second < 1000 )
		//	score += 1;
	}
	else {
		score = jwcmp(asg1, asg2);
	}
	return score;
}



int asgcmp_to_test(const vector <string> & asg1, const vector <string> & asg2,
			   const map<string, std::pair<string, unsigned int> > * const asg_table_pointer){
	map<string, std::pair<string, unsigned int> >::const_iterator p1, p2;
	p1 = asg_table_pointer->find(asg1.at(0));
	p2 = asg_table_pointer->find(asg2.at(0));
	
	if ( p1 == asg_table_pointer->end() || p2 == asg_table_pointer->end() ) {
		std::cout << "Error: either assignee is not found in the assignee tree. "<< asg1.at(0) << " or " << asg2.at(0) << std::endl;
		exit(3);
	}
	int score = 0;
	if ( p1->second.first == p2->second.first && p1->second.first.size() > 3 ) {
		score = cASGscore::ASGNUMLARGE;
		if ( p1->second.second < 100 || p2->second.second < 100)
			score += 2;
		else if ( p1->second.second < 1000 || p2->second.second < 1000 )
			score += 1;
	}
	else {
		const double jw_threshold = 0.9;
		static const cSentence_JWComparator sjw(jw_threshold);
		vector < const string * > vec_asg1;
		vector < string >::const_iterator q1 = asg1.begin();
		for ( ++q1; q1 != asg1.end(); ++q1 )
			vec_asg1.push_back(&(*q1));
		
		vector < const string * > vec_asg2;
		vector < string >::const_iterator q2 = asg2.begin();
		for ( ++q2; q2 != asg2.end(); ++q2 )
			vec_asg2.push_back(&(*q2));
		
		score = Longest_Common_Subsequence_Incontinuous <const string *, cSentence_JWComparator>(vec_asg1, vec_asg2, sjw).size();
	}
	return score;
}

/*
template < typename Iter1, typename Iter2 >
unsigned int num_common_elements ( Iter1 p1b, Iter1 p1e , Iter2 p2b, Iter2 p2e) {
	// it has to be a sorted version container, like set, or sorted vector or list
	unsigned int cnt = 0;
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
	}
	return cnt;
}
*/



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

class cSimilarity_Score {
private:
	unsigned int score;
public:
	operator unsigned int() {return score;}
};

class cJWscore : public cSimilarity_Score {
public:
	static const unsigned int JWSUB33 = 0;
	static const unsigned int JWMISSING = 1;
	static const unsigned int JW33 = 2;
	static const unsigned int JW66 = 3;
	static const unsigned int JW100 = 4;
	static const unsigned int JW100MULT = 5;
	static const unsigned int JW100MULTFULL = 6;
};

class cASGscore : public cSimilarity_Score {
public:
	static const unsigned int ASGSUB33 = 0;
	static const unsigned int ASGMISSING = 1;
	static const unsigned int ASG33 = 2;
	static const unsigned int ASG66 = 3;
	static const unsigned int ASG100 = 4;
	static const unsigned int ASG100MULT = 5;
	static const unsigned int ASGNUMLARGE = 6;
	static const unsigned int ASGNUMMEDIUM = 7;
	static const unsigned int ASGNUMSMALL = 8;
};

class cDISTscore : public cSimilarity_Score {
public:
	static const unsigned int DIST100PLUS = 0;
	static const unsigned int DISTMISSING = 1;
	static const unsigned int DIST100 = 2;
	static const unsigned int DIST75 = 3;
	static const unsigned int DIST50 = 4;
	static const unsigned int DIST10NOST = 5;
	static const unsigned int DIST10ST = 6;
	static const unsigned int DIST0ST = 7;
};

class cCLASSscore : public cSimilarity_Score {
public:
	static const unsigned int CLASS0 = 0;
	static const unsigned int CLASSMISS = 1;
	static const unsigned int CLASS25 = 2;
	static const unsigned int CLASS50 = 3;
	static const unsigned int CLASS75PLUS = 4;
};

class cCOAUTHscore : public cSimilarity_Score {
public:
	static const unsigned int C0 = 0;
	static const unsigned int C1 = 1;
	static const unsigned int C2 = 2;
	static const unsigned int C3 = 3;
	static const unsigned int C4 = 4;
	static const unsigned int C5 = 5;
	static const unsigned int C6 = 6;
	static const unsigned int C7 = 7;
	static const unsigned int C8 = 8;
	static const unsigned int C9 = 9;
	static const unsigned int C10 = 10;
};

class cMIDNAMEscore : public cSimilarity_Score {
public:
	static const unsigned int M0 = 0;
	static const unsigned int MMISSING = 1;
	static const unsigned int M33 = 2;
	static const unsigned int M67 = 3;
	static const unsigned int M100 = 4;
};

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
int asgcmp_old(const string & asg1, const string & asg2, const map<string, std::pair<string, unsigned int> > * const asg_table_pointer);
int asgcmp_to_test(const vector <string> & asg1, const vector <string> & asg2,
		   const map<string, std::pair<string, unsigned int> > * const asg_table_pointer);


class cSentence_JWComparator {
private:
	const double threshold;
public:
	bool operator() (const string * s1, const string * s2) const;
	explicit cSentence_JWComparator(const double inputthreshold): threshold(inputthreshold){};
};


#endif /* DISAMBIGCOMP_H_ */

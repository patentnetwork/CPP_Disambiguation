#include "DisambigNNLS.h"
#include <iostream>

vector < double > NNLS_fit (const vector< vector < double > > & A, const vector < double > & b, double & R_sq) {

const unsigned int m = A.size();
if ( m == 0 )
    throw cException_NNLS(" row = 0");
if ( m != b.size() )
    throw cException_NNLS ( " Matrix row != vector row.");

const unsigned int n = A.at(0).size();



if ( n == 0 )
    throw cException_NNLS(" col = 0");

double * pA = new double [m * n];
double * pMat = pA;
for ( unsigned int i = 0; i < m; ++i )
    for ( unsigned int j = 0; j < n; ++j )
        *pMat ++ = A.at(i).at(j);

const unsigned int mda = m;

double *pb = new double [m] ;
double *pbcur = pb;

for ( unsigned int i = 0; i < m; ++i )
    *pbcur ++ = b.at(i);

double rnorm;
double *pw = new double [n];
double *pzz = new double [m];
int *index = new int [n];
int mode;
double *px = new double [n];
double *pxcur = px;



nnls(pA, mda, m, n, pb, px, &rnorm, pw, pzz, index, &mode);

vector <double> ans;
for ( unsigned int i = 0; i < n; ++ i)
    ans.push_back(*pxcur++);

delete [] px;
delete [] index;
delete [] pzz;
delete [] pw;
delete [] pb;
delete [] pA;

// now calculating the R-square

double temp = 0;
for ( unsigned int i = 0; i < m; ++i )
    temp += b.at(i);

const double b_av = temp/m;

double total_error = 0;

for ( unsigned int i = 0; i < m; ++i )
    total_error += ( b.at(i) - b_av ) * (b.at(i) - b_av );

R_sq = 1 - rnorm*rnorm/total_error;

if ( mode != 1)
    throw cException_NNLS ( "NNLS failed");

std::cout << "R-Square = " << R_sq << std::endl;
std::cout << "Coefficients = ";
for ( unsigned int i = 0; i < n; ++i)
	std::cout << ans.at(i) << ", ";
std::cout << std::endl;

return ans;
}

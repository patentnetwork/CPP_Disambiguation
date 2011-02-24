#ifndef DISAMBIGNNLS_H_INCLUDED
#define DISAMBIGNNLS_H_INCLUDED

#include <vector>
#include <iostream>
#include "nnls.h"
using std::vector;

// find non negative vector x to minimize | Ax -b |
vector < double > NNLS_fit (const vector< vector < double > > & A, const vector < double > & b, double & R_sq);

class cException_NNLS : public std::exception {
private:
    std::string m_errmsg;
public:
    cException_NNLS(const char * input): m_errmsg(input) {}
    const char * what() const throw(){ return m_errmsg.c_str();}
    ~cException_NNLS() throw() {}
};


#endif // DISAMBIGNNLS_H_INCLUDED

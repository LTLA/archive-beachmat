#ifndef BEACHMAT_INTEGER_MATRIX_H
#define BEACHMAT_INTEGER_MATRIX_H

#include "matrix.h"
#include "output.h"

namespace beachmat {

/********************************************
 * Virtual base class for integer matrices. *
 ********************************************/

typedef any_matrix<int, Rcpp::IntegerVector> integer_matrix;

/* Simple integer matrix */

typedef simple_matrix<int, Rcpp::IntegerVector> simple_integer_matrix;

/* HDF5Matrix */

#ifdef BEACHMAT_USE_HDF5

class HDF5_integer_matrix : public HDF5_matrix<int, Rcpp::IntegerVector> {
public:    
    HDF5_integer_matrix(const Rcpp::RObject&);
    ~HDF5_integer_matrix();
};

#endif

/* Bigmemory integer matrix */

typedef bigmemory_matrix<int, Rcpp::IntegerVector, 4> bigmemory_integer_matrix;

/* Dispatcher */

std::unique_ptr<integer_matrix> create_integer_matrix(const Rcpp::RObject&);

/***************************************************
 * Virtual base class for output integer matrices. *
 ***************************************************/

typedef output_matrix<int, Rcpp::IntegerVector> integer_output;

/* Simple output integer matrix */

typedef simple_output<int, Rcpp::IntegerVector> simple_integer_output;

/* HDF5 output integer matrix */

#ifdef BEACHMAT_USE_HDF5

class HDF5_integer_output : public HDF5_output<int, Rcpp::IntegerVector, integer_zero> {
public:    
    HDF5_integer_output(int, int);
    ~HDF5_integer_output();
};

#endif

/* Output dispatchers */

std::unique_ptr<integer_output> create_integer_output(int, int, bool=true);

std::unique_ptr<integer_output> create_integer_output(int, int, const Rcpp::RObject&, bool);

}

#endif

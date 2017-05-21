#ifndef BEACHMAT_CHARACTER_MATRIX_H
#define BEACHMAT_CHARACTER_MATRIX_H

#include "matrix.h"

namespace beachmat { 

/* Virtual base class for character matrices. */

class character_matrix {
public:    
    character_matrix();
    virtual ~character_matrix();
    
    virtual size_t get_nrow() const=0;
    virtual size_t get_ncol() const=0;
    
    void get_row(size_t, Rcpp::StringVector::iterator); 
    virtual void get_row(size_t, Rcpp::StringVector::iterator, size_t, size_t)=0;

    void get_col(size_t, Rcpp::StringVector::iterator);
    virtual void get_col(size_t, Rcpp::StringVector::iterator, size_t, size_t)=0;

    virtual Rcpp::String get(size_t, size_t)=0;

    virtual std::unique_ptr<character_matrix> clone() const=0;
};

/* Simple character matrix */

class simple_character_matrix : public character_matrix {
public:
    simple_character_matrix(const Rcpp::RObject& incoming);
    ~simple_character_matrix();
  
    size_t get_nrow() const;
    size_t get_ncol() const;
 
    void get_row(size_t, Rcpp::StringVector::iterator, size_t, size_t);
    void get_col(size_t, Rcpp::StringVector::iterator, size_t, size_t);

    Rcpp::String get(size_t, size_t);

    std::unique_ptr<character_matrix> clone() const;
private:
    simple_matrix<Rcpp::String, Rcpp::StringVector> mat;
};

/* HDF5Matrix */

class HDF5_character_matrix : public character_matrix {
public:    
    HDF5_character_matrix(const Rcpp::RObject&);
    ~HDF5_character_matrix();

    size_t get_nrow() const;
    size_t get_ncol() const;
 
    void get_row(size_t, Rcpp::StringVector::iterator, size_t, size_t);
    void get_col(size_t, Rcpp::StringVector::iterator, size_t, size_t);

    Rcpp::String get(size_t, size_t);

    std::unique_ptr<character_matrix> clone() const;
protected:
    HDF5_matrix<Rcpp::String> mat; // Type doesn't really matter here, as we override all methods anyway.
    H5::StrType str_type;
    std::vector<char> row_buf, col_buf;
};

/* Dispatcher */

std::unique_ptr<character_matrix> create_character_matrix(const Rcpp::RObject&);

}

#endif

#ifndef BEACHMAT_INPUT_MATRIX_H
#define BEACHMAT_INPUT_MATRIX_H

#include "beachmat.h"
#include "any_matrix.h"
#include "utils.h"

namespace beachmat { 

/* Simple matrix */

template<typename T, class V>
class simple_matrix : public any_matrix {
public:    
    simple_matrix(const Rcpp::RObject&);
    ~simple_matrix();

    T get(size_t, size_t);

    template <class Iter>
    void get_row(size_t, Iter, size_t, size_t);

    template <class Iter>
    void get_col(size_t, Iter, size_t, size_t);
private:
    V mat;
};

/* dense Matrix */

template<typename T, class V>
class dense_matrix : public any_matrix {
public:    
    dense_matrix(const Rcpp::RObject&);
    ~dense_matrix();

    T get(size_t, size_t);

    template <class Iter>
    void get_row(size_t, Iter, size_t, size_t);

    template <class Iter>
    void get_col(size_t, Iter, size_t, size_t);
protected:
    V x;
};

/* column-major sparse Matrix */

template<typename T, class V>
class Csparse_matrix : public any_matrix {
public:    
    Csparse_matrix(const Rcpp::RObject&);
    ~Csparse_matrix();

    T get(size_t, size_t);

    template <class Iter>
    void get_row(size_t, Iter, size_t, size_t);

    template <class Iter>
    void get_col(size_t, Iter, size_t, size_t);
protected:
    Rcpp::IntegerVector i, p;
    V x;

    size_t currow, curstart, curend;
    std::vector<int> indices; // Left as 'int' to simplify comparisons with 'i' and 'p'.
    void update_indices(size_t, size_t, size_t);

    T get_empty() const; // Specialized function for each realization (easy to extend for non-int/double).
};

/* symmetric packed Matrix */

template<typename T, class V>
class Psymm_matrix : public any_matrix {
public:    
    Psymm_matrix(const Rcpp::RObject&);
    ~Psymm_matrix();

    T get(size_t, size_t);   

    template <class Iter>
    void get_row(size_t, Iter, size_t, size_t);

    template <class Iter>
    void get_col(size_t, Iter, size_t, size_t);
protected:
    V x;
    bool upper;

    template <class Iter>
    void get_rowcol(size_t, Iter, size_t, size_t);

    size_t get_index(size_t, size_t) const;
};

/* Run length encoding-based matrix */

template<typename T, class V>
class Rle_matrix : public any_matrix {
public:
    Rle_matrix(const Rcpp::RObject&);
    ~Rle_matrix();

    T get(size_t, size_t);

    template<class Iter>
    void get_row(size_t, Iter, size_t, size_t); 

    template<class Iter>
    void get_col(size_t, Iter, size_t, size_t); 
private:
    V runvalue;
    std::vector<size_t> coldex;
    std::vector<std::deque<size_t> > cumrow;

    size_t cache_row, cache_start, cache_end;
    std::vector<size_t> cache_indices;
    void update_indices(size_t r, size_t start, size_t end);
};

/* HDF5Matrix */

template<typename T, int RTYPE>
class HDF5_matrix : public any_matrix {
public:
    HDF5_matrix(const Rcpp::RObject&);
    ~HDF5_matrix();

    void extract_row(size_t, T*, size_t, size_t);
    template<typename X>
    void extract_row(size_t, X*, const H5::DataType&, size_t, size_t);

    void extract_col(size_t, T*, size_t, size_t);
    template<typename X>
    void extract_col(size_t, X*, const H5::DataType&, size_t, size_t);
    
    void extract_one(size_t, size_t, T*); // Use of pointer is a bit circuitous, but necessary for character access.
    template<typename X>
    void extract_one(size_t, size_t, X*, const H5::DataType&);  

    const H5::DataType& get_datatype() const;
protected:
    Rcpp::RObject realized;

    H5::H5File hfile;
    H5::DataSet hdata;
    H5::DataSpace hspace, rowspace, colspace, onespace;
    hsize_t h5_start[2], col_count[2], row_count[2], one_count[2], zero_start[1];

    H5::DataType default_type;
    H5T_class_t set_types();
};

#include "Input_methods.h"

}

#endif


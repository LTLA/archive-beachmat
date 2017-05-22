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
    Csparse_matrix(const Rcpp::RObject&, T);
    ~Csparse_matrix();

    T get(size_t, size_t);

    template <class Iter>
    void get_row(size_t, Iter, size_t, size_t);

    template <class Iter>
    void get_col(size_t, Iter, size_t, size_t);
protected:
    Rcpp::IntegerVector i, p;
    V x;
    const T fill;

    size_t currow, curstart, curend;
    std::vector<int> indices; // Left as 'int' to simplify comparisons with 'i' and 'p'.
    void update_indices(size_t, size_t, size_t);
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
    size_t get_index(size_t, size_t) const;
};

/* HDF5Matrix */

template<typename T>
class HDF5_matrix : public any_matrix {
public:
    HDF5_matrix(const Rcpp::RObject&, int, const H5T_class_t&);
    ~HDF5_matrix();

    void extract_row(size_t, T*, const H5::DataType&, size_t, size_t);
    void extract_col(size_t, T*, const H5::DataType&, size_t, size_t);
    void extract_one(size_t, size_t, T*, const H5::DataType&);  

    const H5::DataSet& get_dataset() const;
protected:
    Rcpp::RObject realized;

    H5::H5File hfile;
    H5::DataSet hdata;
    H5::DataSpace hspace, rowspace, colspace, onespace;
    hsize_t h5_start[2], col_count[2], row_count[2], one_count[2], zero_start[1];
};

#include "Input_methods.h"

}

#endif

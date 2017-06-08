#include "logical_matrix.h"

namespace beachmat {

/* Csparse logical input methods. */

template<>
int Csparse_matrix<int, Rcpp::LogicalVector>::get_empty() const { return 0; }

/* Sparse numeric output methods. */

template<>
int sparse_output<int, Rcpp::LogicalVector>::get_empty() const { return 0; }

/* HDF5 logical output methods. */

template<>
int HDF5_output<int, Rcpp::LogicalVector>::get_empty() const { return 0; }

/* Dispatch definition */

std::unique_ptr<logical_matrix> create_logical_matrix(const Rcpp::RObject& incoming) { 
    if (incoming.isS4()) {
        std::string ctype=get_class(incoming);
        if (ctype=="lgeMatrix") { 
            return std::unique_ptr<logical_matrix>(new dense_logical_matrix(incoming));
        } else if (ctype=="lgCMatrix") { 
            return std::unique_ptr<logical_matrix>(new Csparse_logical_matrix(incoming));
        } else if (ctype=="lgTMatrix") {
            throw std::runtime_error("lgTMatrix not supported, convert to lgCMatrix");
        } else if (ctype=="lspMatrix") {
            return std::unique_ptr<logical_matrix>(new Psymm_logical_matrix(incoming));
        } else if (ctype=="HDF5Matrix" || ctype=="DelayedMatrix") { 
            return std::unique_ptr<logical_matrix>(new HDF5_logical_matrix(incoming));
        } else if (ctype=="RleMatrix") {
            return std::unique_ptr<logical_matrix>(new Rle_logical_matrix(incoming));
        }
        throw_custom_error("unsupported class '", ctype, "' for logical_matrix");
    } 
    return std::unique_ptr<logical_matrix>(new simple_logical_matrix(incoming));
}

/* Output dispatch definition */

std::unique_ptr<logical_output> create_logical_output(int nrow, int ncol, output_mode mode) {
    switch (mode) {
        case BASIC:
            return std::unique_ptr<logical_output>(new simple_logical_output(nrow, ncol));
        case SPARSE:
            return std::unique_ptr<logical_output>(new sparse_logical_output(nrow, ncol));
        case HDF5:
            return std::unique_ptr<logical_output>(new HDF5_logical_output(nrow, ncol));
        default:
            throw std::runtime_error("unsupported output mode for logical matrices");
    }
}

std::unique_ptr<logical_output> create_logical_output(int nrow, int ncol, const Rcpp::RObject& incoming, bool simplify, bool preserve_zero) {
    return create_logical_output(nrow, ncol, choose_output_mode(incoming, simplify, preserve_zero));
}

}

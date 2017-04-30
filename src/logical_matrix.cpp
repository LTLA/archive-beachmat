#include "logical_matrix.h"

/* Methods for a HDF5 matrix. */

#ifdef BEACHMAT_USE_HDF5

HDF5_logical_matrix::HDF5_logical_matrix(const Rcpp::RObject& incoming) : HDF5_matrix<int, H5::PredType::NATIVE_INT32>(incoming) { 
    const Rcpp::RObject& h5_seed=incoming.slot("seed");
    const Rcpp::RObject& firstval=get_safe_slot(h5_seed, "first_val");
    if (firstval.sexp_type()!=LGLSXP) { 
        throw_custom_error("'first_val' slot in a ", get_class(h5_seed), " object should be logical");
    }
    if (hdata.getTypeClass()!=H5T_INTEGER) { 
        throw std::runtime_error("data type in HDF5 file is not integer");
    }
    return;
}

HDF5_logical_matrix::~HDF5_logical_matrix() {}

#endif

/* Dispatch definition */

std::shared_ptr<logical_matrix> create_logical_matrix(const Rcpp::RObject& incoming) { 
    if (incoming.isS4()) {
        std::string ctype=get_class(incoming);
        if (ctype=="lgeMatrix") { 
            return std::shared_ptr<logical_matrix>(new dense_logical_matrix(incoming));
        } else if (ctype=="lgCMatrix") { 
            return std::shared_ptr<logical_matrix>(new Csparse_logical_matrix(incoming));
        } else if (ctype=="lgTMatrix") {
            throw std::runtime_error("lgTMatrix not supported, convert to lgCMatrix");
        } else if (ctype=="lspMatrix") {
            return std::shared_ptr<logical_matrix>(new Psymm_logical_matrix(incoming));
        } else if (ctype=="HDF5Matrix") { 
#ifdef BEACHMAT_USE_HDF5            
            return std::shared_ptr<logical_matrix>(new HDF5_logical_matrix(incoming));
#else
            throw std::runtime_error("'beachmat' not compiled with HDF5 support");
#endif            
        }
        throw_custom_error("unsupported class '", ctype, "' for logical_matrix");
    } 
    return std::shared_ptr<logical_matrix>(new simple_logical_matrix(incoming));
}



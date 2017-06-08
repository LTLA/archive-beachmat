#include "character_matrix.h"

namespace beachmat {

/* Methods for the virtual class. */

character_matrix::character_matrix() {}

character_matrix::~character_matrix() {}

void character_matrix::get_col(size_t c, Rcpp::StringVector::iterator out) { 
    get_col(c, out, 0, get_nrow());
}

void character_matrix::get_row(size_t r, Rcpp::StringVector::iterator out) { 
    get_row(r, out, 0, get_ncol());
}

/* Methods for the simple character matrix. */

simple_character_matrix::simple_character_matrix(const Rcpp::RObject& incoming) : mat(incoming) {}

simple_character_matrix::~simple_character_matrix() {}

size_t simple_character_matrix::get_nrow() const {
    return mat.get_nrow();
}

size_t simple_character_matrix::get_ncol() const {
    return mat.get_ncol();
}

void simple_character_matrix::get_row(size_t r, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    mat.get_row(r, out, start, end);
}

void simple_character_matrix::get_col(size_t c, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    mat.get_col(c, out, start, end);
}

Rcpp::String simple_character_matrix::get(size_t r, size_t c) {
    return mat.get(r, c);
}

std::unique_ptr<character_matrix> simple_character_matrix::clone() const {
    return std::unique_ptr<character_matrix>(new simple_character_matrix(*this));
}

/* Methods for the Rle character matrix. */

Rle_character_matrix::Rle_character_matrix(const Rcpp::RObject& incoming) : mat(incoming) {}

Rle_character_matrix::~Rle_character_matrix() {}

size_t Rle_character_matrix::get_nrow() const {
    return mat.get_nrow();
}

size_t Rle_character_matrix::get_ncol() const {
    return mat.get_ncol();
}

void Rle_character_matrix::get_row(size_t r, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    mat.get_row(r, out, start, end);
}

void Rle_character_matrix::get_col(size_t c, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    mat.get_col(c, out, start, end);
}

Rcpp::String Rle_character_matrix::get(size_t r, size_t c) {
    return mat.get(r, c);
}

std::unique_ptr<character_matrix> Rle_character_matrix::clone() const {
    return std::unique_ptr<character_matrix>(new Rle_character_matrix(*this));
}

/* Methods for the HDF5 character matrix. */

HDF5_character_matrix::HDF5_character_matrix(const Rcpp::RObject& incoming) : mat(incoming) {
    const H5::DataType& str_type=mat.get_datatype();
    if (!str_type.isVariableStr()) { 
        bufsize=str_type.getSize(); 
        row_buf.resize(bufsize*(mat.get_ncol()));
        col_buf.resize(bufsize*(mat.get_nrow()));
        one_buf.resize(bufsize);
    } else {
        throw std::runtime_error("variable-length strings not supported for HDF5_character_matrix");
    }
}

HDF5_character_matrix::~HDF5_character_matrix() {}

size_t HDF5_character_matrix::get_nrow() const {
    return mat.get_nrow();
}

size_t HDF5_character_matrix::get_ncol() const {
    return mat.get_ncol();
}

void HDF5_character_matrix::get_row(size_t r, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    char* ref=row_buf.data();
    mat.extract_row(r, ref, start, end);
    for (size_t c=start; c<end; ++c, ref+=bufsize, ++out) {
        (*out)=ref; 
    }
    return;
} 

void HDF5_character_matrix::get_col(size_t c, Rcpp::StringVector::iterator out, size_t start, size_t end) { 
    char* ref=col_buf.data();
    mat.extract_col(c, ref, start, end);
    for (size_t r=start; r<end; ++r, ref+=bufsize, ++out) {
        (*out)=ref; 
    }
    return;
}
 
Rcpp::String HDF5_character_matrix::get(size_t r, size_t c) { 
    char* ref=one_buf.data();
    mat.extract_one(r, c, ref);
    return ref;
}

std::unique_ptr<character_matrix> HDF5_character_matrix::clone() const {
    return std::unique_ptr<character_matrix>(new HDF5_character_matrix(*this));
}
   
/* Dispatch definition */

std::unique_ptr<character_matrix> create_character_matrix(const Rcpp::RObject& incoming) { 
    if (incoming.isS4()) { 
        std::string ctype=get_class(incoming);
        if (ctype=="HDF5Matrix" || ctype=="DelayedMatrix") { 
            return std::unique_ptr<character_matrix>(new HDF5_character_matrix(incoming));
        } else if (ctype=="RleMatrix") { 
            return std::unique_ptr<character_matrix>(new Rle_character_matrix(incoming));
        }
        std::stringstream err;
        err << "unsupported class '" << ctype << "' for character_matrix";
        throw std::runtime_error(err.str().c_str());
    } 
    return std::unique_ptr<character_matrix>(new simple_character_matrix(incoming));
}

}

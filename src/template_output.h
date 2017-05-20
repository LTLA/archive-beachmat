#ifndef BEACHMAT_TEMPLATE_OUTPUT_H
#define BEACHMAT_TEMPLATE_OUTPUT_H

/* Methods for the base output class. */

template<typename T, class V>
output_matrix<T, V>::output_matrix(size_t nr, size_t nc) : nrow(nr), ncol(nc) {}

template<typename T, class V>
output_matrix<T, V>::~output_matrix() {}

template<typename T, class V>
void output_matrix<T, V>::fill_row(size_t r, typename V::iterator in) {
    fill_row(r, in, 0, this->ncol);
    return;
}

template<typename T, class V>
void output_matrix<T, V>::fill_col(size_t c, typename V::iterator in) {
    fill_col(c, in, 0, this->nrow);
    return;
}

template<typename T, class V>
size_t output_matrix<T, V>::get_nrow() const { 
    return this->nrow;
}

template<typename T, class V>
size_t output_matrix<T, V>::get_ncol() const { 
    return this->ncol;
}

template<typename T, class V>
void output_matrix<T, V>::get_row(size_t r, typename V::iterator out) {
    get_row(r, out, 0, ncol);
    return;
}

template<typename T, class V>
void output_matrix<T, V>::get_col(size_t c, typename V::iterator out) {
    get_col(c, out, 0, nrow);
    return;
}

/* Methods for the simple output matrix. */

template<typename T, class V>
simple_output<T, V>::simple_output(size_t nr, size_t nc) : output_matrix<T, V>(nr, nc) { 
    (this->data)=V(nr*nc);
    return; 
}

template<typename T, class V>
simple_output<T, V>::~simple_output() {}

template<typename T, class V>
std::unique_ptr<output_matrix<T, V> > simple_output<T, V>::clone() const {
    return std::unique_ptr<output_matrix<T, V> >(new simple_output<T, V>(*this));
}

template<typename T, class V>
void simple_output<T, V>::fill_col(size_t c, typename V::iterator in, size_t start, size_t end) {
    std::copy(in, in + end - start, data.begin()+c*(this->nrow)+start); 
    return;
}

template<typename T, class V>
void simple_output<T, V>::fill_row(size_t r, typename V::iterator in, size_t start, size_t end) {
    const size_t& NR=this->nrow;
    auto mIt=data.begin() + r + start * NR;
    for (size_t c=start; c<end; ++c, mIt+=NR, ++in) {
        (*mIt)=*in;        
    }
    return;
}

template<typename T, class V>
void simple_output<T, V>::fill(size_t r, size_t c, T in) {
    data[r + (this->nrow)*c]=in;
    return;
}

template<typename T, class V>
void simple_output<T, V>::get_row(size_t r, typename V::iterator out, size_t start, size_t end) {
    const size_t& NR=this->nrow;
    auto src=data.begin()+start*NR+r;
    for (size_t col=start; col<end; ++col, src+=NR, ++out) { (*out)=(*src); }
    return;
}

template<typename T, class V>
void simple_output<T, V>::get_col(size_t c, typename V::iterator out, size_t start, size_t end) {
    auto src=data.begin() + c*(this->nrow);
    std::copy(src+start, src+end, out);
    return;
}

template<typename T, class V>
T simple_output<T, V>::get(size_t r, size_t c) {
    return data[c*(this->nrow)+r];
}

template<typename T, class V>
Rcpp::RObject simple_output<T, V>::yield() {
    Rcpp::RObject out(SEXP(this->data));
    out.attr("dim") = Rcpp::IntegerVector::create(this->nrow, this->ncol); 
    return out;
}

/* Methods for HDF5 output matrix. */

#ifdef BEACHMAT_USE_HDF5

template<typename T, class V, const T& FILL>
HDF5_output<T, V, FILL>::HDF5_output (size_t nr, size_t nc, const H5::PredType& hpt) : output_matrix<T, V>(nr, nc), HPT(hpt) {

    // File opening.
    Rcpp::Environment hdf5env("package:HDF5Array");
    Rcpp::Function filenamefun=hdf5env["getHDF5DumpFile"];
    fname=make_to_string(filenamefun(Rcpp::Named("for.use", Rcpp::LogicalVector::create(1))));
    hfile.openFile(fname, H5F_ACC_RDWR);

    H5::DSetCreatPropList plist;
    plist.setFillValue(HPT, &FILL);
    hsize_t dims[2];
    dims[0]=this->ncol; // Setting the dimensions (0 is column, 1 is row; internally transposed).
    dims[1]=this->nrow; 
    hspace.setExtentSimple(2, dims);

    // Creating the data set.
    Rcpp::Function datanamefun=hdf5env["getHDF5DumpName"];
    dname=make_to_string(datanamefun(Rcpp::Named("for.use", Rcpp::LogicalVector::create(1))));
    hdata=hfile.createDataSet(dname, HPT, hspace, plist); 

    Rcpp::Function appendfun=hdf5env["appendDatasetCreationToHDF5DumpLog"];
    appendfun(Rcpp::StringVector(fname), 
            Rcpp::StringVector(dname), 
            Rcpp::IntegerVector::create(this->nrow, this->ncol),
            Rcpp::StringVector(translate_type(V().sexp_type())));

    h5_start[0]=0;
    h5_start[1]=0;
    col_count[0]=1;
    col_count[1]=this->nrow;
    row_count[0]=this->ncol;
    row_count[1]=1;
    
    zero_start[0]=0;
    one_count[0]=1;
    one_count[1]=1;
    onespace=H5::DataSpace(1, one_count);
    onespace.selectAll();

    return;
}

template<typename T, class V, const T& FILL>
HDF5_output<T, V, FILL>::~HDF5_output() {}

template<typename T, class V, const T& FILL>
std::unique_ptr<output_matrix<T, V> > HDF5_output<T, V, FILL>::clone() const {
    return std::unique_ptr<output_matrix<T, V> >(new HDF5_output<T, V, FILL>(*this));
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::select_col(size_t c, size_t start, size_t end) {
    col_count[1]=end-start;
    colspace.setExtentSimple(1, col_count+1);
    colspace.selectAll();
    h5_start[0]=c;
    h5_start[1]=start;
    hspace.selectHyperslab(H5S_SELECT_SET, col_count, h5_start);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::fill_col(size_t c, typename V::iterator in, size_t start, size_t end) {
    select_col(c, start, end);
    hdata.write(&(*in), HPT, colspace, hspace);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::select_row(size_t r, size_t start, size_t end) {
    row_count[0] = end-start;
    rowspace.setExtentSimple(1, row_count);
    rowspace.selectAll();
    h5_start[0] = start;
    h5_start[1] = r;
    hspace.selectHyperslab(H5S_SELECT_SET, row_count, h5_start);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::fill_row(size_t c, typename V::iterator in, size_t start, size_t end) {
    select_row(c, start, end);
    hdata.write(&(*in), HPT, rowspace, hspace);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::select_one(size_t r, size_t c) {
    h5_start[0]=c;
    h5_start[1]=r;
    hspace.selectHyperslab(H5S_SELECT_SET, one_count, h5_start);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::fill(size_t r, size_t c, T in) {
    select_one(r, c);
    hdata.write(&in, HPT, onespace, hspace);
    return;
}

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::get_row(size_t r, typename V::iterator out, size_t start, size_t end) { 
    select_row(r, start, end);
    hdata.read(&(*out), HPT, rowspace, hspace);
    return;
} 

template<typename T, class V, const T& FILL>
void HDF5_output<T, V, FILL>::get_col(size_t c, typename V::iterator out, size_t start, size_t end) { 
    select_col(c, start, end);
    hdata.read(&(*out), HPT, colspace, hspace);
    return;
}

template<typename T, class V, const T& FILL>
T HDF5_output<T, V, FILL>::get(size_t r, size_t c) { 
    select_one(r, c);
    T out;
    hdata.read(&out, HPT, onespace, hspace);
    return out;
}

template<typename T, class V, const T& FILL>
Rcpp::RObject HDF5_output<T, V, FILL>::yield() {
    std::string seedclass="HDF5ArraySeed";
    Rcpp::S4 h5seed(seedclass);

    // Assigning to slots.
    if (!h5seed.hasSlot("file")) {
        throw_custom_error("missing 'file' slot in ", seedclass, " object");
    }
    h5seed.slot("file") = fname;
    if (!h5seed.hasSlot("name")) {
        throw_custom_error("missing 'name' slot in ", seedclass, " object");
    }
    h5seed.slot("name") = dname;
    if (!h5seed.hasSlot("dim")) {
        throw_custom_error("missing 'dim' slot in ", seedclass, " object");
    }
    h5seed.slot("dim") = Rcpp::IntegerVector::create(this->nrow, this->ncol);
    if (!h5seed.hasSlot("first_val")) {
        throw_custom_error("missing 'first_val' slot in ", seedclass, " object");
    }
    if (this->nrow && this->ncol) { 
        h5seed.slot("first_val") = get(0, 0);
    } else {
        h5seed.slot("first_val") = V(0); // empty vector.
    }

    // Assigning the seed to the HDF5Matrix.
    std::string matclass="HDF5Matrix";
    Rcpp::S4 h5mat(matclass);
    if (!h5mat.hasSlot("seed")) {
        throw_custom_error("missing 'seed' slot in ", matclass, " object");
    }
    h5mat.slot("seed") = h5seed;

    return SEXP(h5mat);
}

#endif

#endif


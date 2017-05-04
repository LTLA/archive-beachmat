#ifndef TEMPLATE_TESTFUN_H
#define TEMPLATE_TESTFUN_H

/* This function tests the output fill_row/fill_col methods; that they
 * call the fill_row/fill_col methods of the derived classes (along with the
 * correct arguments to the overloaded virtual methods). 
 */

template <class T, class M, class O>  // M, O are automatically deduced.
Rcpp::RObject pump_out(M& ptr, O& optr, const Rcpp::IntegerVector& mode) {
    if (mode.size()!=1) { 
        throw std::runtime_error("'mode' should be an integer scalar"); 
    }
    const int Mode=mode[0];
    const int& nrows=ptr->get_nrow();
    const int& ncols=ptr->get_ncol();

    if (Mode==1) { 
        // By column.
        T target(nrows);
        for (int c=0; c<ncols; ++c) {
            ptr->get_col(c, target.begin());
            optr->fill_col(c, target.begin());                
        }
    } else if (Mode==2) { 
        // By row.
        T target(ncols);
        for (int r=0; r<nrows; ++r) {
            ptr->get_row(r, target.begin());
            optr->fill_row(r, target.begin());                
        }
    } else if (Mode==3) {
        // By cell.
        for (int c=0; c<ncols; ++c){ 
            for (int r=0; r<nrows; ++r) {
                optr->fill(r, c, ptr->get(r, c));
            }
        }
    } else { 
        throw std::runtime_error("'mode' should be in [1,3]"); 
    }
    return optr->yield();
}

/* This function tests the fill_row/fill_col methods, and that they properly
 * call the fill_row/fill_col methods of the derived classes with slices.
 */

template <class T, class M, class O>  
Rcpp::RObject pump_out_slice (M& ptr, O& optr, const Rcpp::IntegerVector& mode, const Rcpp::IntegerVector& rows, const Rcpp::IntegerVector& cols) {

    if (mode.size()!=1) { 
        throw std::runtime_error("'mode' should be an integer scalar"); 
    }
    const int Mode=mode[0];

    if (rows.size()!=2) { 
        throw std::runtime_error("'rows' should be an integer vector of length 2"); 
    }
    const int rstart=rows[0]-1, rend=rows[1];
    const int nrows=rend-rstart;    

    if (cols.size()!=2) { 
        throw std::runtime_error("'cols' should be an integer vector of length 2"); 
    }
    const int cstart=cols[0]-1, cend=cols[1];
    const int ncols=cend-cstart;    

    if (Mode==1) { 
        // By column.
        T target(nrows);
        for (int c=0; c<ncols; ++c) {
            ptr->get_col(c+cstart, target.begin(), rstart, rend);
            optr->fill_col(c+cstart, target.begin(), rstart, rend);
        }
    } else if (Mode==2) { 
        // By row.
        T target(ncols);
        for (int r=0; r<nrows; ++r) {
            ptr->get_row(r+rstart, target.begin(), cstart, cend);
            optr->fill_row(r+rstart, target.begin(), cstart, cend);
        }
    } else { 
        throw std::runtime_error("'mode' should be in [1,2]"); 
    }

    return optr->yield();
}

#endif

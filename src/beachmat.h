#ifndef BEACHMAT_H
#define BEACHMAT_H

#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <stdexcept>
#include <sstream>

#include "Rcpp.h"
#include "R.h"
#include "Rinternals.h"

#define BEACHMAT_USE_HDF5

#ifdef BEACHMAT_USE_HDF5
#include "c++/H5Cpp.h"
#endif

#endif
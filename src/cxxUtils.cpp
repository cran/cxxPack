// cxxUtils.cpp: misc utilities
//
// Copyright (C) 2010 Dominick Samperi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 3 as 
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cxxUtils.hpp>

namespace cxxPack {

/**
 * Gets the dim attribute vector of a SEXP. This may be the only
 * place were we use R's low-level macros, and these functions
 * are not really needed anyway since the dimension info is
 * directly available from Rcpp::NumericVector, Rcpp::NumericMatrix.
 */
std::vector<int> getRDims(SEXP s) {
    SEXP dimAttr = Rf_getAttrib(s, R_DimSymbol);
    if(dimAttr == R_NilValue)
	return std::vector<int>(0);
    int len = Rf_length(dimAttr);
    std::vector<int> dims(len);
    for(int i=0; i < len; ++i)
	dims[i] = INTEGER(dimAttr)[i];
    return dims;
}

/**
 * Sets the dim attribute vector for a SEXP.
 */
void setRDims(SEXP s, std::vector<int>& dims) {
    if(dims.size() <= 0)
	throw std::range_error("Invalid dims in setRDims");
    Rcpp::IntegerVector iv(dims.size());
    for(int i=0; i < (int)dims.size(); ++i)
	iv[i] = dims[i];
    Rf_setAttrib(s, R_DimSymbol, iv);
}

}

namespace Rcpp {

#ifdef SWAP_POSIXt // For R versions >= 2.12
    std::string datetimeClass[] = { "POSIXct", "POSIXt" };
#else
    std::string datetimeClass[] = { "POSIXt", "POSIXct" };
#endif

    // Wrap() implementations for some objects in the Rcpp package.

    template<> SEXP wrap<RcppDate>(const RcppDate& date) {
	Rcpp::NumericVector value(1);
	value[0] = date.getJulian();
	Rcpp::RObject robj((SEXP)value);
	robj.attr("class") = "Date";
	return value;
    }

    template<> SEXP wrap<RcppDatetime>(const RcppDatetime& date) {
	Rcpp::NumericVector value(1);
	Rcpp::CharacterVector classname(2);
	value[0] = date.getFractionalTimestamp();
	Rcpp::RObject robj((SEXP)value);
	classname[0] = Rcpp::datetimeClass[0];
	classname[1] = Rcpp::datetimeClass[1];
	robj.attr("class") = classname;
	return value;
    }

    template<> SEXP wrap<RcppVector<double> >(const RcppVector<double>& vec) {
	Rcpp::NumericVector value(vec.size());
	for(int i=0; i < (int)vec.size(); ++i)
	    value(i) = vec(i);
	return value;
    }

    template<> SEXP wrap<RcppVector<int> >(const RcppVector<int>& vec) {
	Rcpp::IntegerVector value(vec.size());
	for(int i=0; i < (int)vec.size(); ++i)
	    value(i) = vec(i);
	return value;
    }

    /*
    template<> SEXP wrap<RcppMatrix<double> >(const RcppMatrix<double>& mat) {
	int nrows = mat.getDim1();
	int ncols = mat.getDim2();
	Rcpp::NumericMatrix m(nrows, ncols);
	for(int i=0; i < nrows; ++i)
	    for(int j=0; j < ncols; ++j)
		m(i,j) = mat(i,j);
	return m;
    }

    template<> SEXP wrap<RcppMatrix<int> >(const RcppMatrix<int>& mat) {
	int nrows = mat.getDim1();
	int ncols = mat.getDim2();
	Rcpp::IntegerMatrix m(nrows, ncols);
	for(int i=0; i < nrows; ++i)
	    for(int j=0; j < ncols; ++j)
		m(i,j) = mat(i,j);
	return m;
    }
    */
}

// AppLayer.cpp: STL wrappers for R vectors
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

#include <AppLayer.hpp>

namespace cxxPack {

CNumericVector::CNumericVector(Rcpp::NumericVector nv) {
    if(nv.size() <= 0)
	throw std::range_error("CNumericVector: invalid size");
    vec.resize(nv.size());
    for(int i=0; i < nv.size(); ++i) vec[i] = nv(i);
}

CNumericVector::operator SEXP() {
    Rcpp::NumericVector nv(vec.size());
    for(int i=0; i < nv.size(); ++i) nv(i) = vec[i];
    return nv;
}

CNumericMatrix::CNumericMatrix(Rcpp::NumericMatrix nm) {
    if(nm.nrow() <=0 || nm.ncol() <= 0)
	throw std::range_error("CNumericMatrix: invalid size");
    mat.resize(nm.nrow());
    for(int i=0; i < nm.nrow(); ++i) {
	mat[i].resize(nm.ncol());
	for(int j=0; j < nm.ncol(); ++j)
	    mat[i][j] = nm(i,j);
    }
}

CNumericMatrix::operator SEXP() {
    Rcpp::NumericMatrix nm(nrow(), ncol());
    for(int i=0; i < nrow(); ++i)
	for(int j=0; j < ncol(); ++j)
	    nm(i,j) = mat[i][j];
    return nm;
}

CDateVector::CDateVector(Rcpp::NumericVector nv) {
    if(nv.size() <= 0)
	throw std::range_error("CDateVector: invalid size");
    vec.resize(nv.size());
    for(int i=0; i < nv.size(); ++i) vec[i] = FinDate(nv(i));
}

CDateVector::operator SEXP() {
    Rcpp::NumericVector nv(vec.size());
    for(int i=0; i < nv.size(); ++i) nv(i) = vec[i].getRValue();
    Rcpp::RObject(nv).attr("class") = "Date";
    return nv;
}

CDatetimeVector::CDatetimeVector(Rcpp::NumericVector nv) {
    if(nv.size() <= 0)
	throw std::range_error("CDatetimeVector: invalid size");
    vec.resize(nv.size());
    for(int i=0; i < nv.size(); ++i) vec[i] = RcppDatetime(nv(i));
}

CDatetimeVector::operator SEXP() {
    Rcpp::NumericVector nv(vec.size());
    for(int i=0; i < nv.size(); ++i) nv(i) = vec[i].getFractionalTimestamp();
    Rcpp::CharacterVector cv(2);
    cv(0) = Rcpp::datetimeClass[0];
    cv(1) = Rcpp::datetimeClass[1];
    Rcpp::RObject(nv).attr("class") = cv;
    return nv;
}

}

namespace Rcpp {

// Work-aruond for the fact that Rcpp::wrap(obj) does not work when
// obj is an object of a class that has operator SEXP().
template<> SEXP wrap<cxxPack::CDateVector>(const cxxPack::CDateVector& dv) {
    cxxPack::CDateVector *p = const_cast<cxxPack::CDateVector*>(&dv);
    return (SEXP)*p;
}
template<> SEXP wrap<cxxPack::CDatetimeVector>(const cxxPack::CDatetimeVector& dv) {
    cxxPack::CDatetimeVector *p = const_cast<cxxPack::CDatetimeVector*>(&dv);
    return (SEXP)*p;
}
template<> SEXP wrap<cxxPack::CNumericVector>(const cxxPack::CNumericVector& dv)	 {
    cxxPack::CNumericVector *p = const_cast<cxxPack::CNumericVector*>(&dv);
    return (SEXP)*p;
}
template<> SEXP wrap<cxxPack::CNumericMatrix>(const cxxPack::CNumericMatrix& dv)	 {
    cxxPack::CNumericMatrix *p = const_cast<cxxPack::CNumericMatrix*>(&dv);
    return (SEXP)*p;
}

} // end namespace Rcpp

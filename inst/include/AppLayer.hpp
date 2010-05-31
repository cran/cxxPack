// AppLayer.hpp: STL wrappers for R vectors
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

#ifndef APPLAYER_HPP
#define APPLAYER_HPP

#include <Rcpp.h>
#include <FinDate.hpp>

namespace cxxPack {

// Experimental application layer wrappers. The advantage of using STL
// here is that there is no need to implement copy constructors.

class CNumericVector {
public:
    CNumericVector(Rcpp::NumericVector nv);
    inline int size() { return vec.size(); }
    inline double& operator()(int i) { 
	if(i < 0 || i >= (int)vec.size())
	    throw std::range_error("CNumericVector: index out of range");
	return vec[i]; 
    }
    operator SEXP();
private:
    std::vector<double> vec;
};

class CNumericMatrix {
public:
    CNumericMatrix(Rcpp::NumericMatrix nm);
    inline int nrow() { return mat.size(); }
    inline int ncol() { return mat[0].size(); }
    inline double& operator()(int i, int j) {
	if(i < 0 || i >= (int)mat.size() || j < 0 || j > (int)mat[0].size())
	    throw std::range_error("CNumericMatrix: index out of range");
	return mat[i][j]; 
    }
    operator SEXP();
private:
    std::vector<std::vector<double> > mat;
};

class CDateVector {
public:
    CDateVector(Rcpp::NumericVector nv);
    inline int size() { return vec.size(); }
    inline FinDate& operator()(int i) { 
	if(i < 0 || i >= (int)vec.size())
	    throw std::range_error("CDateVector: index out of range");
	return vec[i]; 
    }
    operator SEXP();
private:
    std::vector<FinDate> vec;
};

class CDatetimeVector {
public:
    CDatetimeVector(Rcpp::NumericVector nv);
    inline int size() { return vec.size(); }
    inline RcppDatetime& operator()(int i) { 
	if(i < 0 || i >= (int)vec.size())
	    throw std::range_error("CDatetimeVector: index out of range");
	return vec[i]; 
    }
    operator SEXP();
private:
    std::vector<RcppDatetime> vec;
};

} // end namespace cxxPack

namespace Rcpp {

// Work-aruond for the fact that Rcpp::wrap(obj) does not work when
// obj is an object of a class that has operator SEXP().
template<> SEXP wrap<cxxPack::CNumericVector>(const cxxPack::CNumericVector&);
template<> SEXP wrap<cxxPack::CNumericMatrix>(const cxxPack::CNumericMatrix&);
template<> SEXP wrap<cxxPack::CDateVector>(const cxxPack::CDateVector&);
template<> SEXP wrap<cxxPack::CDatetimeVector>(const cxxPack::CDatetimeVector&);

} // end namespace Rcpp

#endif

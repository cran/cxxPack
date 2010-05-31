// cxxUtils.hpp: misc utilities
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

#ifndef cxxUtils_HPP
#define cxxUtils_HPP

#include <string>
#include <vector>

#include <Rcpp.h>

#include <FinDate.hpp>

/**
 * Returns the string value of an object that knows how to stream itself.
 */
namespace cxxPack {

// Convert objects that have operator<<() to a string.
template <typename T>
std::string to_string(const T& obj) {
    std::stringstream ss;
    ss << obj;
    return ss.str();
}


// Using the following functions to get the dimensions of an R vector
// or matrix is deprecated because this is available more naturally
// with the help of Rcpp::NumericVector and Rcpp::NumericMatrix classes
// They are needed in some older code that was written before these
// classes were available.
std::vector<int> getRDims(SEXP s);
void setRDims(SEXP s, std::vector<int>& dims);

} // end of namespace cxxPack

namespace Rcpp {

    extern std::string datetimeClass[];

    // Wraps for some of the "Classic API" types.
    template<> SEXP wrap<RcppDate>(const RcppDate&);
    template<> SEXP wrap<RcppDatetime>(const RcppDatetime&);
    template<> SEXP wrap<RcppDateVector>(const RcppDateVector&);
    template<> SEXP wrap<RcppDatetimeVector>(const RcppDatetimeVector&);

    template<> SEXP wrap<RcppVector<double> >(const RcppVector<double>&);
    template<> SEXP wrap<RcppVector<int> >(const RcppVector<int>&);
    template<> SEXP wrap<RcppMatrix<double> >(const RcppMatrix<double>&);
    template<> SEXP wrap<RcppMatrix<int> >(const RcppMatrix<int>&);

} // end of namespace Rcpp

#endif

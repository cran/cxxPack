// cxxMath.hpp: math functions
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

#ifndef cxxMathhpp
#define cxxMathhpp

#include <Rcpp.h>

namespace cxxPack {

    /**
     * Complex Gamma function.
     */
    std::complex<double> cgamma(std::complex<double> z);

    /**
     * C++ interfaces to R's fft(). A similar interface to FFTW could
     * be provided, but this would create a dependency on those libs.
     */
    Rcpp::ComplexVector fft1d(Rcpp::ComplexVector invec, bool reverse=false);
    Rcpp::ComplexMatrix fft2d(Rcpp::ComplexMatrix invec, bool reverse=false);
    std::vector<std::complex<double> > 
    fft1d(std::vector<std::complex<double> > invec, bool reverse=false);
    std::vector<std::vector<std::complex<double> > >
    fft2d(std::vector<std::vector<std::complex<double> > > invec, bool reverse=false);

    /**
     * Fast pow() when the exponent is integer.
     */
    double pown(double, int);
}

#endif

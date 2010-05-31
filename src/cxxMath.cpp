// cxxMath.cpp: math functions
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

#include <limits>
#include <Rcpp.h>
#include <complex>
#include <algorithm>

#include <cxxMath.hpp>

namespace cxxPack {

// More p's can be included for increased accuracy.
static double pi = 3.14159265358979323846264338327950288;
static double p[] = {0.99999999999980993, 676.5203681218851, 
		     -1259.1392167224028, 771.32342877765313, 
		     -176.61502916214059, 12.507343278686905,
		     -0.13857109526572012, 9.9843695780195716e-6, 
		     1.5056327351493116e-7 };

/**
 * Complex gamma function using Lanczos approximation (see Wikipedia).
 * Uses the reflection formula: Gamma(z) Gamma(1-z) = pi/sin(pi*z) when
 * Re(z) < 0.5.
 */
std::complex<double> cgamma(std::complex<double> z) {

    int g = 7;

    // Reflection formula: Gamma(1-z) Gamma(z) = pi/sin(pi z).
    if(z.real() < 0.5)
	return pi/(sin(pi*z)*cgamma(1.0-z));

    z -= 1;
    std::complex<double> x = p[0];
    for(int i = 1; i < g+2; ++i) {
	x += p[i]/(z+(double)i);
    }
    std::complex<double> t = z + 0.5 + (double)g;
    return sqrt(2*pi)*pow(t,z+0.5)*exp(-t)*x;
}

/**
 * R interface to cgamma(complex<double>).
 */
RcppExport SEXP cgamma_(SEXP inRvec) {

    Rcpp::ComplexVector in_cv, out_cv;
    Rcpp::ComplexMatrix in_cm, out_cm;
    Rcomplex *inRptr=0, *outRptr=0;

    // Setup pointers to data as a linear vector (even when matrix).
    int len=0;
    if(Rf_isMatrix(inRvec)) {
	in_cm = Rcpp::ComplexMatrix(inRvec);
	out_cm = Rcpp::ComplexMatrix(in_cm.nrow(),in_cm.ncol());
	len = in_cm.nrow()*in_cm.ncol();
	inRptr = COMPLEX(in_cm);
	outRptr = COMPLEX(out_cm);
    }
    else {
	in_cv = Rcpp::ComplexVector(inRvec);
	out_cv = Rcpp::ComplexVector(in_cv.size());
	len = in_cv.size();
	inRptr = COMPLEX(in_cv);
	outRptr = COMPLEX(out_cv);
    }

    // Get C++ pointers.
    std::complex<double>* inCptr = reinterpret_cast<std::complex<double>*>(inRptr);
    std::complex<double>* outCptr = reinterpret_cast<std::complex<double>*>(outRptr);

    // Do the computations and return the output ComplexVector.
    std::transform(&inCptr[0], &inCptr[len], &outCptr[0], cgamma);

    return Rf_isMatrix(inRvec) ? out_cm : out_cv;
}

/**
 * pown() computes x^n (x to the n-th power) by using Algorithm A from
 * Donald Knuth's "The Art of Computer Programming," Volume 2
 * (Seminumerical Algorithms), Second Edition, Page 442. (Knuth
 * notes that this algorithm dates from 1414 A.D., at least.) 
 *
 * The algorithm is based on the idea that you can compute x^n
 * faster by repeated squaring and multiplication by x instead of
 * by multiplying x by itself n times (time proportional to log n
 * instead of n). The correctness of the algorithm is easy to
 * check by noting that the quantity y * z^m is an invariant of
 * the loop, is equal to x^n before the first iteration, and the
 * loop terminates with m == 0 or m == 1. If m == 0, y == x^n, the
 * desired result. Otherwise, y * z == x^n.
*/
double pown(double x, int n) {
	register int m = n, neg = (m < 0) ;
	double y = 1.0, z = x ;
	if(neg)
		m = -m ;
	while(m > 1) {

		/* Loop invariant: y * z^m = x^n */
		if(m & 1) y *= z ;
		z *= z ;
		m >>= 1 ;
	}

	/* Here y * z^m = x^n, and m == 0 or 1. If m == 0, y == x^n
	   and we are done. Otherwise, y * z == x^n. */
	if(m == 1) y *= z ;
	if(neg)
		y = 1.0/y ;
	return(y) ;
}

/**
 * Various C++ interfaces to R's fft function.
 */
Rcpp::ComplexVector fft1d(Rcpp::ComplexVector invec, bool reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    return fft(invec, reverse);
}

Rcpp::ComplexMatrix fft2d(Rcpp::ComplexMatrix invec, bool reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    return fft(invec, reverse);
}

std::vector<std::complex<double> > 
fft1d(std::vector<std::complex<double> > invec, bool reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    int len = invec.size();
    Rcpp::ComplexVector in_cv(len);
    for(int i=0; i < len; ++i) {
	in_cv[i].r = invec[i].real();
	in_cv[i].i = invec[i].imag();
    }
    Rcpp::ComplexVector out_cv = fft(in_cv, reverse);
    std::vector<std::complex<double> > outvec(len);
    for(int i=0; i < len; ++i) {
	outvec[i].real() = out_cv[i].r;
	outvec[i].imag() = out_cv[i].i;
    }
    return outvec;
}

std::vector<std::vector<std::complex<double> > >
fft2d(std::vector<std::vector<std::complex<double> > > invec, bool reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    int nrow = invec.size();
    int ncol = invec[0].size();
    Rcpp::ComplexMatrix in_mat(nrow, ncol);
    for(int i=0; i < nrow; ++i)
	for(int j=0; j < ncol; ++j) {
	    in_mat(i,j).r = invec[i][j].real();
	    in_mat(i,j).i = invec[i][j].imag();
	}
    Rcpp::ComplexMatrix out_mat = fft(in_mat, reverse);
    std::vector<std::vector<std::complex<double> > > outvec(nrow);
    for(int i=0; i < nrow; ++i) {
	outvec[i].resize(ncol);
	for(int j=0; j < ncol; ++j) {
	    outvec[i][j].real() = out_mat(i,j).r;
	    outvec[i][j].imag() = out_mat(i,j).i;
	}
    }
    return outvec;
}

/**
 * These functions expose R's fft to R so they are only
 * for testing purposes! Use ff() directly from R.
 */
RcppExport SEXP fft1d(SEXP invec, SEXP reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    return fft(invec, Rcpp::as<bool>(reverse));
}

RcppExport SEXP fft2d(SEXP invec, SEXP reverse) {
    Rcpp::Environment stats("package:stats");
    Rcpp::Function fft = stats.get("fft");
    return fft(invec, Rcpp::as<bool>(reverse));
}

} // end cxxPack namespace



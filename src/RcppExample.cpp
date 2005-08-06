// RQuantLib function RcppExample
//
// Copyright 2005 Dominick Samperi
//
// This program is part of the Rcpp R/C++ interface library for R (GNU S).
// It is made available under the terms of the GNU General Public
// License, version 2, or at your option, any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more
// details.

#include "Rcpp.hpp"

#include "MyLib.hpp"

RcppExport SEXP Rcpp_Example(SEXP params, SEXP nlist, SEXP vec, SEXP mat) {

    SEXP rl=0;

    try {

	Adder sum;
	sum.addTo(1);
	sum.addTo(2);
	Rprintf("sum = %lf\n", sum.getValue());

	Multiplier product;
	product.multiplyBy(5);
	Rprintf("product = %lf\n", product.getValue());

	int i=0, j=0;

	// Get parameters in params.
	RcppParams rparam(params);
	string optType = rparam.getStringValue("optType");
	int    settleDays = rparam.getIntValue("settleDays");
	double strikePrice = rparam.getDoubleValue("strikePrice");
	double price = rparam.getDoubleValue("price");
	double volatility = rparam.getDoubleValue("volatility");
	double riskFreeRate = rparam.getDoubleValue("riskFreeRate");
	double dividendYield = rparam.getDoubleValue("dividendYield");
	Date   tradeDate  = rparam.getDateValue("tradeDate");
	Date   exerciseDate = rparam.getDateValue("exerciseDate");

	// Wrap named list nlist. Use nl.getLength(), nl.getName(i),
	// and nl.getValue(i) to fetch data.
	RcppNamedList nl(nlist);

	// vec parameter viewed as vector of ints (with truncation).
	RcppVector<int> vecI(vec);

	// vec parameter viewed as vector of doubles.
	RcppVector<double> vecD(vec);

	// mat parameter viewed as matrix of ints (with truncation).
	RcppMatrix<int> matI(mat);

	// mat parameter viewed as matrix of doubles.
	RcppMatrix<double> matD(mat);

	// Do some computations with the matrices.
	int nx = matD.getDim1();
	int ny = matD.getDim2();
	for(i = 0; i < nx; i++)
	    for(j = 0; j < ny; j++)
		matD(i,j) = 2 * matD(i,j);

	int len = vecD.getLength();
	for(i = 0; i < len; i++)
	    vecD(i) = 3 * vecD(i);

	// Get copy of matrix/vector in standard (unchecked) C/C++ format.
	// May be useful when these vectors need to be passed to
	// C/C++ code that does not know about Rcpp classes.
	double **a = matD.cMatrix();
	double  *v = vecD.cVector();

	// Get a zero matrix the same size as matD.
	RcppMatrix<double> matZ(nx, ny);

	// Build result set to be returned as a list to R.
	RcppResultSet rs;

	rs.add("nlFirstName", nl.getName(0));
	rs.add("nlFirstValue", nl.getValue(0));
	rs.add("matD", matD);
	rs.add("vecD", vecD);
	rs.add("a", a, nx, ny);
	rs.add("v", v, len);
	rs.add("settleDays", settleDays);
	rs.add("price", price);
	rs.add("riskFreeRate", riskFreeRate);
	rs.add("dividendYield", dividendYield);

	ostringstream osExerciseDate, osTradeDate;
	osExerciseDate << exerciseDate;
	osTradeDate << tradeDate;

	ostringstream os;
	os << tradeDate;
	Rprintf("Trade Date = %s\n", os.str().c_str());
	os.seekp(ios::beg);
	os << exerciseDate;
	Rprintf("Exercise Date = %s\n", os.str().c_str());

	rs.add("optType", optType);
	rs.add("volatility", volatility);
	rs.add("strikePrice", strikePrice);
	rs.add("tradeDate", osTradeDate.str());
	rs.add("exerciseDate", osExerciseDate.str());

	// Instead of returning selected input parameters as we did in
	// the last three statements, the entire input parameter list
	// can be returned like this:
	// rs.add("params", params, false);

	// Get the list to be returned to R.
	rl = rs.getReturnList();
	
    } catch(std::exception& e) {
	error("Exception: %s\n", e.what());
    } catch(...) {
	error("Exception: unknown reason\n");
    }
    
    return rl;
}

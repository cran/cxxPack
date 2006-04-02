// RcppExample.cpp: Part of the R/C++ interface class library, Version 3.0
//
// Copyright (C) 2005-2006 Dominick Samperi
//
// This library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published by 
// the Free Software Foundation; either version 2.1 of the License, or (at 
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License 
// along with this library; if not, write to the Free Software Foundation, 
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

#include "Rcpp.hpp"

RcppExport SEXP Rcpp_Example(SEXP params, SEXP nlist, SEXP vec, SEXP mat) {

    SEXP  rl=0;
    char* exceptionMesg=NULL;

    try {

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
	// C/C++ code that does not know about Rcpp classes...
	double **a = matD.cMatrix();
	double  *v = vecD.cVector();

	// ...or we might want to use an STL container...
	vector<double> stlvec = vecD.stlVector();
	nx = stlvec.size();
	for(i = 0; i < nx; i++)
	    stlvec[i] += 1;

	// ...or perhaps a container of containers.
	vector<vector<double> > stlmat = matD.stlMatrix();
	nx = stlmat.size();
	ny = stlmat[0].size();
	for(i = 0; i < nx; i++)
	    for(j = 0; j < ny; j++)
		stlmat[i][j] += 2;

	// Get a zero matrix the same size as matD.
	RcppMatrix<double> matZ(nx, ny);

	// Build result set to be returned as a list to R.
	RcppResultSet rs;

	rs.add("nlFirstName", nl.getName(0));
	rs.add("nlFirstValue", nl.getValue(0));
	rs.add("matD", matD);
	rs.add("stlvec", stlvec);
	rs.add("stlmat", stlmat);
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
	
    } catch(std::exception& ex) {
	exceptionMesg = copyMessageToR(ex.what());
    } catch(...) {
	exceptionMesg = copyMessageToR("unknown reason");
    }
    
    if(exceptionMesg != NULL)
	error(exceptionMesg);

    return rl;
}

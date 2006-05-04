// RcppExample.cpp: Part of the R/C++ interface class library, Version 4.0
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

/*
 * Sample function illustrates how to use the Rcpp R/C++ interface library.
 */
RcppExport SEXP Rcpp_Example(SEXP params, SEXP nlist, SEXP vec, SEXP mat,
			     SEXP df) {

    SEXP  rl=R_NilValue; // Use this when there is nothing to be returned.
    char* exceptionMesg=NULL;

    try {

	int i=0, j=0;

	// Get parameters in params.
	RcppParams rparam(params);
	string method = rparam.getStringValue("method");
	double tolerance = rparam.getDoubleValue("tolerance");
	int    maxIter = rparam.getIntValue("maxIter");

	//Date class is defined only when USING_QUANTLIB is set. In this case
	//dates can be input as vectors of length three on the R side in
	//the form c(month, day, year), and the corresponding QuantLib date is
	//obtained using code like this:
	//Date   tradeDate  = rparam.getDateValue("tradeDate");
	//See RQuantLib package function discount.cpp for more info.

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
	int nrows = matD.getDim1();
	int ncols = matD.getDim2();
	for(i = 0; i < nrows; i++)
	    for(j = 0; j < ncols; j++)
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
	vector<double> stlvec(vecD.stlVector());
	nrows = (int)stlvec.size();
	for(i = 0; i < nrows; i++)
	    stlvec[i] += 1;

	// ...or perhaps a container of containers.
	vector<vector<double> > stlmat(matD.stlMatrix());
	nrows = (int)stlmat.size();
	ncols = (int)stlmat[0].size();
	for(i = 0; i < nrows; i++)
	    for(j = 0; j < ncols; j++)
		stlmat[i][j] += 2;

	// Get a zero matrix the same size as matD.
	RcppMatrix<double> matZ(nrows, ncols);

	// Make a vector of strings
 	vector<string> vec(2);
        vec[0] = "hello";
	vec[1] = "world";

	RcppFrame inframe(df); // we will simply pass this back to the user.

	// Make a pre-data frame, that is, a list object that when passed
	// the the R function data.frame() will return a data frame with
	// the specified column names and data types. The first row added
	// determines the types for all columns. It would be safer
	// to use the vector at() function instead of the unchecked [] here.
	vector<string> colNames(3);
	colNames[0] = "alpha"; // column of strings
	colNames[1] = "beta";  // column of reals
	colNames[2] = "gamma"; // column of ints
	RcppFrame frame(colNames);

	// First row (this one determines column types).
	vector<ColDatum> row1(3);
	row1[0].setStringValue("a");
	row1[1].setDoubleValue(3.14);
	row1[2].setLogicalValue(1);
	frame.addRow(row1);

	// Second row.
	vector<ColDatum> row2(3);
	row2[0].setStringValue("b");
	row2[1].setDoubleValue(6.28);
	row2[2].setLogicalValue(0);
	frame.addRow(row2);

	// Build result set to be returned as a list to R.
	RcppResultSet rs;
	
	rs.add("method", method);
	rs.add("tolerance", tolerance);
	rs.add("maxIter", maxIter);
	rs.add("nlFirstName", nl.getName(0));
	rs.add("nlFirstValue", nl.getValue(0));
	rs.add("matD", matD);
	rs.add("stlvec", stlvec);
	rs.add("stlmat", stlmat);
	rs.add("a", a, nrows, ncols);
	rs.add("v", v, len);

	rs.add("strings", vec);

	rs.add("InputDF", inframe);
	rs.add("PreDF", frame);

	// Instead of returning selected input parameters as we did in
	// the last three statements, the entire input parameter list
	// can be returned like this:
	rs.add("params", params, false);

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

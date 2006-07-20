// RcppExample.cpp: Part of the R/C++ interface class library, Version 4.2
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
RcppExport SEXP Rcpp_Example(SEXP params, SEXP nlist, 
			     SEXP numvec, SEXP nummat,
			     SEXP df, SEXP datevec, SEXP stringvec) {

    SEXP  rl=R_NilValue; // Use this when there is nothing to be returned.
    char* exceptionMesg=NULL;

    try {

	int i=0, j=0;

	// Get parameters in params.
	RcppParams rparam(params);
	string method = rparam.getStringValue("method");
	double tolerance = rparam.getDoubleValue("tolerance");
	int    maxIter = rparam.getIntValue("maxIter");
	RcppDate startDate = rparam.getDateValue("startDate");

	// The output of commands like this may not appear under Windows.
	cout << "Start Date: " << startDate << endl;

	// QuantLib note: an RcppDate is automatically converted to QuantLib
	// Date when the context calls for this, provided 
	// USING_QUANTLIB is set.

	RcppDateVector dateVec(datevec);
	//dateVec(0) = RcppDate(12, 15, 1989); // update one element.
	//for(int i = 0; i < dateVec.size(); i++)
	//   cout << dateVec(i) << endl;

	RcppStringVector stringVec(stringvec);
	//stringVec(1) = string("New York"); // update one element.
	//for(int i = 0; i < stringVec.size(); i++)
	//  cout << stringVec(i) << endl;

	// and nl.getValue(i) to fetch data.
	RcppNamedList nl(nlist);

	// numvec parameter viewed as vector of ints (with truncation).
	//RcppVector<int> vecI(numvec);

	// mat parameter viewed as matrix of ints (with truncation).
	//RcppMatrix<int> matI(nummat);

	// vec parameter viewed as vector of doubles.
	RcppVector<double> vecD(numvec);

	// mat parameter viewed as matrix of doubles.
	RcppMatrix<double> matD(nummat);

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
	//RcppMatrix<double> matZ(nrows, ncols);

	// Make a vector of strings
 	vector<string> vec(2);
        vec[0] = "hello";
	vec[1] = "world";

	// Process the input data frame and show factors and dates.
	RcppFrame inframe(df);

	/*
	cout << "\nFactors and Dates in frame..." << endl;
	vector<vector<ColDatum> > table = inframe.getTableData();
	int nrow = table.size();
	int ncol = table[0].size();
	for(int row=0; row < nrow; row++) {
	    for(int col=0; col < ncol; col++) {
		RcppDate d;
		string name;
		int level;
		switch(table[row][col].getType()) {
		case COLTYPE_FACTOR:
		    level = table[row][col].getFactorLevel();
		    name  = table[row][col].getFactorLevelName();
		    cout << "Factor level, name: " 
			 << level << ", "
			 << name << endl;
		    break;
		case COLTYPE_DATE:
		    d = table[row][col].getDateValue();
		    cout << "Date: " << d << endl;
		    break;
		default:
		    ; // Ignore other types.
		}
	    }
	}
	*/

	// Make a pre-data frame, that is, a list object that when passed
	// the the R function data.frame() will return a data frame with
	// the specified column names and data types. The first row added
	// determines the types for all columns.
	int numCol=4;
	vector<string> colNames(numCol);
	colNames[0] = "alpha"; // column of strings
	colNames[1] = "beta";  // column of reals
	colNames[2] = "gamma"; // factor column
	colNames[3] = "delta"; // column of Dates
	RcppFrame frame(colNames);

	// Third column will be a factor. In the current implementation the
	// level names are copied to every factor value (and factors
	// in the same column must have the same level names). The level names
	// for a particular column will be factored out (pardon the pun) in
	// a future release.
	int numLevels = 2;
	string *levelNames = new string[2];
	levelNames[0] = string("pass"); // level 1
	levelNames[1] = string("fail"); // level 2

	// First row (this one determines column types).
	vector<ColDatum> row1(numCol);
	row1[0].setStringValue("a");
	row1[1].setDoubleValue(3.14);
	row1[2].setFactorValue(levelNames, numLevels, 1);
	row1[3].setDateValue(RcppDate(7,4,2006));
	frame.addRow(row1);

	// Second row.
	vector<ColDatum> row2(numCol);
	row2[0].setStringValue("b");
	row2[1].setDoubleValue(6.28);
	row2[2].setFactorValue(levelNames, numLevels, 1);
	row2[3].setDateValue(RcppDate(12,25,2006));
	frame.addRow(row2);

	// Done with levelNames.
	delete [] levelNames;

	RcppDate aDate(12, 25, 1999);

	// Build result set to be returned as a list to R.
	RcppResultSet rs;

	rs.add("date", aDate);
	rs.add("dateVec", dateVec);
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
	rs.add("stringVec", stringVec);
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

// RcppExample.cpp: Sample client for Rcpp R/C++ Object Mapping Library
//
// Copyright (C) 2005-2009 Dominick Samperi
//
// As an extension of R this software is naturally released under the 
// same terms, namely, the GNU General Public License Version 2.
// For more information go to http://www.gnu.org/licenses/gpl.html.

#include <algorithm> // used for one example, not required to use Rcpp.

#include "Rcpp.hpp"

// Include system-dependent configuration information generated
// automatically by the configure script.
#include "config.hpp"

using namespace std;
using namespace Rcpp;

// Get monthly dates between start and end that fall on the nth
// occurrence of the specified weekday in each month. (S&P index
// options expire on the Saturday immediately following the third
// Friday of the expiration month, so this exercise is not 
// entirely useless.)
static list<RcppDate> getSchedule(RcppDate start, RcppDate end, 
			          int n, int weekday) {
    list<RcppDate> dateList;
    int nextMonth, nextYear;
    RcppDate date = start.nthWeekday(n, weekday);
    while(date <= end) {
	if(date >= start) // could have nthWeekday < start in first month.
	    dateList.push_back(date);
	RcppLocalTime lt(date);
	if(lt.getMonth() == RcppDate::Dec) {
	    nextMonth = 1;
	    nextYear = lt.getYear()+1;
	}
	else {
	    nextMonth = lt.getMonth()+1;
	    nextYear = lt.getYear();
	}
	date = RcppDate(nextMonth,1,nextYear); // beginning of next month
	date = date.nthWeekday(n, weekday); // nth occurrence in that month
    }
    return dateList;
}

// Returns a vector of RcppZoo objects with different index types and with
// vector and matrix data. 
static vector<RcppZoo> getZooObjects() {

	int nrows = 5;
	int ncols = 3;

	vector<double> vd(nrows); // vector data
	vector<vector<double> > vvd(nrows); // matrix data
	for(int i = 0; i < nrows; ++i) {
	    vd[i] = i;
	    vector<double> col(ncols);
	    for(int j = 0; j < ncols; ++j)
		col[j] = i+j;
	    vvd[i] = col;
	}

	vector<int> indInt(nrows); // int ordered index
	vector<double> indDouble(nrows); // double ordered index
	vector<RcppDate> indDate(nrows); // Date ordered index
	vector<RcppDateTime> indDateTime(nrows); // DateTime ordered index
	vector<int> shuffle(nrows);

	RcppDate startDate(11,1,2009); // 11/1/2009
	RcppDateTime startDateTime(11,1,2009,.75); // 11/1/2009 18:00

	// Shuffle index vectors to test sorting of RcppZoo data rows on
	// output to R.
	for(int i = 0; i < nrows; ++i)
	    shuffle[i] = i;
	random_shuffle(shuffle.begin(), shuffle.end());
	Rprintf("shuffle: ");
	for(int i = 0; i < nrows; ++i) {
	    indInt[shuffle[i]] = i;
	    indDouble[shuffle[i]] = i+.5;
	    indDate[shuffle[i]] = startDate + i;
	    indDateTime[shuffle[i]] = startDateTime + i;
	    Rprintf("%d ", shuffle[i]);
	}
	Rprintf("\n");

	//indDouble[0] += .05; // this would cause freq=1 to be rejected.
	RcppZoo zooVecIntIndex(vd, indInt,1); // freq will be accepted
	RcppZoo zooVecDoubleIndex(vd, indDouble,1);
	RcppZoo zooVecDateIndex(vd, indDate);
	RcppZoo zooVecDateTimeIndex(vd, indDateTime);

	RcppZoo zooMatIntIndex(vvd, indInt);
	RcppZoo zooMatDoubleIndex(vvd, indDouble);
	RcppZoo zooMatDateIndex(vvd, indDate);
	RcppZoo zooMatDateTimeIndex(vvd, indDateTime);

	vector<RcppZoo> v;
	v.push_back(zooVecIntIndex);
	v.push_back(zooVecDoubleIndex);
	v.push_back(zooVecDateIndex);
	v.push_back(zooVecDateTimeIndex);
	v.push_back(zooMatIntIndex);
	v.push_back(zooMatDoubleIndex);
	v.push_back(zooMatDateIndex);
	v.push_back(zooMatDateTimeIndex);
	return v;
}

// Illustrates how to construct an RcppFrame from C++ data structures that
// can be passed back to R as a data.frame object.
static RcppFrame getDataFrame() {
    
    int numCol=7;
    int numRow=5;
    vector<string> colNames(numCol);
    vector<string> rowNames(numRow);
    vector<RcppColumn> cols;

    colNames[0] = "IntCol"; // column of ints
    colNames[1] = "DoubleCol";  // column of double's
    colNames[2] = "StringCol"; // column of strings
    colNames[3] = "FactorCol"; // column of factors
    colNames[4] = "BoolCol";   // column of bool's
    colNames[5] = "DateCol";   // column of Date's
    colNames[6] = "DateTimeCol"; // column of POSIXt times

    // Five observations of a 2-level factor:
    vector<string> factorData(numRow);
    factorData[0] = "pass";
    factorData[1] = "pass";
    factorData[2] = "fail";
    factorData[3] = "pass";
    factorData[4] = "fail";

#define RcppColumn_Make_Copy

#ifdef RcppColumn_Make_Copy

    // When an RcppColumn constructor is handed a reference to a vector
    // it will make a copy and NOT assume that the original vector will
    // remain in scope. It will also make a copy in the usual pass-by-value
    // situations. To reduce the amount of copying pointers to vectors can
    // be passed to the constructors instead (see the other branch of this
    // ifdef). In this case RcppColumn keeps a pointer to the original data
    // and assumes that the client will delete this data at the right time,
    // introducing some risk for a little more efficiency. Of course, smart
    // pointers would make a better solution.

    RcppFactor myfactor(factorData);

    // Other variables observed.
    vector<int> intData(numRow); // integer observations
    vector<double> doubleData(numRow); // double observations
    vector<string> stringData(numRow);
    vector<bool> boolData(numRow);
    vector<RcppDate> dateData(numRow);
    vector<RcppDateTime> dateTimeData(numRow);

    // Fill with dummy observations.
    for(int i = 0; i < numRow; ++i) {
	rowNames[i] = to_string(i+1);
	intData[i] = i;
	doubleData[i] = i + 3.14;
	stringData[i] = "A" + to_string(i);
	boolData[i] = (i%2 == 0);
	dateData[i] = RcppDate(10,1,2009) + i;
	dateTimeData[i] = RcppDateTime(10,1,2009,.75) + i;
    }

#else // Client takes responsibility for freeing memory here

    RcppFactor *myfactor = new RcppFactor(factorData);

    vector<int>* intData = new vector<int>(numRow);
    vector<double>* doubleData = new vector<double>(numRow);
    vector<string>* stringData = new vector<string>(numRow);
    vector<bool>* boolData = new vector<bool>(numRow);
    vector<RcppDate>* dateData = new vector<RcppDate>(numRow);
    vector<RcppDateTime>* dateTimeData = new vector<RcppDateTime>(numRow);

    // Fill with dummy observations.
    for(int i = 0; i < numRow; ++i) {
	rowNames[i] = to_string(i+1);
	(*intData)[i] = i;
	(*doubleData)[i] = i + 3.14;
	(*stringData)[i] = "A" + to_string(i);
	(*boolData)[i] = (i%2 == 0);
	(*dateData)[i] = RcppDate(10,1,2009) + i;
	(*dateTimeData)[i] = RcppDateTime(10,1,2009,.75) + i;
    }

#endif

    // Prepare columns and build the data frame.
    cols.push_back(RcppColumn(intData));
    cols.push_back(RcppColumn(doubleData));
    cols.push_back(RcppColumn(stringData));
    cols.push_back(RcppColumn(boolData));
    cols.push_back(RcppColumn(myfactor));
    cols.push_back(RcppColumn(dateData));
    cols.push_back(RcppColumn(dateTimeData));

    return RcppFrame(rowNames, colNames, cols);
}

static void printZooTS(RcppZoo& zoo) {

    // The permutation is used to display the TS
    // in index sorted order.
    vector<int> perm = zoo.getSortPermutation();
    bool isMatrix = zoo.isMatrix();
    bool isRegular = zoo.isRegular();
    if(isRegular)
	Rprintf("Regular Zoo TS with freq %d\n", zoo.getFrequency());
    else
	Rprintf("Zoo TS is not regular\n");

    switch(zoo.getIndType()) {
    case RcppZoo::IND_INTEGER: {
	vector<int> indInt = zoo.getIndInt();
	if(isMatrix) {
	    vector<vector<double> > m = zoo.getDataMat();
	    for(int i=0; i < (int)indInt.size(); ++i) {
		Rprintf("%d ", indInt[perm[i]]);
		for(int j=0; j < (int)m[0].size(); ++j)
		    Rprintf("%lf ", m[perm[i]][j]);
		Rprintf("\n");
	    }
	}
	else {
	    vector<double> v = zoo.getDataVec();
	    for(int i=0; i < (int)indInt.size(); ++i)
		Rprintf("%d ", indInt[perm[i]]);
	    Rprintf("\n");
	    for(int j=0; j < (int)v.size(); ++j)
		Rprintf("%lf ", v[perm[j]]);
	    Rprintf("\n");
	}
	break;
    }
    case RcppZoo::IND_DOUBLE: {
	vector<double> indDouble = zoo.getIndDouble();
	if(isMatrix) {
	    vector<vector<double> > m = zoo.getDataMat();
	    for(int i=0; i < (int)indDouble.size(); ++i) {
		Rprintf("%lf ", indDouble[perm[i]]);
		for(int j=0; j < (int)m[0].size(); ++j)
		    Rprintf("%lf ", m[perm[i]][j]);
		Rprintf("\n");
	    }
	}
	else {
	    vector<double> v = zoo.getDataVec();
	    for(int i=0; i < (int)indDouble.size(); ++i)
		Rprintf("%lf ", indDouble[perm[i]]);
	    Rprintf("\n");
	    for(int j=0; j < (int)v.size(); ++j)
		Rprintf("%lf ", v[perm[j]]);
	    Rprintf("\n");
	}
	break;
    }
    case RcppZoo::IND_DATE: {
	vector<RcppDate> indDate = zoo.getIndDate();
	if(isMatrix) {
	    vector<vector<double> > m = zoo.getDataMat();
	    for(int i=0; i < (int)indDate.size(); ++i) {
		string temp = to_string(indDate[perm[i]]);
		Rprintf("%s ", temp.c_str());
		for(int j=0; j < (int)m[0].size(); ++j) {
		    Rprintf("%lf ", m[perm[i]][j]);
		}
		Rprintf("\n");
	    }
	}
	else {
	    vector<double> v = zoo.getDataVec();
	    for(int i=0; i < (int)indDate.size(); ++i) {
		string temp = to_string(indDate[perm[i]]);
		Rprintf("%s ", temp.c_str());
	    }
	    Rprintf("\n");
	    for(int j=0; j < (int)v.size(); ++j)
		Rprintf("%lf ", v[perm[j]]);
	    Rprintf("\n");
	}
	break;
    }
    case RcppZoo::IND_DATETIME: {
	vector<RcppDateTime> indDateTime = zoo.getIndDateTime();
	if(isMatrix) {
	    vector<vector<double> > m = zoo.getDataMat();
	    for(int i=0; i < (int)indDateTime.size(); ++i) {
		string temp = to_string(indDateTime[perm[i]]);
		Rprintf("%s ", temp.c_str());
		for(int j=0; j < (int)m[0].size(); ++j)
		    Rprintf("%lf ", m[perm[i]][j]);
		Rprintf("\n");
	    }
	}
	else {
	    vector<double> v = zoo.getDataVec();
	    for(int i=0; i < (int)indDateTime.size(); ++i) {
		string temp = to_string(indDateTime[perm[i]]);
		Rprintf("%s ", temp.c_str());
	    }
	    Rprintf("\n");
	    for(int j=0; j < (int)v.size(); ++j)
		Rprintf("%lf ", v[perm[j]]);
	    Rprintf("\n");
	}
	break;
    }
    }
}

static void demoRcppFunction(SEXP func, SEXP hypot,
			     RcppFrame& frame,
			     RcppZoo& zoots) {

    // This function illustrates how to call an R function from C++
    // where the function expects three arguments: a data frame,
    // a zoo time series, and a vector (the R function is defined on
    // the RcppExample help page). The function returns a vector (it
    // could return any object that we can model on the C++ side).

    // First define the input vector (could use vector<double> here)
    int n=5;
    double* x = new double[n];
    for(int i=0; i < n; ++i) x[i] = i+1;

    // Next create the C++ interface to the R function (three arguments).
    RcppFunction f(func, 3);

    // Add the arguments:
    f.setNextArg(getSEXP(frame));
    f.setNextArg(getSEXP(zoots));
    f.setNextArg(getSEXP(x, n));

    // Call the function, create C++ object from result, and print.
    SEXP result = f.callR();
    RcppVector<double> vec(result);
    Rprintf("RcppFunction: func returned: (");
    for(int i=0; i < vec.size(); ++i)
	Rprintf("%6.2lf ", vec(i));
    Rprintf(")\n");

    // Here is a simpler example. It illustrates that there should
    // be a good reason for using callbacks since there is a fair
    // amount of overhead compared with a straight call to a compiled
    // library. The introduction of vec2 could be optimized out
    // by using REAL(result)[0], but that would be cheating.
    RcppFunction g(hypot,2);
    g.setNextArg(getSEXP(3.0));
    g.setNextArg(getSEXP(4.0));
    result = g.callR();
    RcppVector<double> vec2(result);
    Rprintf("RcppFunction: hypot returned %lf\n", vec2(0));

    // NOTE: Current implementation resets all args to unset
    //       after each call to callR(), so they must be set
    //       again before the next call.
}

#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

/*
 * Sample function illustrates how to use the Rcpp R/C++ interface library.
 */
RcppExport SEXP Rcpp_Example(SEXP params, SEXP nlist, 
			     SEXP numvec, SEXP nummat,
			     SEXP df, SEXP datevec, SEXP datetimevec,
			     SEXP stringvec,
			     SEXP func, SEXP hypot,
			     SEXP zoots, SEXP myfactor) {

    SEXP  rl=R_NilValue; // Use this when there is nothing to be returned.
    char* exceptionMesg=NULL;

    try {

	Rprintf("\nRcpp Version: %s\n\n", getRcppVersionCStr());

	// The next few system-dependent lines are not essential and
	// can be safely omitted.
#ifdef _MS_WINDOWS_

#ifdef __MINGW32__
	Rprintf("DLL built on Windows using MinGW\n");
#endif

#ifdef _USRDLL
	Rprintf("DLL built on Windows using MSVC\n");
#endif
	char *os;
       	if((os = getenv("OS")) != 0) { // For Windows (no uname)
	    Rprintf("OS: %s\n", os);
	}
#else // non-Windows case

#ifdef HAVE_SYS_UTSNAME_H
	struct utsname buf, *unamep = &buf;
	if(uname(unamep) == 0) {
	    Rprintf("Shared lib built on %s system\n", unamep->sysname);
	    Rprintf("Node:    %s\n", unamep->nodename);
	    Rprintf("Machine: %s\n", unamep->machine);
	    Rprintf("OS Rel:  %s\n", unamep->release);
	    Rprintf("OS Vers: %s\n", unamep->version);
	}
	else {
	    Rprintf("Shared lib built on UNKNOWN system type\n");
	}
#else
	Rprintf("Shared lib built on non-Windows, non-UNIX system\n");
#endif

#endif

	// Get input parameters from params SEXP.
	RcppParams rparam(params);
	RcppNumList nl(nlist); // DEPRECATED: use RcppParams instead.
	string method = rparam.getStringValue("method");
	double tolerance = rparam.getDoubleValue("tolerance");
	int    maxIter = rparam.getIntValue("maxIter");

	// RcppDate handles both R date types, Date and POSIXt.
	RcppDateTime currentTime = rparam.getDateTimeValue("currentTime");
	RcppDate startDate = rparam.getDateValue("startDate");
	RcppDate endDate = rparam.getDateValue("endDate");
	int weekday = rparam.getIntValue("weekday");
	int nthOccurrence = rparam.getIntValue("nthOccurrence");


	// To actually do something with the input data we compute
	// monthly dates between startDate and endDate that fall 
	// on the nth occurrence of the specified weekday in each month
	// (function is defined above). The weekday is mapped as
	// Sun=0, Mon=1, etc., and there are symbolic constants on the
	// C++ side RcppDate::Sun, RcppDate::Mon, etc.
	list<RcppDate> dateList = getSchedule(startDate, endDate,
					      nthOccurrence, weekday);

	// Construct C++ objects that correspond to input R objects.
	RcppDateVector dateVec(datevec);
	RcppDateTimeVector datetimeVec(datetimevec);
	RcppStringVector stringVec(stringvec);

	RcppFrame inframe(df);
	// Printing a RcppFrame:
	// Use inframe.print(), or more generally
	// vector<RcppColumn> cols = inframe.getColumns();
	// double x = cols[i].getDoubleValue(j); // i/j = row/col
	// RcppDate d = cols[i].getDateValue(j);
	// string level = cols[i].getFactorValue(j);
	// ...

	RcppVector<int> vecI(numvec);
	RcppMatrix<int> matI(nummat);
	RcppVector<double> vecD(numvec);
	RcppMatrix<double> matD(nummat);

	// The default value is NULL so we have to check.
	if(zoots != R_NilValue) {
	    RcppZoo zoo(zoots);
	    Rprintf("\nInput zoo time series:\n");
	    printZooTS(zoo);
	    Rprintf("\n");
	}

	// Since we use this later and it may be NULL, use pointer.
	RcppFactor* factor=0;
	if(myfactor != R_NilValue)
	    factor = new RcppFactor(myfactor);

	// Date/time conventions used by RcppDate and RcppDateTime.
	//
	// 1. Specify dates as month, day, year.
	// 2. Can include fraction of day param for RcppDateTime.
	// 3. Subtracting two dates yields the number of DAYS between
	//    them for both RcppDate and RcppDateTime. To get the number
	//    of seconds (including fractional seconds) in the latter 
	//    case multiply by RcppDate::DAYS2SECS (i.e., 60*60*24).
	// 4. Increments to dates are in units of DAYS. To add n seconds
	//    to an RcppDateTime, add n/DAYS2SECS days (casting n to
	//    double if necessary).
	// 5. To inspect the underlying M/D/Y or other info about an
	//    RcppDate or RcppDateTime use the RcppLocalTime class.
	// 6. Both date types can be output using << in the usual way.
	// 7. It is possible to subtract an RcppDate from an RcppDateTime
	//    object, but the answer would need to be interpreted w.r.t.
	//    the user's timezone, and this kind of calculation should
	//    be avoided. (An RcppDate initialized to M/D/Y is equivalent
	//    to an RcppDateTime initialized to M/D/Y with zero fractional
	//    day part, corresponding to time 00:00, or 12AM, provided 
	//    you are in the GMT timezone. For example, if your timezone 
	//    requires adjustment to an earlier time than GMT then the 
	//    RcppDateTime just described would display as the prior day 
	//    relative to the RcppDate object. Note that for timezones 
	//    to display properly the TZ environment variable needs to 
	//    be set. Use the Sys.timezone() R command to check.)

	// Examples.
	RcppDate d1(10,1,2009);   // Never has fractional part of day.
	RcppDate d2 = d1 + 5;     // Five days later (no fractions).
	RcppDateTime dt1(10,1,2009); // Same as d1, with time 12AM.

	RcppDateTime dt2 = dt1 + 5.25; // 5.25 days later.
	double datediff1 = dt2 - dt1; // equals 5.25 (days)
	double datediff2 = dt2 - d1;  // also 5.25, but here two different
	// date types are involved. This particular result is not affected
	// by the timezone because RcppDate and RcppDateTime (a subclass of
	// RcppDate) are defined consistently.

	RcppDateTime dt3(10,1,2009,.75); // Can have fraction of day.
	// The latter time is 10/1/2009 at 18:00 GMT, where
	// the time follows from 18/24 = .75 (fraction of day).

	// Inspect an RcppDateTime (similarly for RcppDate).
	RcppDateTime dt4 = dt1 + 5.3/RcppDate::DAYS2SECS; // add 5.3 seconds.
	RcppLocalTime lt(dt4); // create inspector object.
	int month = lt.getMonth(), day = lt.getDay(), year = lt.getYear();
	int hour = lt.getHour(), min = lt.getMinute(), sec = lt.getSecond();
	double fracsec = lt.getFracSec(); // from Jan 1, 1970 base (0.3 here)

	// It is possible to get the numerical representation for a date
	// that is used by R, buy Excel on the PC, and by Excel on the Mac.
	// Placing this double value into an Excel spreadsheet (including
	// the fractional part) and formatting it as a date will show
	// the date/time as expected. Similarly, if x is the numeric value
	// in R, it can be viewed as a Date using
	// as.Date(x, origin=as.Date('1970-01-01')) or
	// as.POSIXct(x, origin=as.POSIXct(as.Date('1970-01-01'))), depending
	// on what type of object the number was computed for.
	double valExcelPC  = dt4.numExcelPC();
	double valExcelMac = dt4.numExcelMac();
	double valR = dt4.getRValue();
	
	// Do some computations with the matrices.
	int nrows = matD.getDim1();
	int ncols = matD.getDim2();
	for(int i = 0; i < nrows; i++)
	    for(int j = 0; j < ncols; j++)
		matD(i,j) = 2 * matD(i,j);

	int len = vecD.size();
	for(int i = 0; i < len; i++)
	    vecD(i) = 3 * vecD(i);

	// Get copy of matrix/vector in standard (unchecked) C/C++ format.
	// May be useful when these vectors need to be passed to
	// C/C++ code that does not know about Rcpp classes...
	double **a = matD.cMatrix();
	double  *v = vecD.cVector();

	// ...or we might want to use an STL container...
	vector<double> stlvec(vecD.stlVector());
	nrows = (int)stlvec.size();
	for(int i = 0; i < nrows; i++)
	    stlvec[i] += 1;

	// ...or perhaps a container of containers.
	vector<vector<double> > stlmat(matD.stlMatrix());
	nrows = (int)stlmat.size();
	ncols = (int)stlmat[0].size();
	for(int i = 0; i < nrows; i++)
	    for(int j = 0; j < ncols; j++)
		stlmat[i][j] += 2;

	// Get a zero matrix the same size as matD.
	RcppMatrix<double> matZ(nrows, ncols);

	// Prepare to build a return list.
	RcppResultSet rs;

	// Construct a RcppFrame object from the C++ side that will
	// be seen as a data frame when returned to R. See the
	// getDataFrame() function above for more info.
	RcppFrame frame = getDataFrame();
	// Rprintf("Printing frame from getDataFrame()...\n");
	// frame.print();

	// Construct a few RcppZoo objects that will be seen as zoo
	// time series objects when returned to R. Of course, for this
	// to work properly the zoo package should be installed (otherwise
	// the returned zoo structures will look like lists). Since
	// tseries objects can be converted into zoo objects using
	// as.zoo() on the R side, tseries objects can be handled by
	// first converting them to zoo objects (this conversion is
	// reversible). The zoo objects returned are assigned names
	// zoo1, zoo2, ..., zoo8. See getZooObjects() above for more info.
	vector<RcppZoo> zoovec = getZooObjects();
	for(int i=0; i < (int)zoovec.size(); ++i) {
	    rs.add("zoo"+to_string(i+1), zoovec[i]);
	    // Rprintf("Zoo %d:", i);
	    // printZooTS(zoovec[i]); // use this to dump contents.
	}

	// This function shows how to use RcppFunction:
	demoRcppFunction(func, hypot, frame, zoovec[0]);

	// Calls QuantLib Equity Option Example.
	// See inst/doc/QuantLib/README.txt for more information.
	//SEXP EquityOption();
	//EquityOption();

	// Return selected C++ objects.
	rs.add("dateVec", dateVec);
	rs.add("datetimeVec", datetimeVec);
	rs.add("matD", matD);
	rs.add("stlvec", stlvec);
	rs.add("stlmat", stlmat);
	rs.add("a", a, nrows, ncols);
	rs.add("v", v, len);
	rs.add("frame", frame);
	rs.add("inframe", inframe);
	if(factor != 0)
	    rs.add("factor", *factor);

	// This is a generic/template function call, so dateList could
	// be vector<RcppDate>, list<RcppDate>, etc. The only requirements
	// are that the elements of the collection must be of type RcppDate
	// or RcppDateTime, and the elements must provide the function
	// getRValue() that returns the double value used by R to represent
	// that element's value.
	rs.add("dateList", dateList.begin(), dateList.end());

	// It is often useful to return the input parameter list like this.
	rs.add("params", params, false);

	
	// Use all variables so compiler does not complain (Windows):
	tolerance = tolerance;	maxIter = maxIter; 
	datediff1 = datediff1; datediff2 = datediff2;
	month = month; day = day; year = year;	
	hour = hour; min = min;	sec = sec; fracsec = fracsec;
	valExcelPC = valExcelPC; valExcelMac = valExcelMac; valR = valR;

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


// Rcpp.cpp: Part of the R/C++ Object Mapping Library
//
// Copyright (C) 2005-2009 Dominick Samperi
//
// As an extension of R this software is naturally released under the 
// same terms, namely, the GNU General Public License Version 2.
// For more information go to http://www.gnu.org/licenses/gpl.html.

#include <algorithm>

#include "Rcpp.hpp"

using namespace std;

#ifdef _WINDOWS
#include <time.h>
#endif

namespace Rcpp {

    static const char *RcppVersion = "6.1";

    const char* getRcppVersionCStr() { return RcppVersion; }

    RcppExport SEXP  getRcppVersion() { // Can be called from R
	SEXP value = PROTECT(allocVector(STRSXP, 1));
	SET_STRING_ELT(value, 0, mkChar(RcppVersion));
	UNPROTECT(1);
	return value;
    }

RcppParams::RcppParams(SEXP params) {

    // list(name=value,etc.) does not have a class attribute? Also,
    // isList(params) is false!? The best soln we could come up with is:
    if(!isNewList(params))
	throw std::range_error("Invalid SEXP in RcppParams constructor");

    int len = length(params);
    SEXP names = getAttrib(params, R_NamesSymbol);
    if(names == R_NilValue)
	throw std::range_error("RcppParams: list must have named elements");
    for(int i = 0; i < len; i++) {
	string nm = string(CHAR(STRING_ELT(names,i)));
	if(nm.size() == 0)
	    throw std::range_error("RcppParams: all list elements must be named");
	pmap[nm] = i;
    }
    _params = params;
}

void RcppParams::checkNames(char *inputNames[], int len) {
    for(int i = 0; i < len; i++) {
	map<string,int>::iterator iter = pmap.find(inputNames[i]);
	if(iter == pmap.end()) {
	    string mesg = "checkNames: missing required parameter ";
	    throw range_error(mesg+inputNames[i]);
	}
    }
}

RcppNumList::RcppNumList(SEXP theList) {
    if(!isNewList(theList))
	throw std::range_error("RcppNumList: non-list passed to constructor");
    len = length(theList);
    names = getAttrib(theList, R_NamesSymbol);
    namedList = theList;
}
string RcppNumList::getName(int i) {
    if(i < 0 || i >= len) {
	throw std::range_error("RcppNumList::getName: index out of bounds: "+to_string(i));
    }
    return string(CHAR(STRING_ELT(names,i)));
}
double RcppNumList::getValue(int i) {
    if(i < 0 || i >= len) {
	throw std::range_error("RcppNumList::getValue: index out of bounds: "+to_string(i));
    }
    SEXP elt = VECTOR_ELT(namedList, i);
    if(isReal(elt))
	return REAL(elt)[0];
    else if(isInteger(elt))
	return (double)INTEGER(elt)[0];
    else
	throw std::range_error("RcppNumList: contains non-numeric value");
    return 0; // never get here
}

RcppFactor::RcppFactor(vector<string> names) {

    // Copy unique sorted list to levelNames.
    levelNames = names;
    vector<string>::iterator iter = levelNames.end();
    sort(levelNames.begin(), levelNames.end());
    iter = unique(levelNames.begin(), levelNames.end());
    levelNames.resize(iter - levelNames.begin());

    // Now get position of each name in levelNames.
    observations.resize(names.size());
    for(int i = 0; i < (int)names.size(); ++i) {
	iter = find(levelNames.begin(), levelNames.end(), names[i]);
	observations[i] = iter - levelNames.begin();
    }
}

void RcppFactor::print() {
    Rprintf("Factor levels:\n");
    for(int i=0; i < (int)levelNames.size(); ++i)
	Rprintf("  %s(%d)\n", levelNames[i].c_str(), i+1);
    Rprintf("Factor Observations:\n");
    for(int i=0; i < (int)observations.size(); ++i) {
	Rprintf("  %d  %s(%d)\n", i, getObservedLevelStr(i).c_str(),
		getObservedLevelNum(i));
    }
}

void RcppColumn::print() {
    switch(type) {
    case COLTYPE_INT:
	Rprintf("INT:\n");
	for(int i = 0; i < (int)colInt->size(); ++i)
	    Rprintf("  %d\n", (*colInt)[i]);
	break;
    case COLTYPE_DOUBLE:
	Rprintf("DOUBLE:\n");
	for(int i = 0; i < (int)colDouble->size(); ++i)
	    Rprintf("  %lf\n", (*colDouble)[i]);
	break;
    case COLTYPE_STRING:
	Rprintf("STRING:\n");
	for(int i = 0; i < (int)colString->size(); ++i)
	    Rprintf("  %s\n", (*colString)[i].c_str());
	break;
    case COLTYPE_FACTOR:
	Rprintf("FACTOR:\n");
	for(int i = 0; i < (int)colFactor->getNumObservations(); ++i)
	    Rprintf("  %s(%d)\n",
		    colFactor->getObservedLevelStr(i).c_str(),
		    colFactor->getObservedLevelNum(i));
	break;
    case COLTYPE_LOGICAL:
	Rprintf("LOGICAL:\n");
	for(int i = 0; i < (int)colBool->size(); ++i)
	    Rprintf("  %s\n", ((*colBool)[i]?"T":"F"));
	break;
    case COLTYPE_DATE:
	Rprintf("DATE:\n");
	for(int i = 0; i < (int)colDate->size(); ++i)
	    Rprintf("  %s\n", to_string((*colDate)[i]).c_str());
	break;
    case COLTYPE_DATETIME:
	Rprintf("DATETIME:\n");
	for(int i = 0; i < (int)colDateTime->size(); ++i)
	    Rprintf("  %s\n", to_string((*colDateTime)[i]).c_str());
	break;
    }
}

RcppLocalTime::RcppLocalTime(const RcppDate& date) {
    isPOSIXDate = false;
    int jdn = date.numJulian();
    RcppMDY mdy = RcppDate::jdn2mdy(jdn);
    month = mdy.month;
    day = mdy.day;
    year = mdy.year;
    hour = minute = second = 0;
    fracsec = 0;
    weekday = date.getWeekday();
}

RcppLocalTime::RcppLocalTime(const RcppDateTime& date) {
    isPOSIXDate = true;

    // Need to set TZ environment variable, TZ=EDT, e.g.
    time_t tsecs = (time_t)date.getrdtnum(); // drop fraction.
    fracsec = date.getrdtnum() - tsecs;
 
#ifdef _WINDOWS
    if(localtime_s(&tmdata, &tsecs) != 0)
	throw range_error("system call to localtime failed");
#else
    tmdata = *localtime(&tsecs);
#endif
    month = tmdata.tm_mon+1;
    day = tmdata.tm_mday;
    year = tmdata.tm_year+1900;
    hour = tmdata.tm_hour;
    minute = tmdata.tm_min;
    second = tmdata.tm_sec;
    weekday = tmdata.tm_wday;
}

string RcppLocalTime::getWeekdayName() const {
    if(weekday < 0 || weekday > 6)
	throw range_error(string("getWeekdayName: invalid weekday: ")
			  +to_string(weekday));
    return RcppDate::weekdayName[weekday];
}
string RcppLocalTime::getMonthName() const {
    if(month < 1 || month > 12)
	throw range_error(string("getMonthName: invalid month")
			  +to_string(month));
    return RcppDate::monthName[month-1];
}
struct tm RcppLocalTime::getTMData() const { 
    if(!isPOSIXDate)
	throw std::range_error("ERROR: attempt to fetch tm data for non-POSIX date");
    return tmdata; 
}

/** Assumption: the rows of the data part are sorted based on the index vector.
 *  The zoo package maintains this constraint at the R level. It also
 *  ensures that the index vector is consistent with the specified
 *  frequency; if it is not, the frequency attribute is dropped.
 *  When we construct an RcppZoo object on the C++ side we have 
 *  to take responsibility for maintaining these constraints. 
 *  See the other RcppZoo constructors.
 */
RcppZoo::RcppZoo(SEXP zoosexp) {

    int numProtected = 0; // not to be confused with the one in RcppResultSet

    // First check class names.
    SEXP classlist = getAttrib(zoosexp, R_ClassSymbol);
    if(classlist == R_NilValue)
	throw range_error("Could not get class names for zoo object");
    string classname1, classname2 = "";
    classname1 = CHAR(STRING_ELT(classlist,0));
    if(length(classlist) > 1)
	classname2 = CHAR(STRING_ELT(classlist,1));
    bool classNamesOK = ((length(classlist) > 1 &&
			  classname1 == "zooreg" &&
			  classname2 == "zoo") ||
			 (classname1 == "zoo"));
    isRegular_ = (classname1 == "zooreg");
    if(!classNamesOK)
	throw range_error("Invalid class names in RcppZoo constructor");

    // Check that data type is numeric.
    if(!isNumeric(zoosexp))
	throw range_error("Non-numeric data type in RcppZoo constructor");

    // Get Data part.
    bool intData = (bool)isInteger(zoosexp); // coerce to double if true
    SEXP dimsAttr = getAttrib(zoosexp, R_DimSymbol);
    if(dimsAttr != R_NilValue && length(dimsAttr) == 2) { // matrix data
	isMatrix_ = true;
	int dim1 = INTEGER(dimsAttr)[0];
	int dim2 = INTEGER(dimsAttr)[1];
	dataMat.resize(dim1);
	for(int i=0; i < dim1; ++i) {
	    vector<double> col(dim2);
	    if(intData) {
		int *ip = INTEGER(zoosexp);
		for(int j=0; j < dim2; ++j)
		    col[j] = ip[i + dim1*j];
	    }
	    else {
		double *rp = REAL(zoosexp);
		for(int j=0; j < dim2; ++j)
		    col[j] = rp[i + dim1*j];
	    }
	    dataMat[i] = col;
	}
    }
    else { // vector data
	isMatrix_ = false;
	int dim1 = length(zoosexp);
	dataVec.resize(dim1);
	if(intData) {
	    int *ip = INTEGER(zoosexp);
	    for(int i=0; i < dim1; ++i)
		dataVec[i] = ip[i];
	}
	else {
	    double *rp = REAL(zoosexp);
	    for(int i=0; i < dim1; ++i) {
		dataVec[i] = rp[i];
	    }
	}
    }	

    // Get frequency if regular.
    if(isRegular_) {
	SEXP attrBufFreq = PROTECT(allocVector(STRSXP,1));
	numProtected++;
	SET_STRING_ELT(attrBufFreq, 0, mkChar("frequency"));
	SEXP freqAttr = getAttrib(zoosexp, attrBufFreq);
	frequency_ = (int)REAL(freqAttr)[0];
    }
    
    // Get index attribute.
    SEXP attrBufIndex = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(attrBufIndex, 0, mkChar("index"));
    SEXP indexAttr = getAttrib(zoosexp, attrBufIndex);
    if(indexAttr == R_NilValue)
	throw range_error("No index attribute in RcppZoo");

    // Get index class name if available (Date, POSIXt)
    SEXP indexClass = getAttrib(indexAttr, R_ClassSymbol);
    string indexClassName("None"); // no class for integer/double.
    if(indexClass != R_NilValue)
	indexClassName = CHAR(STRING_ELT(indexClass,0));

    // The zoo object from R should already be sorted so we
    // define the identity sort permutation.
    int lenIndex = length(indexAttr);
    sortPerm.resize(lenIndex);
    for(int i=0; i < lenIndex; ++i)
	sortPerm[i] = i;

    // Get index vector.
    if(isInteger(indexAttr)) {
	indInt.resize(lenIndex);
	int *ip = INTEGER(indexAttr);
	for(int i = 0; i < lenIndex; ++i) {
	    indInt[i] = ip[i];
	}
	indexType_ = IND_INTEGER;
    }
    else if(indexClassName == "Date") {
	indDate.resize(lenIndex);
	double *rp = REAL(indexAttr);
	for(int i = 0; i < lenIndex; ++i) {
	    indDate[i] = RcppDate(rp[i]);
	}
	indexType_ = IND_DATE;
    }
    else if(indexClassName == "POSIXt") {
	indDateTime.resize(lenIndex);
	double *rp = REAL(indexAttr);
	for(int i = 0; i < lenIndex; ++i) {
	    indDateTime[i] = RcppDateTime(rp[i]);
	}
	indexType_ = IND_DATETIME;
    }
    else if(isNumeric(indexAttr)) {
	indDouble.resize(lenIndex);
	double *rp = REAL(indexAttr);
	for(int i = 0; i < lenIndex; ++i) {
	    indDouble[i] = rp[i];
	}
	indexType_ = IND_DOUBLE;
    }
    else // Could easily support string and factor indices, but why?
	throw range_error("Zoo index type not supported");

    UNPROTECT(numProtected);
}

// ZooValidator is used by the RcppZoo constructors that are intended to
// be used from the C++ side. Checks that the specfied (positive) 
// frequency is consistent with the index vector. Also computes the 
// perturbation vector that can be used to sort the index vector 
// and the rows of the data matrix on output to R. We work with 
// references to the original data and do not modify this data 
// to limit the amount of copying (and to prevent unintended side-effects).
template <typename TIndex, typename TData>
class ZooValidator {
    vector<TIndex>& index;
    vector<TData>& data;
public:
    // Comparator is used to generate the perturbation necessary to
    // sort the index vector and the data rows.
    template <typename T>
    class Comparator {
	vector<T>& vi_;
    public:
	Comparator(vector<T>& v) : vi_(v) {}
	bool operator()(int i, int j) {
	    return vi_[i] < vi_[j];
	}
    };
    ZooValidator(vector<TIndex>& index_, vector<TData>& data_)
	: index(index_), data(data_) {}
    bool frequencyIsValid(int freq) {
	for(int i=1; i < (int)index.size(); ++i) {
	    double dist = 1.0*(index[i] - index[i-1])/freq;
	    double frac = dist - (int)dist;
	    if(frac > 1.0e-8)
		return false;
	}
	return true;
    }
    vector<int> getSortPermutation() {
	vector<int> seq(index.size());
	for(int i=0; i < (int)index.size(); ++i)
	    seq[i] = i;
	sort(seq.begin(), seq.end(), Comparator<TIndex>(index));
	return seq;
    }
};

// Constructors for use on the C++ side.
RcppZoo::RcppZoo(vector<double>& data, vector<int>& index, int freq) 
    : indInt(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_INTEGER;
    ZooValidator<int,double> v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<double>& data, vector<double>& index, int freq)
    : indDouble(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_DOUBLE; 
    ZooValidator<double,double> v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<double>& data, vector<RcppDate>& index, int freq)
    : indDate(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_DATE; 
    isRegular_ = false;
    ZooValidator<RcppDate,double> v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<double>& data, vector<RcppDateTime>& index, int freq)
    : indDateTime(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_DATETIME; 
    ZooValidator<RcppDateTime,double> v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<vector<double> >& data, vector<int>& index, int freq)
    : indInt(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_INTEGER; 
    ZooValidator<int,vector<double> > v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<vector<double> >& data, vector<double>& index, int freq)
    : indDouble(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_DOUBLE; 
    ZooValidator<double,vector<double> > v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<vector<double> >& data, vector<RcppDate>& index,int freq)
    : indDate(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_DATE; 
    ZooValidator<RcppDate,vector<double> > v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}
RcppZoo::RcppZoo(vector<vector<double> >& data, vector<RcppDateTime>& index,int freq)
    : indDateTime(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_DATETIME; 
    ZooValidator<RcppDateTime,vector<double> > v(index, data);
    isRegular_ = false;
    if(frequency_ > 0 && v.frequencyIsValid(frequency_))
	isRegular_ = true;
    else if(frequency_ > 0)
	Rprintf("WARNING: Ignoring invalid frequency\n");
    sortPerm = v.getSortPermutation();
}

RcppFactor::RcppFactor(SEXP fac) {
    SEXP classname = getAttrib(fac, R_ClassSymbol);
    if(!isFactor(fac) || classname == R_NilValue ||
       strcmp(CHAR(STRING_ELT(classname,0)),"factor") != 0)
	throw std::range_error("Invalid SEXP in RcppFactor constructor");
    int nObs = length(fac);
    SEXP names = getAttrib(fac, R_LevelsSymbol);
    int numLevels = length(names);
    levelNames.resize(numLevels);
    observations.resize(nObs);
    for(int k=0; k < numLevels; k++)
	levelNames[k] = string(CHAR(STRING_ELT(names,k)));
    int *ip = INTEGER(fac);
    for(int j=0; j < nObs; ++j)
	observations[j] = ip[j]-1;
}

RcppColumn::RcppColumn(const RcppColumn& col) { // copy constructor
	needsCleanup = col.needsCleanup;
	switch(col.type) {
		case RcppColumn::COLTYPE_INT:
			type = COLTYPE_INT;
			if(needsCleanup) // they all own private copies.
				colInt = new std::vector<int>(*col.colInt);
			else
				colInt = col.colInt; // point to same area managed by client.
			break;
		case RcppColumn::COLTYPE_DOUBLE:
			type = COLTYPE_DOUBLE;
			if(needsCleanup)
				colDouble = new std::vector<double>(*col.colDouble);
			else
				colDouble = col.colDouble;
			break;
		case RcppColumn::COLTYPE_STRING:
			type = COLTYPE_STRING;
			if(needsCleanup)
				colString = new std::vector<std::string>(*col.colString);
			else
				colString = col.colString;
			break;
		case RcppColumn::COLTYPE_LOGICAL:
			type = COLTYPE_LOGICAL;
			if(needsCleanup)
				colBool = new std::vector<bool>(*col.colBool);
			else
				colBool = col.colBool;
			break;
		case RcppColumn::COLTYPE_FACTOR:
			type = COLTYPE_FACTOR;
			if(needsCleanup)
				colFactor = new RcppFactor(*col.colFactor);
			else
				colFactor = col.colFactor;
			break;
		case RcppColumn::COLTYPE_DATE:
			type = COLTYPE_DATE;
			if(needsCleanup)
				colDate = new std::vector<RcppDate>(*col.colDate);
			else
				colDate = col.colDate;
			break;
		case RcppColumn::COLTYPE_DATETIME:
			type = COLTYPE_DATETIME;
			if(needsCleanup)
				colDateTime = new std::vector<RcppDateTime>(*col.colDateTime);
			else
				colDateTime = col.colDateTime;
			break;
		default:
			;
	}
}

RcppColumn::~RcppColumn() {
	if(needsCleanup) {
		switch(type) {
		case RcppColumn::COLTYPE_INT:
			delete colInt;
			break;
		case RcppColumn::COLTYPE_DOUBLE:
			delete colDouble;
			break;
		case RcppColumn::COLTYPE_STRING:
			delete colString;
			break;
		case RcppColumn::COLTYPE_LOGICAL:
			delete colBool;
			break;
		case RcppColumn::COLTYPE_FACTOR:
			delete colFactor;
			break;
		case RcppColumn::COLTYPE_DATE:
			delete colDate;
			break;
		case RcppColumn::COLTYPE_DATETIME:
			delete colDateTime;
			break;
		default:
			;
		}
	}
}

RcppFrame::RcppFrame(SEXP df) {

    int numProtected = 0;
    
    SEXP classname = getAttrib(df, R_ClassSymbol);
    if(!isFrame(df) || classname == R_NilValue ||
       strcmp(CHAR(STRING_ELT(classname,0)),"data.frame") != 0)
	throw std::range_error("Invalid SEXP in RcppFrame constructor");

    int ncol = length(df);
    SEXP names = getAttrib(df, R_NamesSymbol);
    colNames.resize(ncol);
    SEXP colData = VECTOR_ELT(df,0); // First column of data.
    int nrow = length(colData);
    if(nrow == 0)
	throw std::range_error("RcppFrame::RcppFrame: zero lenth column.");

    for(int i=0; i < ncol; i++) {
	colNames[i] = string(CHAR(STRING_ELT(names,i)));
	SEXP colData = VECTOR_ELT(df,i);
	if(!isVector(colData) || length(colData) != nrow)
	    throw std::range_error("RcppFrame::RcppFrame: invalid column.");

	bool isDateClass = false, isPOSIXDate = false;
	SEXP classname = getAttrib(colData, R_ClassSymbol);
	if(classname != R_NilValue) {
	    isDateClass = (strcmp(CHAR(STRING_ELT(classname,0)),"Date") == 0);
	    isPOSIXDate = (strcmp(CHAR(STRING_ELT(classname,0)),"POSIXt") == 0);
	}

	if(isReal(colData)) {
	    if(isDateClass) {
		vector<RcppDate>* colDate = new vector<RcppDate>(nrow);
		double *rp = REAL(colData);
		for(int j=0; j < nrow; j++) // Column of Date's
		    (*colDate)[j] = RcppDate((int)rp[j]);
		cols.push_back(RcppColumn(*colDate));
	    }
	    else if(isPOSIXDate) {
		vector<RcppDateTime>* colDateTime = new vector<RcppDateTime>(nrow);
		double *rp = REAL(colData);
		for(int j=0; j < nrow; j++) // Column of Date's
		    (*colDateTime)[j] = RcppDateTime(rp[j]);
		cols.push_back(RcppColumn(*colDateTime));
	    }
	    else { // Column of REAL's
		vector<double>* colDouble = new vector<double>(nrow);
		double *rp = REAL(colData);
		for(int j=0; j < nrow; j++)
		    (*colDouble)[j] = rp[j];
		cols.push_back(RcppColumn(*colDouble));
	    }
	}
	else if(isInteger(colData)) {
	    int *ip = INTEGER(colData);
	    vector<int>* colInt = new vector<int>(nrow);
	    for(int j=0; j < nrow; j++)
		(*colInt)[j] = ip[j];
	    cols.push_back(RcppColumn(*colInt));
	}
	else if(isString(colData)) { // Non-factor string column
	    vector<string>* colString = new vector<string>(nrow);
	    for(int j=0; j < nrow; j++)
		(*colString)[j] = CHAR(STRING_ELT(colData,j));
	    cols.push_back(RcppColumn(*colString));
	}
	else if (isFactor(colData)) { // Factor column.
	    SEXP names = getAttrib(colData, R_LevelsSymbol);
	    int numLevels = length(names);
	    vector<string> levelNames(numLevels);
	    vector<string> obsLevelNames(nrow);
	    for(int k=0; k < numLevels; k++)
		levelNames[k] = string(CHAR(STRING_ELT(names,k)));
	    int *ip = INTEGER(colData);
	    for(int j=0; j < nrow; ++j) {
		int obsLevel = ip[j];
		obsLevelNames[j] = levelNames[obsLevel-1];
	    }
	    RcppFactor* colFactor = new RcppFactor(obsLevelNames);
	    cols.push_back(RcppColumn(*colFactor));
	}
	else if(isLogical(colData)) {
	    vector<bool>* colBool = new vector<bool>(nrow);
	    int *ip = INTEGER(colData);
	    for(int j=0; j < nrow; j++) {
		(*colBool)[j] = (bool)ip[j];
	    }
	    cols.push_back(RcppColumn(*colBool));
	}
	else
	    throw std::range_error("RcppFrame::RcppFrame: unsupported data frame column type.");
    }

    // Get row.names attribute and data
    SEXP rownamesAttrName = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(rownamesAttrName, 0, mkChar("row.names"));
    SEXP rownamesAttr = getAttrib(df, rownamesAttrName);
    if(rownamesAttr == R_NilValue)
	throw range_error("No row.names attribute in RcppFrame");

    // Get row.names class if available. (Don't need this.)
    SEXP rownamesClass = getAttrib(rownamesAttr, R_ClassSymbol);
    string rownamesClassName("None"); // no class for integer/double.
    if(rownamesClass != R_NilValue)
	rownamesClassName = CHAR(STRING_ELT(rownamesClass,0));

    // Get row.names vector. R sometimes uses numeric names but we
    // convert to strings when this happens.
    rowNames.resize(length(rownamesAttr));
    if(isInteger(rownamesAttr)) {
	int *ip = INTEGER(rownamesAttr);
	for(int i = 0; i < length(rownamesAttr); ++i)
	    rowNames[i] = to_string(ip[i]);
    }
    else if(isNumeric(rownamesAttr)) {
	double *rp = REAL(rownamesAttr);
	for(int i = 0; i < length(rownamesAttr); ++i)
	    rowNames[i] = to_string(rp[i]);
    }
    else if(isString(rownamesAttr)) {
	for(int i = 0; i < length(rownamesAttr); ++i) {
	    rowNames[i] = CHAR(STRING_ELT(rownamesAttr,i));
	}
    }

    UNPROTECT(numProtected);
}

double RcppParams::getDoubleValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getDoubleValue: no such name: ";
		throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    if(!isNumeric(elt) || length(elt) != 1) {
	string mesg = "getDoubleValue: must be scalar ";
	throw std::range_error(mesg+name);
    }
    if(isInteger(elt))
	return (double)INTEGER(elt)[0];
    else if(isReal(elt))
	return REAL(elt)[0];
    else {
	string mesg = "getDoubleValue: invalid value for ";
	throw std::range_error(mesg+name);
    }
    return 0; // never get here
}

int RcppParams::getIntValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getIntValue: no such name: ";
	throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    if(!isNumeric(elt) || length(elt) != 1) {
	string mesg = "getIntValue: must be scalar: ";
	throw std::range_error(mesg+name);
    }
    if(isInteger(elt))
	return INTEGER(elt)[0];
    else if(isReal(elt))
	return (int)REAL(elt)[0];
    else {
	string mesg = "getIntValue: invalid value for: ";
	throw std::range_error(mesg+name);
    }
    return 0; // never get here
}

bool RcppParams::getBoolValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getBoolValue: no such name: ";
	throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    if(isLogical(elt))
	return (bool)(INTEGER(elt)[0]);
    else {
	string mesg = "getBoolValue: invalid value for: ";
	throw std::range_error(mesg+name);
    }
    return false; // never get here
}

string RcppParams::getStringValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getStringValue: no such name: ";
	throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    if(isString(elt))
		return string(CHAR(STRING_ELT(elt,0)));
    else {
	string mesg = "getStringValue: invalid value for: ";
	throw std::range_error(mesg+name);
    }
    return ""; // never get here
}

RcppDate RcppParams::getDateValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getDateValue: no such name: ";
	throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    return RcppDate(elt);
}

RcppDateTime RcppParams::getDateTimeValue(string name) {
    map<string,int>::iterator iter = pmap.find(name);
    if(iter == pmap.end()) {
	string mesg = "getDateValue: no such name: ";
	throw std::range_error(mesg+name);
    }
    int posn = iter->second;
    SEXP elt = VECTOR_ELT(_params,posn);
    return RcppDateTime(elt);
}

RcppDate::RcppDate(SEXP dateSEXP) {
    SEXP cls = getAttrib(dateSEXP, R_ClassSymbol);
    string clsname = string(CHAR(STRING_ELT(cls,0)));
    if(clsname != "Date")
	throw("Invalid R type in RcppDate constructor");
    rdtnum = REAL(dateSEXP)[0] * DAYS2SECS;
}

RcppDateTime::RcppDateTime(SEXP dateSEXP) {
    SEXP cls = getAttrib(dateSEXP, R_ClassSymbol);
    string clsname = string(CHAR(STRING_ELT(cls,0)));
    if(clsname != "POSIXt")
	throw("Invalid R type in RcppDateTime constructor");
    rdtnum = REAL(dateSEXP)[0];
}

RcppDateVector::RcppDateVector(SEXP vec) {
    int i;
    if(!isNumeric(vec) || isMatrix(vec) || isLogical(vec))
	throw std::range_error("RcppVector: invalid numeric vector in constructor");
    int len = length(vec);
    if(len == 0)
	throw std::range_error("RcppVector: null vector in constructor");
    SEXP cls = getAttrib(vec, R_ClassSymbol);
    string clsname = string(CHAR(STRING_ELT(cls,0)));
    if(clsname != "Date")
	throw std::range_error("RcppDateVector: invalid SEXP");

    v = new RcppDate[len];
    double *rp = REAL(vec);
    for(i = 0; i < len; i++)
	v[i] = RcppDate((int)rp[i]);
    length = len;
}

RcppDateTimeVector::RcppDateTimeVector(SEXP vec) {
    int i;
    if(!isNumeric(vec) || isMatrix(vec) || isLogical(vec))
	throw std::range_error("RcppVector: invalid numeric vector in constructor");
    int len = length(vec);
    if(len == 0)
	throw std::range_error("RcppVector: null vector in constructor");
    SEXP cls = getAttrib(vec, R_ClassSymbol);
    string clsname = string(CHAR(STRING_ELT(cls,0)));
    if(clsname != "POSIXt")
	throw std::range_error("RcppDateTimeVector: invalid SEXP");

    v = new RcppDateTime[len];
    double *rp = REAL(vec);
    for(i = 0; i < len; i++)
	v[i] = RcppDateTime(rp[i]);
    length = len;
}

RcppStringVector::RcppStringVector(SEXP vec) {
    int i;
    if(isMatrix(vec) || isLogical(vec))
	throw std::range_error("RcppVector: invalid numeric vector in constructor");
    if(!isString(vec))
	throw std::range_error("RcppStringVector: invalid string");
    int len = length(vec);
    if(len == 0)
	throw std::range_error("RcppVector: null vector in constructor");
    v = new string[len];
    for(i = 0; i < len; i++)
	v[i] = string(CHAR(STRING_ELT(vec,i)));
    length = len;
}

template <typename T>
RcppVector<T>::RcppVector(SEXP vec) {
    int i;

    // The function isVector returns TRUE for vectors AND
    // matrices, so it does not distinguish. We could
    // check the dim attribute here to be sure that it
    // is not present (i.e., dimAttr == R_NilValue, not 0!).
    // But it is easier to simply check if it is set via
    // isMatrix (in which case we don't have a vector).
    if(!isNumeric(vec) || isMatrix(vec) || isLogical(vec))
	throw std::range_error("RcppVector: invalid numeric vector in constructor");
    len = length(vec);
    v = (T *)R_alloc(len, sizeof(T));
    if(isInteger(vec)) {
	int *ip = INTEGER(vec);
	for(i = 0; i < len; i++)
	    v[i] = (T)(ip[i]);
    }	
    else if(isReal(vec)) {
	double *rp = REAL(vec);
	for(i = 0; i < len; i++)
	    v[i] = (T)(rp[i]);
    }
}

template <typename T>
RcppVector<T>::RcppVector(int _len) {
    len = _len;
    v = (T *)R_alloc(len, sizeof(T));
    for(int i = 0; i < len; i++)
	v[i] = 0;
}

template <typename T>
T *RcppVector<T>::cVector() {
    T* tmp = (T *)R_alloc(len, sizeof(T));
    for(int i = 0; i < len; i++)
	tmp[i] = v[i];
    return tmp;
}

template <typename T>
vector<T> RcppVector<T>::stlVector() {
    vector<T> tmp(len);
    for(int i = 0; i < len; i++)
	tmp[i] = v[i];
    return tmp;
}

template <typename T>
RcppMatrix<T>::RcppMatrix(SEXP mat) {

    if(!isNumeric(mat) || !isMatrix(mat))
	throw std::range_error("RcppMatrix: invalid numeric matrix in constructor");

    // Get matrix dimensions
    SEXP dimAttr = getAttrib(mat, R_DimSymbol);
    dim1 = INTEGER(dimAttr)[0];
    dim2 = INTEGER(dimAttr)[1];

    // We guard against  the possibility that R might pass an integer matrix.
    // Can be prevented using R code: temp <- as.double(a), dim(temp) <- dim(a)
    int i,j;
    int isInt = isInteger(mat);
    T *m = (T *)R_alloc(dim1*dim2, sizeof(T));
    a = (T **)R_alloc(dim1, sizeof(T *));
    for(i = 0; i < dim1; i++)
	a[i] = m + i*dim2;
    if(isInt) {
	int *ip = INTEGER(mat);
	for(i=0; i < dim1; i++)
	    for(j=0; j < dim2; j++)
		a[i][j] = (T)(ip[i+dim1*j]);
    }	
    else {
	double *rp = REAL(mat);
	for(i=0; i < dim1; i++)
	    for(j=0; j < dim2; j++)
		a[i][j] = (T)(rp[i+dim1*j]);
    }	
}

template <typename T>
RcppMatrix<T>::RcppMatrix(int _dim1, int _dim2) {
    dim1 = _dim1;
    dim2 = _dim2;
    int i,j;
    T *m = (T *)R_alloc(dim1*dim2, sizeof(T));
    a = (T **)R_alloc(dim1, sizeof(T *));
    for(i = 0; i < dim1; i++)
	a[i] = m + i*dim2;
    for(i=0; i < dim1; i++)
	for(j=0; j < dim2; j++)
	    a[i][j] = 0;
}

template <typename T>
vector<vector<T> > RcppMatrix<T>::stlMatrix() {
    int i,j;
    vector<vector<T> > temp;
    for(i = 0; i < dim1; i++) {
	temp.push_back(vector<T>(dim2));
    }
    for(i = 0; i < dim1; i++)
	for(j = 0; j < dim2; j++)
	    temp[i][j] = a[i][j];
    return temp;
}

template <typename T>
T **RcppMatrix<T>::cMatrix() {
    int i,j;
    T *m = (T *)R_alloc(dim1*dim2, sizeof(T));
    T **tmp = (T **)R_alloc(dim1, sizeof(T *));
    for(i = 0; i < dim1; i++)
	tmp[i] = m + i*dim2;
    for(i=0; i < dim1; i++)
	for(j=0; j < dim2; j++)
	    tmp[i][j] = a[i][j];
    return tmp;
}

SEXP RcppFunction::callR() {
    if(numArgsSet != nArgs)
	throw std::range_error("RcppFunction: call before all args set");
    SEXP expr = PROTECT(allocVector(LANGSXP,nArgs+1));
    numProtected++;
    SETCAR(expr, fn);
    SEXP t = expr;
    for(int i = 0; i < nArgs; ++i) {
	t = CDR(t);
	SETCAR(t, args[i]);
    }
    SEXP result = eval(expr, R_NilValue);
    UNPROTECT(numProtected);
    numProtected = 0;
    numArgsSet = 0; // must set args again for next call
    return result;
}

// Explicit instantiation (required for external linkage)
template class RcppVector<int>;
template class RcppVector<double>;
template class RcppMatrix<int>;
template class RcppMatrix<double>;

RcppSEXP getSEXP(RcppDate& date) {
    int numProtected = 0;
    SEXP value = PROTECT(allocVector(REALSXP, 1));
    numProtected++;
    SEXP dateclass = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(dateclass, 0, mkChar("Date"));
    REAL(value)[0] = date.getRValue();
    setAttrib(value, R_ClassSymbol, dateclass); 
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppDate& date) {
    RcppSEXP sp = getSEXP(date);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppDateTime& date) {
    int numProtected = 0;
    SEXP value = PROTECT(allocVector(REALSXP, 1));
    numProtected++;
    SEXP dateclass = PROTECT(allocVector(STRSXP,2));
    numProtected++;
    SET_STRING_ELT(dateclass, 0, mkChar("POSIXt"));
    SET_STRING_ELT(dateclass, 1, mkChar("POSIXct"));
    REAL(value)[0] = date.getRValue();
    setAttrib(value, R_ClassSymbol, dateclass); 
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppDateTime& date) {
    RcppSEXP sp = getSEXP(date);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(double x) {
    int numProtected = 0;
    SEXP value = PROTECT(allocVector(REALSXP, 1));
    numProtected++;
    REAL(value)[0] = x;
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, double x) {
    RcppSEXP sp = getSEXP(x);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(int i) {
    int numProtected = 0;
    SEXP value = PROTECT(allocVector(INTSXP, 1));
    numProtected++;
    INTEGER(value)[0] = i;
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, int i) {
    RcppSEXP sp = getSEXP(i);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(string strvalue) {
    int numProtected = 0;
    SEXP value = PROTECT(allocVector(STRSXP, 1));
    numProtected++;
    SET_STRING_ELT(value, 0, mkChar(strvalue.c_str()));
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, string strvalue) {
    RcppSEXP sp = getSEXP(strvalue);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(double *vec, int len) {
    int numProtected = 0;
    if(vec == 0)
	throw std::range_error("RcppResultSet::add: NULL double vector");
    SEXP value = PROTECT(allocVector(REALSXP, len));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < len; i++)
	rp[i] = vec[i];
    return RcppSEXP(value, numProtected);
}

void RcppResultSet::add(string name, double *vec, int len) {
    RcppSEXP sp = getSEXP(vec, len);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppDateVector& datevec) {
    int numProtected = 0;
    if(datevec.size() == 0) RcppSEXP(R_NilValue, 0);
    SEXP value = PROTECT(allocVector(REALSXP, datevec.size()));
    numProtected++;
    SEXP dateclass = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(dateclass, 0, mkChar("Date"));
    setAttrib(value, R_ClassSymbol, dateclass); 
    double *rp = REAL(value);
    for(int i = 0; i < (int)datevec.size(); i++)
	rp[i] = datevec(i).getRValue();
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppDateVector& datevec) {
    RcppSEXP sp = getSEXP(datevec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppDateTimeVector& datevec) {
    int numProtected = 0;
    if(datevec.size() == 0) return RcppSEXP(R_NilValue, 0);
    SEXP value = PROTECT(allocVector(REALSXP, datevec.size()));
    numProtected++;
    SEXP dateclass = PROTECT(allocVector(STRSXP,2));
    numProtected++;
    SET_STRING_ELT(dateclass, 0, mkChar("POSIXt"));
    SET_STRING_ELT(dateclass, 1, mkChar("POSIXct"));
    setAttrib(value, R_ClassSymbol, dateclass); 
    double *rp = REAL(value);
    for(int i = 0; i < (int)datevec.size(); i++)
	rp[i] = datevec(i).getRValue();
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppDateTimeVector& datevec) {
    RcppSEXP sp = getSEXP(datevec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppStringVector& stringvec) {
    int numProtected = 0;
    int len = (int)stringvec.size();
    SEXP value = PROTECT(allocVector(STRSXP, len));
    numProtected++;
    for(int i = 0; i < len; i++)
        SET_STRING_ELT(value, i, mkChar(stringvec(i).c_str()));
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppStringVector& stringvec) {
    RcppSEXP sp = getSEXP(stringvec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(int *vec, int len) {
    int numProtected = 0;
    if(vec == 0)
	throw std::range_error("RcppResultSet::add: NULL int vector");
    SEXP value = PROTECT(allocVector(INTSXP, len));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < len; i++)
	ip[i] = vec[i];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, int *vec, int len) {
    RcppSEXP sp = getSEXP(vec, len);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(double **mat, int nx, int ny) {
    int numProtected = 0;
    if(mat == 0)
	throw std::range_error("RcppResultSet::add: NULL double matrix");
    SEXP value = PROTECT(allocMatrix(REALSXP, nx, ny));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    rp[i + nx*j] = mat[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, double **mat, int nx, int ny) {
    RcppSEXP sp = getSEXP(mat, nx, ny);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(int **mat, int nx, int ny) {
    int numProtected = 0;
    if(mat == 0)
	throw std::range_error("RcppResultSet::add: NULL int matrix");
    SEXP value = PROTECT(allocMatrix(INTSXP, nx, ny));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    ip[i + nx*j] = mat[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, int **mat, int nx, int ny) {
    RcppSEXP sp = getSEXP(mat, nx, ny);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(vector<string>& vec) {
    int numProtected = 0;
    if(vec.size() == 0)
	throw std::range_error("RcppResultSet::add; zero length vector<string>");
    int len = (int)vec.size();
    SEXP value = PROTECT(allocVector(STRSXP, len));
    numProtected++;
    for(int i = 0; i < len; i++)
        SET_STRING_ELT(value, i, mkChar(vec[i].c_str()));
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, vector<string>& vec) {
    RcppSEXP sp = getSEXP(vec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(vector<int>& vec) {
    int numProtected = 0;
    if(vec.size() == 0)
	throw std::range_error("RcppResultSet::add; zero length vector<int>");
    int len = (int)vec.size();
    SEXP value = PROTECT(allocVector(INTSXP, len));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < len; i++)
	ip[i] = vec[i];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, vector<int>& vec) {
    RcppSEXP sp = getSEXP(vec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(vector<double>& vec) {
    int numProtected = 0;
    if(vec.size() == 0)
	throw std::range_error("RcppResultSet::add; zero length vector<double>");
    int len = (int)vec.size();
    SEXP value = PROTECT(allocVector(REALSXP, len));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < len; i++)
	rp[i] = vec[i];
    return RcppSEXP(value,numProtected);
}
void RcppResultSet::add(string name, vector<double>& vec) {
    RcppSEXP sp = getSEXP(vec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(vector<vector<int> >& mat) {
    int numProtected = 0;
    if(mat.size() == 0)
	throw std::range_error("RcppResultSet::add: zero length vector<vector<int> >");
    else if(mat[0].size() == 0)
	throw std::range_error("RcppResultSet::add: no columns in vector<vector<int> >");
    int nx = (int)mat.size();
    int ny = (int)mat[0].size();
    SEXP value = PROTECT(allocMatrix(INTSXP, nx, ny));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    ip[i + nx*j] = mat[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, vector<vector<int> >& mat) {
    RcppSEXP sp = getSEXP(mat);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(vector<vector<double> >& mat) {
    int numProtected = 0;
    if(mat.size() == 0)
	throw std::range_error("RcppResultSet::add: zero length vector<vector<double> >");
    else if(mat[0].size() == 0)
	throw std::range_error("RcppResultSet::add: no columns in vector<vector<double> >");
    int nx = (int)mat.size();
    int ny = (int)mat[0].size();
    SEXP value = PROTECT(allocMatrix(REALSXP, nx, ny));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    rp[i + nx*j] = mat[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, vector<vector<double> >& mat) {
    RcppSEXP sp = getSEXP(mat);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppVector<int>& vec) {
    int numProtected = 0;
    int len = (int)vec.size();
    int *a = vec.cVector();
    SEXP value = PROTECT(allocVector(INTSXP, len));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < len; i++)
	ip[i] = a[i];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppVector<int>& vec) {
    RcppSEXP sp = getSEXP(vec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppVector<double>& vec) {
    int numProtected = 0;
    int len = (int)vec.size();
    double *a = vec.cVector();
    SEXP value = PROTECT(allocVector(REALSXP, len));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < len; i++)
	rp[i] = a[i];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppVector<double>& vec) {
    RcppSEXP sp = getSEXP(vec);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppMatrix<int>& mat) {
    int numProtected = 0;
    int nx = mat.getDim1();
    int ny = mat.getDim2();
    int **a = mat.cMatrix();
    SEXP value = PROTECT(allocMatrix(INTSXP, nx, ny));
    numProtected++;
    int *ip = INTEGER(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    ip[i + nx*j] = a[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppMatrix<int>& mat) {
    RcppSEXP sp = getSEXP(mat);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppMatrix<double>& mat) {
    int numProtected = 0;
    int nx = mat.getDim1();
    int ny = mat.getDim2();
    double **a = mat.cMatrix();
    SEXP value = PROTECT(allocMatrix(REALSXP, nx, ny));
    numProtected++;
    double *rp = REAL(value);
    for(int i = 0; i < nx; i++)
	for(int j = 0; j < ny; j++)
	    rp[i + nx*j] = a[i][j];
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppMatrix<double>& mat) {
    RcppSEXP sp = getSEXP(mat);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}

RcppSEXP getSEXP(RcppZoo& zoo) {
    int numProtected = 0;
    SEXP zoosexp; // object to be created.

    // Add the data part.
    vector<int> perm = zoo.getSortPermutation();
    if(zoo.isMatrix()) { // matrix Data
	vector<vector<double> > mat = zoo.getDataMat();
	if(mat.size() == 0)
	    throw std::range_error("RcppZoo add: zero length vector<vector<double> >");
	else if(mat[0].size() == 0)
	    throw std::range_error("RcppZoo add: no columns in vector<vector<double> >");
	int nx = (int)mat.size();
	int ny = (int)mat[0].size();
	zoosexp = PROTECT(allocMatrix(REALSXP, nx, ny));
	numProtected++;
	double *rp = REAL(zoosexp);
	for(int i = 0; i < nx; i++)
	    for(int j = 0; j < ny; j++)
		rp[i + nx*j] = mat[perm[i]][j];
    }
    else { // vector Data
	vector<double> vec = zoo.getDataVec();
	if(vec.size() == 0)
	    throw std::range_error("RcppZoo add; zero length vector<double>");
	int len = (int)vec.size();
	zoosexp = PROTECT(allocVector(REALSXP, len));
	numProtected++;
	double *rp = REAL(zoosexp);
	for(int i = 0; i < len; i++)
	    rp[i] = vec[perm[i]];
    }

    // Add the class info at the top level (zoo, zooreg)
    SEXP classlist;
    if(zoo.isRegular()) {
	// zooreg object.
	classlist = PROTECT(allocVector(STRSXP,2));
	numProtected++;
	SET_STRING_ELT(classlist, 0, mkChar("zooreg"));
	SET_STRING_ELT(classlist, 1, mkChar("zoo"));
	setAttrib(zoosexp, R_ClassSymbol, classlist);

	// Add frequency attibute.
	SEXP freqsexp = PROTECT(allocVector(REALSXP,1));
	numProtected++;
	SEXP attrBufFreq = PROTECT(allocVector(STRSXP,1));
	numProtected++;
	REAL(freqsexp)[0] = zoo.getFrequency();
	SET_STRING_ELT(attrBufFreq,0,mkChar("frequency"));
	setAttrib(zoosexp, attrBufFreq, freqsexp);
    }
    else {
	// Plain zoo object (no frequency).
	classlist = PROTECT(allocVector(STRSXP,1));
	numProtected++;
	SET_STRING_ELT(classlist, 0, mkChar("zoo"));
	setAttrib(zoosexp, R_ClassSymbol, classlist); 
    }

    // Add index vector based on type.
    SEXP indexAttr;
    if(zoo.getIndType() == RcppZoo::IND_INTEGER) {
	vector<int> indvec = zoo.getIndInt();
	if(indvec.size() == 0)
	    throw std::range_error("RcppZoo add; zero length vector<int>");
	indexAttr = PROTECT(allocVector(INTSXP,indvec.size()));
	numProtected++;
	int *ip = INTEGER(indexAttr);
	for(int i = 0; i < (int)indvec.size(); ++i)
	    ip[i] = indvec[perm[i]];
    }
    else if(zoo.getIndType() == RcppZoo::IND_DOUBLE) {
	vector<double> indvec = zoo.getIndDouble();
	if(indvec.size() == 0)
	    throw std::range_error("RcppZoo add; zero length vector<double>");
	indexAttr = PROTECT(allocVector(REALSXP,indvec.size()));
	numProtected++;
	double *rp = REAL(indexAttr);
	for(int i = 0; i < (int)indvec.size(); ++i)
	    rp[i] = indvec[perm[i]];
    }
    else if(zoo.getIndType() == RcppZoo::IND_DATE) {
	vector<RcppDate> indvec = zoo.getIndDate();
	if(indvec.size() == 0)
	    throw std::range_error("RcppZoo add; zero length vector<Date>");
	indexAttr = PROTECT(allocVector(REALSXP,indvec.size()));
	numProtected++;
	double *rp = REAL(indexAttr);
	for(int i = 0; i < (int)indvec.size(); ++i)
	    rp[i] = indvec[perm[i]].getRValue();
    }
    else if(zoo.getIndType() == RcppZoo::IND_DATETIME) {
	vector<RcppDateTime> indvec = zoo.getIndDateTime();
	if(indvec.size() == 0)
	    throw std::range_error("RcppZoo add; zero length vector<DateTime>");
	indexAttr = PROTECT(allocVector(REALSXP,indvec.size()));
	numProtected++;
	double *rp = REAL(indexAttr);
	for(int i = 0; i < (int)indvec.size(); ++i)
	    rp[i] = indvec[perm[i]].getRValue();
    }
    else
	throw range_error("Unsupported index type in RcppZoo add");

    // Set index attribute.
    SEXP attrBufIndex = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(attrBufIndex, 0, mkChar("index"));
    setAttrib(zoosexp, attrBufIndex, indexAttr);

    // Set index attribute class if non-numeric.
    if(!(zoo.getIndType() == RcppZoo::IND_INTEGER 
      || zoo.getIndType() == RcppZoo::IND_DOUBLE)) {
	if(zoo.getIndType() == RcppZoo::IND_DATE) {
	    SEXP dateclass = PROTECT(allocVector(STRSXP,1));
	    numProtected++;
	    SET_STRING_ELT(dateclass, 0, mkChar("Date"));
	    setAttrib(indexAttr, R_ClassSymbol, dateclass); 
	}
	else if(zoo.getIndType() == RcppZoo::IND_DATETIME) {
	    SEXP dateclass = PROTECT(allocVector(STRSXP,2));
	    numProtected++;
	    SET_STRING_ELT(dateclass, 0, mkChar("POSIXt"));
	    SET_STRING_ELT(dateclass, 1, mkChar("POSIXct"));
	    setAttrib(indexAttr, R_ClassSymbol, dateclass); 
	}
    }
    return RcppSEXP(zoosexp, numProtected);
}
void RcppResultSet::add(string name, RcppZoo& zoo) {
    RcppSEXP sp = getSEXP(zoo);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}


RcppSEXP getSEXP(RcppFrame& frame) {
    int numProtected = 0;
    vector<string> colNames = frame.getColNames();
    vector<string> rowNames = frame.getRowNames();

    int ncol = colNames.size();
    int nrow = rowNames.size();
    SEXP rl = PROTECT(allocVector(VECSXP,ncol));
    SEXP nm = PROTECT(allocVector(STRSXP,ncol));
    numProtected += 2;

    // Set class attribute
    SEXP frameclass = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(frameclass, 0, mkChar("data.frame"));
    setAttrib(rl, R_ClassSymbol, frameclass); 

    // Set row.names attibute.
    SEXP rownamesAttr = PROTECT(allocVector(STRSXP,nrow));
    numProtected++;
    for(int i = 0; i < nrow; ++i)
	SET_STRING_ELT(rownamesAttr, i, mkChar(rowNames[i].c_str()));
    SEXP rownamesAttrName = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(rownamesAttrName, 0, mkChar("row.names"));
    setAttrib(rl, rownamesAttrName, rownamesAttr);

    for(int i=0; i < ncol; i++) {
	RcppColumn col = frame.getColumns()[i];
	SEXP value, names;
	if(col.getType() == RcppColumn::COLTYPE_DOUBLE) {
	    value = PROTECT(allocVector(REALSXP,nrow));
	    numProtected++;
	    double *rp = REAL(value);
	    for(int j=0; j < nrow; j++)
		rp[j] = (*col.colDouble)[j];
	}
	else if(col.getType() == RcppColumn::COLTYPE_INT) {
	    value = PROTECT(allocVector(INTSXP,nrow));
	    numProtected++;
	    int *ip = INTEGER(value);
	    for(int j=0; j < nrow; j++)
		ip[j] = (*col.colInt)[j];
	}
	else if(col.getType() == RcppColumn::COLTYPE_FACTOR) {
	    value = PROTECT(allocVector(INTSXP,nrow));
	    numProtected++;
	    int levels = col.colFactor->getNumLevels();
	    names = PROTECT(allocVector(STRSXP,levels));
	    numProtected++;
	    for(int i=0; i < levels; i++)
		SET_STRING_ELT(names, i, 
		mkChar(col.colFactor->getLevelName(i).c_str()));
	    int *ip = INTEGER(value);
	    for(int j=0; j < nrow; j++) {
		ip[j] = col.colFactor->getObservedLevelNum(j);
	    }
	    setAttrib(value, R_LevelsSymbol, names);
	    SEXP factorclass = PROTECT(allocVector(STRSXP,1));
	    numProtected++;
	    SET_STRING_ELT(factorclass, 0, mkChar("factor"));
	    setAttrib(value, R_ClassSymbol, factorclass); 
	}
	else if(col.getType() == RcppColumn::COLTYPE_STRING) {
	    value = PROTECT(allocVector(STRSXP,nrow));
	    numProtected++;
	    for(int j=0; j < nrow; j++) {
		SET_STRING_ELT(value, j, mkChar((*col.colString)[j].c_str()));
	    }
	}
	else if(col.getType() == RcppColumn::COLTYPE_LOGICAL) {
	    value = PROTECT(allocVector(LGLSXP,nrow));
	    numProtected++;
	    for(int j=0; j < nrow; j++) {
		LOGICAL(value)[j] = (*col.colBool)[j];
	    }
	}
	else if(col.getType() == RcppColumn::COLTYPE_DATE) {
	    value = PROTECT(allocVector(REALSXP,nrow));
	    numProtected++;
	    SEXP dateclass = PROTECT(allocVector(STRSXP,1));
	    numProtected++;
	    double *rp = REAL(value);
	    for(int j=0; j < nrow; j++)
		rp[j] = (*col.colDate)[j].getRValue();
	    SET_STRING_ELT(dateclass, 0, mkChar("Date"));
	    setAttrib(value, R_ClassSymbol, dateclass); 
	}
	else if(col.getType() == RcppColumn::COLTYPE_DATETIME) {
	    value = PROTECT(allocVector(REALSXP,nrow));
	    numProtected++;
	    SEXP dateclass = PROTECT(allocVector(STRSXP,2));
	    numProtected++;
	    SET_STRING_ELT(dateclass, 0, mkChar("POSIXt"));
	    SET_STRING_ELT(dateclass, 1, mkChar("POSIXct"));
	    setAttrib(value, R_ClassSymbol, dateclass);
	    double *rp = REAL(value);
	    for(int j=0; j < nrow; j++)
		rp[j] = (*col.colDateTime)[j].getRValue();
	}
	else {
	    throw std::range_error("RcppResultSet::add invalid column type");
	}
	SET_VECTOR_ELT(rl, i, value);
	SET_STRING_ELT(nm, i, mkChar(colNames[i].c_str()));
    }
    setAttrib(rl, R_NamesSymbol, nm);
    return RcppSEXP(rl, numProtected);
}
void RcppResultSet::add(string name, RcppFrame& frame) {
    RcppSEXP sp = getSEXP(frame);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name, sp.sexp));
}


RcppSEXP getSEXP(RcppFactor& factor) {
    int numProtected = 0;
    int numObs = factor.getNumObservations();
    int numLevels = factor.getNumLevels();
    SEXP value = PROTECT(allocVector(INTSXP,numObs));
    numProtected++;
    SEXP names = PROTECT(allocVector(STRSXP,numLevels));
    numProtected++;
    for(int i = 0; i < numLevels; ++i)
	SET_STRING_ELT(names, i, 
		       mkChar(factor.getLevelName(i).c_str()));
    int *ip = INTEGER(value);
    for(int j = 0; j < numObs; j++) {
	ip[j] = factor.getObservedLevelNum(j);
    }
    setAttrib(value, R_LevelsSymbol, names);
    SEXP factorclass = PROTECT(allocVector(STRSXP,1));
    numProtected++;
    SET_STRING_ELT(factorclass, 0, mkChar("factor"));
    setAttrib(value, R_ClassSymbol, factorclass); 
    return RcppSEXP(value, numProtected);
}
void RcppResultSet::add(string name, RcppFactor& factor) {
    RcppSEXP sp = getSEXP(factor);
    numProtected += sp.numProtected;
    values.push_back(make_pair(name,sp.sexp));
}

void RcppResultSet::add(string name, SEXP sexp, bool isProtected) {
    values.push_back(make_pair(name, sexp));
    if(isProtected)
	numProtected++;
}

SEXP RcppResultSet::getReturnList() {
    int nret = (int)values.size();
    SEXP rl = PROTECT(allocVector(VECSXP,nret));
    SEXP nm = PROTECT(allocVector(STRSXP,nret));
    list<pair<string,SEXP> >::iterator iter = values.begin();
    for(int i = 0; iter != values.end(); iter++, i++) {
	SET_VECTOR_ELT(rl, i, iter->second);
	SET_STRING_ELT(nm, i, mkChar(iter->first.c_str()));
    }
    setAttrib(rl, R_NamesSymbol, nm);
    UNPROTECT(numProtected+2);
    return rl;
}

// Print an RcppDate.
ostream& operator<<(ostream& os, const RcppDate& date) {
    os << RcppLocalTime(date);
    return os;
}
    
ostream& operator<<(ostream& os, const RcppDateTime& date) {
    os << RcppLocalTime(date);
    return os;
}
    
ostream& operator<<(ostream& os, const RcppLocalTime& dt) {
	if(dt.isPOSIXDate) {

#ifdef _WINDOWS
		char buf[80];
		if(asctime_s(buf, 80, &dt.tmdata) != 0)
		    throw range_error("asctime_s call failed");
		os << buf;
#else
		string s(asctime(&dt.tmdata));
		os << s.substr(0,s.size()-1); // trim newline.
#endif

	}
    else {
	if(dt.weekday < 0 || dt.weekday > 6)
	    throw range_error("weekday out of range");
        os << RcppDate::weekdayName[dt.weekday] << " "
	   <<dt.month << "/" << dt.day << "/" << dt.year;
    }
    return os;
}

// Used by both RcppDate and RcppDateTime.
double operator-(const RcppDate& date2, const RcppDate& date1) {
    return (date2.rdtnum - date1.rdtnum)/RcppDate::DAYS2SECS;
}

RcppDate operator+(const RcppDate& date, double offsetDays) {
    RcppDate temp(date);
    temp.rdtnum += ((int)offsetDays)*RcppDate::DAYS2SECS;
    return temp;
}
RcppDate& RcppDate::operator+=(double offsetDays) {
    rdtnum += ((int)offsetDays)*RcppDate::DAYS2SECS;
    return *this;
}
RcppDate& RcppDate::operator-=(double offsetDays) {
    rdtnum -= ((int)offsetDays)*RcppDate::DAYS2SECS;
    return *this;
}
RcppDate& RcppDate::operator++() { // prefix
    rdtnum += RcppDate::DAYS2SECS;
    return *this;
}
RcppDate RcppDate::operator++(int) { // postfix
    RcppDate temp(rdtnum);
    rdtnum += RcppDate::DAYS2SECS;
    return temp;
}
RcppDate& RcppDate::operator--() { // prefix
    rdtnum -= RcppDate::DAYS2SECS;
    return *this;
}
RcppDate RcppDate::operator--(int) { // postfix
    RcppDate temp(rdtnum);
    rdtnum -= RcppDate::DAYS2SECS;
    return temp;
}

RcppDateTime operator+(const RcppDateTime& dt, double offsetDays) {
    RcppDateTime temp;
    temp.rdtnum = dt.getrdtnum() + offsetDays*RcppDate::DAYS2SECS;
    return temp;
}

RcppDateTime& RcppDateTime::operator+=(double offsetDays) {
    rdtnum += offsetDays*RcppDate::DAYS2SECS;
    return *this;
}
RcppDateTime& RcppDateTime::operator-=(double offsetDays) {
    rdtnum -= offsetDays*RcppDate::DAYS2SECS;
    return *this;
}
RcppDateTime& RcppDateTime::operator++() { // prefix
    rdtnum += RcppDate::DAYS2SECS;
    return *this;
}
RcppDateTime RcppDateTime::operator++(int) { // postfix
    RcppDateTime temp(rdtnum);
    rdtnum += RcppDate::DAYS2SECS;
    return temp;
}
RcppDateTime& RcppDateTime::operator--() { // prefix
    rdtnum -= RcppDate::DAYS2SECS;
    return *this;
}
RcppDateTime RcppDateTime::operator--(int) { // postfix
    RcppDateTime temp(rdtnum);
    rdtnum -= RcppDate::DAYS2SECS;
    return temp;
}

// There is the possibility that these comparisons may not work
// as expected due to roundoff, but this is not an issue for
// R type Date because there is no fractional part and different
// Date's will have rdtnum values that differ by at least DAYS2SECS.
bool  operator<(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum < date2.rdtnum;
}

bool  operator>(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum > date2.rdtnum;
}

bool  operator>=(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum >= date2.rdtnum;
}

bool  operator<=(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum <= date2.rdtnum;
}

bool  operator==(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum == date2.rdtnum;
}

bool operator!=(const RcppDate &date1, const RcppDate& date2) {
    return date1.rdtnum != date2.rdtnum;
}

// Class constants.
const int RcppDate::R_Offset = 2440588; // Julian day number for 1/1/1970, JDN(1/1/1970).
const int RcppDate::Excel1900_Offset = 2415019; // JDN(1/1/1900) - 2
const int RcppDate::Excel1904_Offset = 2416481; // JDN(1/1/1904)
const int RcppDate::DAYS2SECS = 60*60*24;
const int RcppDate::daysInMonthTable[2][13] = {
	    {0,31,28,31,30,31,30,31,31,30,31,30,31},
	    {0,31,29,31,30,31,30,31,31,30,31,30,31}
};
const string RcppDate::weekdayName[] = { "Sun", "Mon", "Tue", "Wed", 
					 "Thu", "Fri", "Sat", "Sun" };
const string RcppDate::monthName[] = { "Jan", "Feb", "Mar", "Apr",
				       "May", "Jun", "Jul", "Aug", "Sep",
				       "Oct", "Nov", "Dec" };

// The Julian day number (jdn) is the number of days since Monday,
// Jan 1, 4713BC (year = -4712). Here 1BC is year 0, 2BC is year -1, etc.
// On the other hand, R measures days since Jan 1, 1970, and these dates are
// converted to jdn's by adding R_Offset.
//
// mdy2jdn and jdn2mdy are inverse functions for dates back to 
// year = -4799 (4800BC).
//
// See the Wikipedia entry on Julian day number for more information 
// on these algorithms.
//

// Transform month/day/year to Julian day number.
int RcppDate::mdy2jdn(int month, int day, int year) {
    int m = month, d = day, y = year;
    int a = (14 - m)/12;
    y += 4800 - a;
    m += 12*a - 3;
    int jdn = (d + (153*m + 2)/5 + 365*y
	       + y/4 - y/100 + y/400 - 32045);
    return(jdn);
}

// Transform from Julian day number to month/day/year.
RcppMDY RcppDate::jdn2mdy(int jdn) {
    int jul = jdn + 32044;
    int g = jul/146097;
    int dg = jul % 146097;
    int c = (dg/36524 + 1)*3/4;
    int dc = dg - c*36524;
    int b = dc/1461;
    int db = dc % 1461;
    int a = (db/365 + 1)*3/4;
    int da = db - a*365;
    int y = g*400 + c*100 + b*4 + a;
    int m = (da*5 + 308)/153 - 2;
    int d = da - (m + 4)*153 /5 + 122;
    y = y - 4800 + (m + 2)/12;
    m = (m + 2) % 12 + 1;
    d = d + 1;
    return RcppMDY(m,d,y);
}

// Returns the day of the week for the current date. A
// favorite trick done effortlessly by savants, probably
// by another method!
int RcppDate::getWeekday() const { // Sun=0, Mon=1, ..., Sat=6
    int jdn = numJulian();
    return (jdn+1)%7;
}

// Returns date of next weekday (RcppDate::Mon, 
// RcppDate::Tue, etc.), not including the current date.
RcppDate RcppDate::nextWeekday(int dayNum) const {
    int jdn = numJulian();
    int weekday = (jdn+1)%7;
    int offset = (dayNum - weekday + 7) % 7;
    if(offset == 0)
	offset += 7;
    return RcppDate(jdn + offset,true);
}

// Returns the nth-occurence of the specified weekday in the
// current month and year, where the first day of the month is
// counted if it happens to be the specified weekday.
RcppDate RcppDate::nthWeekday(int n, int dayNum) const {
    int count = 0;
    RcppLocalTime thisTime(*this);
    RcppDate date(thisTime.getMonth(), 1, thisTime.getYear());
    if(date.getWeekday() != dayNum)
	date = date.nextWeekday(dayNum);
    for(count = 1; count < n; count++)
	date = RcppDate(date.numJulian()+7,true);
    return date;
}

// Returns true if the current date falls in a leap year.
bool RcppDate::isLeapYear() const {
    RcppLocalTime thisTime(*this);
    int year = thisTime.getYear();
    return ((year%4 == 0)||(year%400 == 0)) && !(year%100 == 0);
}

// Corresponding static function.
bool RcppDate::isLeapYear(int year) {
    return ((year%4 == 0)||(year%400 == 0)) && !(year%100 == 0);
}

// Returns number of days in specified month and year.
int RcppDate::daysInMonth(int month, int year) {
    if(month < 1 || month > 12)
	throw std::range_error("Invalid month/year in daysInMonth");
    return daysInMonthTable[isLeapYear(year)?1:0][month];
}

#include <string.h>

// Paul Roebuck has observed that the memory used by an exception message
// is not reclaimed if error() is called inside of a catch block (due to
// a setjmp() call), and he suggested the following work-around.
char *copyMessageToR(const char* const mesg) {
    char* Rmesg;
    char prefix[20] = "Exception: ";
    void* Rheap = R_alloc(strlen(prefix)+strlen(mesg)+1,sizeof(char));
    Rmesg = static_cast<char*>(Rheap);
#ifdef _WINDOWS
	if(strcpy_s(Rmesg, strlen(prefix)+strlen(mesg)+1,prefix) != 0)
		throw range_error("strcpy error in copyMessageToR");
	if(strcat_s(Rmesg, strlen(prefix)+strlen(mesg)+1,mesg) != 0)
		throw range_error("strcat error in copyMessageToR");
#else
    strcpy(Rmesg, prefix);
	strcat(Rmesg, mesg);
#endif

    return Rmesg;
}

} // end namespace Rcpp


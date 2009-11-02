// Rcpp.hpp: Part of the R/C++ Object Mapping Library
//
// Copyright (C) 2005-2009 Dominick Samperi
//
// As an extension of R this software is naturally released under the 
// same terms, namely, the GNU General Public License Version 2.
// For more information go to http://www.gnu.org/licenses/gpl.html.

#ifndef Rcpp_hpp
#define Rcpp_hpp

#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <R.h>
#include <Rinternals.h>

#ifdef _USRDLL
#define RcppExport extern "C" __declspec(dllexport)
#else
#define RcppExport extern "C"
#endif

namespace Rcpp {

    const char* getRcppVersionCStr();

/**
 * Returns the string value of an object that knows how to stream itself.
 */
template <typename T>
std::string to_string(const T& obj) {
    std::stringstream ss;
    ss << obj;
    return ss.str();
};

// Return value from jul2mdy()
class RcppMDY {
public:
    int month, day, year;
    RcppMDY(int month_, int day_, int year_) 
	: month(month_), day(day_), year(year_) {}
};

// Return value from getSEXP(type) functions.
class RcppSEXP {
public:
    SEXP sexp;
    int numProtected;
    RcppSEXP(SEXP sexp_, int numProtected_) 
	: sexp(sexp_), numProtected(numProtected_) {}
    RcppSEXP() : sexp(R_NilValue), numProtected(0) {}
};

/**
 * Date class that measures days since January 1, 1970.
 * The underlying real representation holds the number of seconds since
 * 1/1/1970 with 1-day resolution. For finer resolution see the 
 * \ref RcppDateTime class. To inspect the underlying month/day/year 
 * and to print use the \ref RcppLocalTime class. Subtracting two RcppDate
 * or RcppDateTime objects yields the number of days between them, including
 * a fractional part in the case of RcppDateTime. Incrementing either of
 * the date types is done in units of days (can have fractional part).
 */
class RcppDate {
protected:

    double rdtnum;

    static const int daysInMonthTable[2][13]; // 0=no-leap, 1=leap; 1<=month<=12.
public:
    friend class RcppLocalTime;

    enum RcppWeekday { Sun=0, Mon=1, Tue=2, Wed=3, Thu=4, Fri=5, Sat=6 };
    enum RcppMonth { Jan=1, Feb=2, Mar=3, Apr=4, May=5, Jun=6,
		     Jul=7, Aug=8, Sep=9, Oct=10, Nov=11, Dec=12 };
    static const int DAYS2SECS, R_Offset, Excel1900_Offset, Excel1904_Offset;
    static int mdy2jdn(int month, int day, int year); // julian from mdy
    static RcppMDY jdn2mdy(int jdn); // mdy to julian
    static const std::string weekdayName[];
    static const std::string monthName[];

    /**
     * Returns the value that R uses to represent this date type.
     */
    double getRValue() {
	return rdtnum/DAYS2SECS;
    }

    RcppDate(SEXP);

    /**
     * Construct from an R Date serial number or a julian day number (JDN).
     */
    RcppDate(int serialNum, bool isJDN = false) {
	if(isJDN)
	    rdtnum = (serialNum - R_Offset)*DAYS2SECS;
	else
	    rdtnum = serialNum*DAYS2SECS;
    }

    /**
     * Make RcppDate from R representation in days.
     */
    RcppDate(double rnum) {
	rdtnum = rnum*DAYS2SECS;
    }

    RcppDate(int month, int day, int year) {
	if(month < 1 || month > 12 || day < 1 || day > 31)
	    throw std::range_error("RcppDate: invalid date");
	int jdn = mdy2jdn(month, day, year);
	rdtnum = (double)(jdn - R_Offset)*DAYS2SECS;
    }

    RcppDate() { // default date has rdtnum = 0.
	int jdn = mdy2jdn(1,1,1970);
	rdtnum = (double)(jdn-R_Offset)*DAYS2SECS;
    }
    double getrdtnum() const { return rdtnum; }

    friend bool     operator<(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator>(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator==(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator!=(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator>=(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator<=(const RcppDate &date1, const RcppDate& date2);

    friend double   operator-(const RcppDate& date1, const RcppDate& date2);
    friend RcppDate operator+(const RcppDate &date, double offsetDays);
    RcppDate& operator+=(double offsetDays);
    RcppDate& operator-=(double offsetDays);
    RcppDate& operator++();    // increment 1 day (prefix op)
    RcppDate operator++(int);  // increment 1 day (postfix op)
    RcppDate& operator--();    // decrement 1 day (prefix op)
    RcppDate operator--(int);  // decrement 1 day (postfix op)

    /**
     * Get day of week for this date: RcppDate::Mon, RcppDate::Tue, etc.
     */
    int getWeekday() const;

    /**
     * Get the next weekday after today: dayNum == RcppDate::Wed means
     * get the next Wednesday, not including the current day if it
     * happens to be a Wednesday.
     */
    RcppDate nextWeekday(int dayNum) const;

    /**
     * Get the n-th occurence of the specified weekday in the
     * month and year determined by this date, where the first of
     * the month is counted if it happens to be the specified
     * weekday. Weekdays are RcppDate::Mon, RcppDate::Tue, etc.
     */
    RcppDate nthWeekday(int n, int dayNum) const;

    /**
     * Is this date in a leap year?
     */
    bool isLeapYear() const;

    /**
     * Is the specified year a leap year?
     */
    static bool isLeapYear(int year);

    /**
     * How many days are in the specified month and year?
     */
    static int daysInMonth(int month, int year);

    /**
     * Return the julian day number for this date.
     */
    int numJulian() const { return (int)(rdtnum/DAYS2SECS) + RcppDate::R_Offset; }

    /**
     * Return the default representation used by Excel for the current 
     * date and time when running on a PC. This is called the "1900
     * date system," and it can be changed using an Excel option. Note that
     * the time component is not included when applied to an RcppDate; use
     * RcppDateTime to capture the time component.
     */
    double numExcelPC() const { return rdtnum/DAYS2SECS 
	    + RcppDate::R_Offset - RcppDate::Excel1900_Offset; }

    /**
     * Return the default representation used by Excel for the current 
     * date and time when running on a Mac. This is called the "1904
     * date system," and it can be changed using an Excel option. Note that
     * the time component is not included when applied to an RcppDate; use
     * RcppDateTime to capture the time component.
     */
    double numExcelMac() const { return rdtnum/DAYS2SECS
	    + RcppDate::R_Offset - RcppDate::Excel1904_Offset; }

    friend std::ostream& operator<<(std::ostream& os, const RcppDate& date);
};

/**
 * Date class that measures time in seconds since 1/1/1970.
 * For consistency with \ref RcppDate the underlying representation is
 * measured in seconds, but here the resolution is in fractions of a second.
 * To inspect the underlying month/day/year/hour/minute/second/fracsec and
 * to print see the \ref RcppLocalTime class.
 */
class RcppDateTime : public RcppDate {
public:
    friend class RcppLocalTime;

    /**
     * Returns the value that R uses to represent this date type.
     */
    double getRValue() {
	return rdtnum;
    }

    RcppDateTime(SEXP); // Checkes for R POSIXt date type.

    /**
     * Make RcppDateTime from R representation (no scaling involved).
     */
    RcppDateTime(double rnum) { 
	rdtnum = rnum;
    }

    /**
     * Make RcppDateTime from month/day/year/dayFrac. For example,
     * dayFrac = .75 = 18/24 means 18:00 GMT.
     */
    RcppDateTime(int month, int day, int year, double dayFrac = 0.0) 
	: RcppDate(month,day,year) {
	rdtnum += dayFrac*DAYS2SECS; // units = seconds
    }

    RcppDateTime() : RcppDate() {} // default to 1/1/1970
    friend std::ostream& operator<<(std::ostream& os, const RcppDateTime& date);
    friend bool     operator<(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator>(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator==(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator>=(const RcppDate &date1, const RcppDate& date2);
    friend bool     operator<=(const RcppDate &date1, const RcppDate& date2);

    friend RcppDateTime operator+(const RcppDateTime& dt, double offsetSecs);
    RcppDateTime& operator+=(double offsetSecs);
    RcppDateTime& operator-=(double offsetSecs);
    RcppDateTime& operator++();    // increment 1 sec (prefix op)
    RcppDateTime operator++(int);  // increment 1 sec (postfix op)
    RcppDateTime& operator--();    // decrement 1 sec (prefix op)
    RcppDateTime operator--(int);  // decrement 1 sec (postfix op)

};

/**
 * Used to inspect the contents of an \ref RcppDate or \ref RcppDateTime
 * object and to stream these objects.
 */
class RcppLocalTime {
    int month, day, year,hour, minute, second, weekday;
    double fracsec;
    struct tm tmdata; // date/time data returned by localtime.
    bool isPOSIXDate; // tmdata is valid only when POSIXDate is true.
public:
    RcppLocalTime(const RcppDate& date);
    RcppLocalTime(const RcppDateTime& date);
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getYear() const { return year; }
    int getHour() const { return hour; }
    int getMinute() const { return minute; }
    int getSecond() const { return second; }
    int getWeekday() const { return weekday; }
    double getFracSec() const { return fracsec; }
    std::string getWeekdayName() const;
    std::string getMonthName() const;
    struct tm getTMData() const;
    friend std::ostream& operator<<(std::ostream& os, const RcppLocalTime& dt);
};

/**
 * Models an R zoo time series object with an ordered but not necessarily
 * equally spaced index. The index can be integer, double, \ref RcppDate,
 * or \ref RcppDateTime. The data part can be a vector or a 2D matrix.
 * Maintains the rows in index-order. Supports time series with a fixed
 * frequency (the regular case), and tseries objects can be handled 
 * by encapsulating them in zoo objects.
 */
class RcppZoo {

    // Vectors of ordered types.
    // Only one of the following index vectors will be used per instance,
    // but C/C++ unions cannot be used (even std::string is not allowed).
    std::vector<int> indInt;
    std::vector<double> indDouble;
    std::vector<RcppDate> indDate;
    std::vector<RcppDateTime> indDateTime;

    // Permutation vector used for sorting index and data rows.
    std::vector<int> sortPerm;

    // Data types supported: only double for now.
    std::vector<double> dataVec;
    std::vector<std::vector<double> > dataMat; // rows sorted by index.
    int indexType_;
    int frequency_;
    bool isMatrix_, isRegular_;

    void lookupError(std::string funcname) {
	throw std::range_error("Invalid call to "+funcname);
    }
public:
    enum IndexType { IND_INTEGER, IND_DOUBLE, IND_DATE, IND_DATETIME };

    // Constructor from R object.
    RcppZoo(SEXP zoosexp);

    // Constructors for use on the C++ side.
    RcppZoo(std::vector<double>& data, std::vector<int>& index, int freq=0);
    RcppZoo(std::vector<double>& data, std::vector<double>& index, int freq=0);
    RcppZoo(std::vector<double>& data, std::vector<RcppDate>& index, int freq=0);
    RcppZoo(std::vector<double>& data, std::vector<RcppDateTime>& index, int freq=0);
    RcppZoo(std::vector<std::vector<double> >& data, std::vector<int>& index, int freq=0);
    RcppZoo(std::vector<std::vector<double> >& data, std::vector<double>& index, int freq=0);
    RcppZoo(std::vector<std::vector<double> >& data, std::vector<RcppDate>& index,int freq=0);
    RcppZoo(std::vector<std::vector<double> >& data, std::vector<RcppDateTime>& index,int freq=0);
    std::vector<double>& getDataVec() { 
	if(isMatrix_) lookupError("getDataVec");
	return dataVec;
    }
    std::vector<std::vector<double> >& getDataMat() { 
	if(!isMatrix_) lookupError("getDataMat");
	return dataMat; 
    }
    std::vector<int>& getIndInt() { 
	if(indexType_ != IND_INTEGER) lookupError("getIndInt");
	return indInt; 
    }
    std::vector<double>& getIndDouble() { 
	if(indexType_ != IND_DOUBLE) lookupError("getIndDouble");
	return indDouble; 
    }
    std::vector<RcppDate>& getIndDate() { 
	if(indexType_ != IND_DATE) lookupError("getIndDate");
	return indDate; 
    }
    std::vector<RcppDateTime>& getIndDateTime() { 
	if(indexType_ != IND_DATETIME) lookupError("getIndDateTime");
	return indDateTime; 
    }
    bool isMatrix() { return isMatrix_; }
    bool isRegular() { return isRegular_; }
    int getIndType() { return indexType_; }
    int getFrequency() { return frequency_; }
    std::vector<int> getSortPermutation() { return sortPerm; }
};

/**
 * Models an R factor, that is, a vector of observations where the possible
 * outcomes take values in a finite set of string values (called levelNames
 * here).
 */
class RcppFactor {
    // This is the unique list of level names (sorted) for an R factor.
    // The numeric value that R uses for levelNames[i] is i+1.
    std::vector<std::string> levelNames; // names indexed by observations.
    std::vector<int> observations;  // observations are pointers into levelNames.
public:
    RcppFactor(SEXP fac); // from R
    RcppFactor(std::vector<std::string> names); // from C++

    /**
     * Returns the index into the vector of level names corresponding to
     * the i-th observation. This vector is zero-based, so this is
     * one less than what R calls the level number for this
     * observation.
     */
    int getObservedLevelIndex(int i) {
	if(i >= 0 && i < (int)observations.size())
	    return observations[i];
	else
	    throw std::range_error("RcppFactor index out of range");
    }

    /**
     * Returns what R calls the level value for the i-th observation.
     */
    int getObservedLevelNum(int i) { return getObservedLevelIndex(i)+1; }

    /**
     * Returns the string value of the i-th observation.
     */
    std::string getObservedLevelStr(int i) { 
	return levelNames[getObservedLevelIndex(i)]; 
    }

    /**
     * Returns the j-th level name from the sorted vector of 
     * unique level names.
     */
    std::string getLevelName(int j) { 
	if(j >= 0 && j < (int)levelNames.size())
	    return levelNames[j];
	else
	    throw std::range_error("RcppFactor level number out of range");
    }

    int getNumObservations() { return observations.size(); }
    int getNumLevels() { return levelNames.size(); }
    void print(); // useful for debugging.
};

/**
 * Models one column of an R data frame. Can be of type double, int,
 * string, bool, \ref RcppFactor, \ref RcppDate, or \ref RcppDateTime.
 */
class RcppColumn {
    // cleanup true means this class takes responsibility for deleting
    // the space pointed to by the corresponding column pointer. False
    // means we point to user data and assume that it will not
    // go out of scope before this object does.
    bool needsCleanup;
    void lookupError(std::string typeStr) const {
	std::string mesg=std::string("Invalid ")+typeStr+"Value request in RcppColumn";
	throw std::range_error(mesg);
    }
public:
enum ColType { COLTYPE_DOUBLE, COLTYPE_INT, COLTYPE_STRING,
	       COLTYPE_FACTOR, COLTYPE_LOGICAL, COLTYPE_DATE, 
	       COLTYPE_DATETIME };

    ColType type;

    // Only one of the following column vectors will be used per instance.
    std::vector<int>* colInt;
    std::vector<double>* colDouble;
    std::vector<std::string>* colString;
    std::vector<bool>* colBool;
    std::vector<RcppDate>* colDate;
    std::vector<RcppDateTime>* colDateTime;
    RcppFactor* colFactor;

	RcppColumn(const RcppColumn& col);
	~RcppColumn();

    // The reference contructors are safe because they make a copy.
    // The pointer constructors are dangerous because they point
    // back to the user's data vectors (and may be faster due to
    // reduced copying overhead). To use the pointer constructors
    // you must not permit the data vectors referenced to go out of scope.
    RcppColumn(std::vector<int>& colInt_) {
	colInt = new std::vector<int>(colInt_);
	needsCleanup = true;
	type=COLTYPE_INT;
    }
    RcppColumn(std::vector<double>& colDouble_) {
	colDouble = new std::vector<double>(colDouble_);
	needsCleanup = true;
	type=COLTYPE_DOUBLE;
    }
    RcppColumn(std::vector<std::string>& colString_) {
	colString = new std::vector<std::string>(colString_);
	needsCleanup = true;
	type=COLTYPE_STRING;
    }
    RcppColumn(std::vector<bool>& colBool_) {
	colBool = new std::vector<bool>(colBool_);
	needsCleanup = true;
	type=COLTYPE_LOGICAL;
    }
    RcppColumn(std::vector<RcppDate>& colDate_) {
	colDate = new std::vector<RcppDate>(colDate_);
	needsCleanup = true;
	type=COLTYPE_DATE;
    }
    RcppColumn(std::vector<RcppDateTime>& colDateTime_) {
	colDateTime = new std::vector<RcppDateTime>(colDateTime_);
	needsCleanup = true;
	type=COLTYPE_DATETIME;
    }
    RcppColumn(RcppFactor& colFactor_) {
	colFactor = new RcppFactor(colFactor_);
	needsCleanup = true;
	type=COLTYPE_FACTOR;
    }

    // Pointer constructors: use with care!
    RcppColumn(std::vector<int>* colInt_) : colInt(colInt_) {
	needsCleanup = false;
	type=COLTYPE_INT;
    }
    RcppColumn(std::vector<double>* colDouble_) : colDouble(colDouble_) {
	needsCleanup = false;
	type=COLTYPE_DOUBLE;
    }
    RcppColumn(std::vector<std::string>* colString_) : colString(colString_) {
	needsCleanup = false;
	type=COLTYPE_STRING;
    }
    RcppColumn(std::vector<bool>* colBool_) : colBool(colBool_) {
	needsCleanup = false;
	type=COLTYPE_LOGICAL;
    }
    RcppColumn(std::vector<RcppDate>* colDate_) : colDate(colDate_) {
	needsCleanup = false;
	type=COLTYPE_DATE;
    }
    RcppColumn(std::vector<RcppDateTime>* colDateTime_) : colDateTime(colDateTime_) {
	needsCleanup = false;
	type=COLTYPE_DATETIME;
    }
    RcppColumn(RcppFactor* colFactor_) : colFactor(colFactor_) {
	needsCleanup = false;
	type=COLTYPE_FACTOR;
    }

    ColType getType() { return type; }
    
    int getIntValue(int i) const { 
	if(type != COLTYPE_INT) lookupError("Int");
	return (*colInt)[i];
    }
    double getDoubleValue(int i) const {
	if(type != COLTYPE_DOUBLE) lookupError("Double");
	return (*colDouble)[i];
    }
    std::string getStringValue(int i) const {
	if(type != COLTYPE_STRING) lookupError("String");
	return (*colString)[i];
    }
    bool getBoolValue(int i) const {
	if(type != COLTYPE_LOGICAL) lookupError("Bool");
	return (*colBool)[i];
    }
    RcppDate getDateValue(int i) const {
	if(type != COLTYPE_DATE) lookupError("Date");
	return (*colDate)[i];
    }
    RcppDateTime getDateTimeValue(int i) const {
	if(type != COLTYPE_DATETIME) lookupError("DateTime");
	return (*colDateTime)[i];
    }
    std::string getFactorValue(int i) const { 
	if(type != COLTYPE_FACTOR) lookupError("Factor");
	return colFactor->getObservedLevelStr(i);
    }

    int size() {
	switch(type) {
	case COLTYPE_INT:
	    return colInt->size();
	case COLTYPE_DOUBLE:
	    return colDouble->size();
	case COLTYPE_STRING:
	    return colString->size();
	case COLTYPE_LOGICAL:
	    return colBool->size();
	case COLTYPE_FACTOR:
	    return colFactor->getNumObservations();
	case COLTYPE_DATE:
	    return colDate->size();
	case COLTYPE_DATETIME:
	    return colDateTime->size();
	}
	return -1;
    }

    void print();


};

/**
 * Models an R data frame as a vector of RcppColumn's, with a corresponding
 * vector of column names and row names consistent with R's representation.
 */
class RcppFrame {
    std::vector<std::string> rowNames;
    std::vector<std::string> colNames;
    std::vector<RcppColumn> cols;
public:
    RcppFrame(SEXP df);
    RcppFrame(std::vector<std::string> rowNames_, std::vector<std::string> colNames_,
	       std::vector<RcppColumn> cols_) : rowNames(rowNames_),
					   colNames(colNames_),
					   cols(cols_) {
	bool badInput = (cols.size() <= 0) || (cols[0].size() <= 0);
	if(!badInput) {
	    if(cols.size() != colNames.size())
		badInput = true;
	    for(int i=0; i < (int)cols.size(); ++i)
		if(cols[i].size() != (int)rowNames.size())
		    badInput = true;
	}
	if(badInput)
	    throw std::range_error("Inconsistent dims in RcppFrame constructor");
    }
    void print() { // for debugging
	for(int c = 0; c < (int)cols.size(); ++c) {
	    cols[c].print();
	}
    }
    int getNumRows() { return rowNames.size(); }
    int getNumCols() { return colNames.size(); }
    std::vector<std::string>& getRowNames() { return rowNames; }
    std::vector<std::string>& getColNames() { return colNames; }
    std::vector<RcppColumn>& getColumns() { return cols; }
};

/**
 * Used to process an input parameter list consisting of name-value
 * pairs, where the values can be of different types: double, int,
 * string, bool, Date, or DateTime.
 */
class RcppParams {
public:
    RcppParams(SEXP params);
    void   checkNames(char *inputNames[], int len);
    double getDoubleValue(std::string name);
    int    getIntValue(std::string name);
    std::string getStringValue(std::string name);
    bool   getBoolValue(std::string name);
    RcppDate getDateValue(std::string name);
    RcppDateTime getDateTimeValue(std::string name);
private:
    std::map<std::string, int> pmap;
    SEXP _params;
};

/**
 * Used to process an input parameter list of name-value pairs where
 * the values are all numbers. This class is redundant and should
 * probably be removed / deprecated.
 */
class RcppNumList {
public:
    RcppNumList(SEXP theList);
    std::string getName(int i);
    double getValue(int i);
    int size() { return len; }
private:
    int len;
    SEXP namedList;
    SEXP names;
};

/** 
 * Fast but dangerous vector class that works directly with R's memory.
 * The SEXP provided must be PROTECT-ed if allocated by the user (not
 * necessary if it came in as a parameter to .Call). Use with caution.
*/
class RcppRVector {
    SEXP sexp;
    double *rp;
    int len;
public:
    RcppRVector(SEXP sexp_) : sexp(sexp_) {
	if(!isVector(sexp) || !isReal(sexp))
	    throw std::range_error("RcppRVector: not a double vector");
	rp = REAL(sexp);
	len = length(sexp);
    }
    inline double& operator()(int i) { 
	    if(i < 0 || i >= len) // Don't clobber R's memory...
		throw std::range_error("RcppRVector: index out of bounds");
	    return rp[i]; 
	}
    int size() { return len; }
};

/**
 * Fast but dangerous matrix class that works directly with R's memory.
 * The SEXP provided must be PROTECT-ed if allocated by the user (not
 * necessary if it came in as a parameter to .Call). Use with caution.
 */
class RcppRMatrix {
    SEXP sexp;
    double *rp;
    int nrows, ncols;
public:
    RcppRMatrix(SEXP sexp_) : sexp(sexp_) {
	if(!isMatrix(sexp) || !isReal(sexp))
	    throw std::range_error("RcppRMatrix: not a double matrix");
	rp = REAL(sexp);
	SEXP dimAttrM = getAttrib(sexp, R_DimSymbol);
	nrows = INTEGER(dimAttrM)[0];
	ncols = INTEGER(dimAttrM)[1];
    } // no range checking!
    inline double& operator()(int i, int j) { 
	if(i < 0 || i >= nrows || j < 0 || j >= ncols)
	    throw std::range_error("RcppRMatrix: index out of bounds");
	return rp[i+nrows*j]; 
    }
    int getNumRows() { return nrows; }
    int getNumCols() { return ncols; }
};

/**
 * Models an R vector of integer or double.
 */
template <typename T>
class RcppVector {
public:
    RcppVector(SEXP vec);
    RcppVector(int len);
    int size() { return len; }
    inline T& operator()(int i) {
	if(i < 0 || i >= len) {
	    throw std::range_error("RcppVector: subscript out of range: "+to_string(i));
	}
	return v[i];
    }
    T *cVector();
    std::vector<T> stlVector();
private:
    int len;
    T *v;
};

/**
 * Models an R string vector.
 */
class RcppStringVector {
public:
    RcppStringVector(SEXP vec);
    ~RcppStringVector() {
	delete [] v;
    }
    inline std::string& operator()(int i) {
	if(i < 0 || i >= length) {
	    throw std::range_error("RcppStringVector: subscript out of range: "+to_string(i));
	}
	return v[i];
    }
    int size() { return length; }
private:
    std::string *v;
    int length;
};

/**
 * Models an R Date vector.
 */
class RcppDateVector {
public:
    RcppDateVector(SEXP vec);
    ~RcppDateVector() {
	delete [] v;
    }
    inline RcppDate& operator()(int i) {
	if(i < 0 || i >= length) {
	    throw std::range_error("RcppDateVector: subscript out of range: "+to_string(i));
	}
	return v[i];
    }
    int size() { return length; }
private:
    RcppDate *v;
    int length;
};

/**
 * Models an R DateTime vector.
 */
class RcppDateTimeVector {
public:
    RcppDateTimeVector(SEXP vec);
    ~RcppDateTimeVector() {
	delete [] v;
    }
    inline RcppDateTime& operator()(int i) {
	if(i < 0 || i >= length) {
	    throw std::range_error("RcppDateTimeVector: subscript out of range: "+to_string(i));
	}
	return v[i];
    }
    int size() { return length; }
private:
    RcppDateTime *v;
    int length;
};

/**
 * Models an R numeric matrix.
 */
template <typename T>
class RcppMatrix {
public:
    RcppMatrix(SEXP mat);
    RcppMatrix(int nx, int ny);
    int getDim1() { return dim1; }
    int getDim2() { return dim2; }
    inline T& operator()(int i, int j) {
	if(i < 0 || i >= dim1 || j < 0 || j >= dim2) {
	    throw std::range_error("RcppMatrix: subscripts out of range: "+to_string(i)+", "+to_string(j));
	}
	return a[i][j];
    }
    T **cMatrix();
    std::vector<std::vector<T> > stlMatrix();
private:
    int dim1, dim2;
    T **a;
};

class RcppFunction {
    SEXP fn;
    int nArgs, numProtected, numArgsSet;
    SEXP *args;
public:
    RcppFunction(SEXP fn_, int nArgs_) 
	: fn(fn_), nArgs(nArgs_), numProtected(0) {
	if(!isFunction(fn))
	    throw std::range_error("RcppFunction: not a function");
	if(nArgs <= 0)
	    throw std::range_error("RcppFunction: bad num of args");
	args = new SEXP[nArgs];
	numArgsSet = 0;
    }
    SEXP callR();
    void setNextArg(const RcppSEXP& sp) {
	if(numArgsSet == nArgs)
	    throw std::range_error("RcppFunction: too many args supplied");
	numProtected += sp.numProtected;
	args[numArgsSet++] = sp.sexp;
    }
};

RcppSEXP getSEXP(double);
RcppSEXP getSEXP(int);
RcppSEXP getSEXP(std::string);
RcppSEXP getSEXP(double *, int);
RcppSEXP getSEXP(int *, int);
RcppSEXP getSEXP(double **, int, int);
RcppSEXP getSEXP(int **, int, int);
RcppSEXP getSEXP(RcppDateTime&);
RcppSEXP getSEXP(RcppDate&);
RcppSEXP getSEXP(RcppFrame&);
RcppSEXP getSEXP(RcppZoo&);
RcppSEXP getSEXP(RcppFactor&);

RcppSEXP getSEXP(std::vector<std::vector<double> >&);
RcppSEXP getSEXP(std::vector<std::vector<int> >&);
RcppSEXP getSEXP(RcppMatrix<int>&);
RcppSEXP getSEXP(RcppMatrix<double>&);
RcppSEXP getSEXP(RcppStringVector&);
RcppSEXP getSEXP(std::vector<std::string>&);

// These operations are redundant because they can be accomplished
// using the template function below. Should deprecate.
RcppSEXP getSEXP(RcppDateVector&);
RcppSEXP getSEXP(RcppDateTimeVector&);
RcppSEXP getSEXP(std::vector<double>&);
RcppSEXP getSEXP(std::vector<int>&);
RcppSEXP getSEXP(RcppVector<int>&);
RcppSEXP getSEXP(RcppVector<double>&);

// Template add for vector/list/etc. of RcppDate or
// RcppDateTime objects. Expects the elements of the
// collection to be of type RcppDate or RcppDateTime,
// and in particular, assumes that this type provides
// the method getRValue() returning double.
template <typename Iterator>
RcppSEXP getSEXP(Iterator first, Iterator last) {
    int numProtected = 0;
    if(first == last) return RcppSEXP(R_NilValue, 0);

    // Allocate return vector.
    int size = distance(first, last);
    SEXP value = PROTECT(allocVector(REALSXP, size));
    numProtected++;

    // Set R type and fill R vectors.
    if(typeid(*first) == typeid(RcppDateTime)) {
	int i;
	Iterator aux;
	SEXP Rclass = PROTECT(allocVector(STRSXP,2));
	numProtected++;	
	SET_STRING_ELT(Rclass, 0, mkChar("POSIXt"));
	SET_STRING_ELT(Rclass, 1, mkChar("POSIXct"));
	setAttrib(value, R_ClassSymbol, Rclass);
	double *rp = REAL(value);
	for(aux = first, i=0; aux != last; ++aux, ++i)
	    rp[i] = aux->getRValue();
    }
    else if(typeid(*first) == typeid(RcppDate)) {
	int i;
	Iterator aux;
	SEXP Rclass = PROTECT(allocVector(STRSXP,1));
	numProtected++;
	SET_STRING_ELT(Rclass, 0, mkChar("Date"));
	setAttrib(value, R_ClassSymbol, Rclass);
	double *rp = REAL(value);
	for(aux = first, i=0; aux != last; ++aux, ++i)
	    rp[i] = aux->getRValue();
    }
    else
	throw std::range_error("Invalid type in add date template func");
    return RcppSEXP(value, numProtected);
}


/**
 * Used to build the result list before returning to R.
 */
class RcppResultSet {
public:
    RcppResultSet() { numProtected = 0; }
    void add(std::string, double);
    void add(std::string, int);
    void add(std::string, std::string);
    void add(std::string, double *, int);
    void add(std::string, int *, int);
    void add(std::string, double **, int, int);
    void add(std::string, int **, int, int);
    void add(std::string, RcppDateTime&);
    void add(std::string, RcppDate&);
    void add(std::string, RcppFrame&);
    void add(std::string, RcppZoo&);
    void add(std::string, RcppFactor&);

    void add(std::string, std::vector<std::vector<double> >&);
    void add(std::string, std::vector<std::vector<int> >&);
    void add(std::string, RcppMatrix<int>&);
    void add(std::string, RcppMatrix<double>&);
    void add(std::string, RcppStringVector&);
    void add(std::string, std::vector<std::string>&);

    // These operations are redundant because they can be accomplished
    // using the template function below. Should deprecate.
    void add(std::string, RcppDateVector&);
    void add(std::string, RcppDateTimeVector&);
    void add(std::string, std::vector<double>&);
    void add(std::string, std::vector<int>&);
    void add(std::string, RcppVector<int>&);
    void add(std::string, RcppVector<double>&);

    // Template add for vector/list/etc. of RcppDate or
    // RcppDateTime objects. Expects the elements of the
    // collection to be of type RcppDate or RcppDateTime,
    // and in particular, assumes that this type provides
    // the method getRValue() returning double.
    template <typename Iterator>
    void add(std::string name, Iterator first, Iterator last) {
	RcppSEXP sp = getSEXP(first, last);
	numProtected += sp.numProtected;
	values.push_back(make_pair(name, sp.sexp));
    }

    void add(std::string, SEXP, bool isProtected);
    SEXP getReturnList();
protected:
    int numProtected;
    std::list<std::pair<std::string,SEXP> > values;
};


// Not sure if this is really needed? See end of Rcpp.cpp.
char *copyMessageToR(const char* const mesg);

} // end namespace Rcpp

#endif

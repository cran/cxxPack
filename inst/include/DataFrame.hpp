// DateFrame.hpp: models an R data frame
//
// Copyright (C) 2009-2010 Dominick Samperi
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

#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <string>
#include <vector>

#include <Rcpp.h>

#include <FinDate.hpp>
#include <Factor.hpp>

namespace cxxPack {

/**
 * Models one column of an R data frame. Can be of type double, int,
 * string, bool, Factor, FinDate, RcppDate, and RcppDatetime.
 */
class FrameColumn {

    void lookupError(std::string typeStr) const {
	std::string mesg=std::string("Invalid ")+typeStr+"Value request in FrameColumn";
	throw std::range_error(mesg);
    }
public:
enum ColType { COLTYPE_DOUBLE, COLTYPE_INT, COLTYPE_STRING,
	       COLTYPE_FACTOR, COLTYPE_LOGICAL, COLTYPE_FINDATE,
	       COLTYPE_RCPPDATE, COLTYPE_RCPPDATETIME, COLTYPE_NONE };

    ColType type;

    // Only one of the following column vectors will be used per instance.
    std::vector<int>* colInt;
    std::vector<double>* colDouble;
    std::vector<std::string>* colString;
    std::vector<bool>* colBool;
    std::vector<FinDate>* colFinDate;
    std::vector<RcppDate>* colRcppDate;
    std::vector<RcppDatetime>* colRcppDatetime;
    Factor* colFactor;

    FrameColumn() { type = COLTYPE_NONE; }
    FrameColumn(const FrameColumn& col);
    ~FrameColumn();

    FrameColumn& operator=(const FrameColumn& col);

    FrameColumn(int type_, int nrows) {
	type = (ColType)type_;
	switch(type) {
	    case COLTYPE_INT:
		colInt = new std::vector<int>(nrows);
		break;
	    case COLTYPE_DOUBLE:
		colDouble = new std::vector<double>(nrows);
		break;
	    case COLTYPE_STRING:
		colString = new std::vector<std::string>(nrows);
		break;
	    case COLTYPE_FINDATE:
		colFinDate = new std::vector<FinDate>(nrows);
		break;
	    case COLTYPE_RCPPDATE:
		colRcppDate = new std::vector<RcppDate>(nrows);
		break;
	    case COLTYPE_RCPPDATETIME:
		colRcppDatetime = new std::vector<RcppDatetime>(nrows);
		break;
	    case COLTYPE_LOGICAL:
	    case COLTYPE_FACTOR:
		throw std::range_error("Factor/Logical cols not permitted in this RcppFrame constructor");
	default:
		throw std::range_error("Invalid column type in FrameColumn constructor");
		break;
		    
	}
    }

    FrameColumn(std::vector<int>& colInt_) {
	colInt = new std::vector<int>(colInt_);
	type=COLTYPE_INT;
    }
    FrameColumn(std::vector<double>& colDouble_) {
	colDouble = new std::vector<double>(colDouble_);
	type=COLTYPE_DOUBLE;
    }
    FrameColumn(std::vector<std::string>& colString_) {
	colString = new std::vector<std::string>(colString_);
	type=COLTYPE_STRING;
    }
    FrameColumn(std::vector<bool>& colBool_) {
	colBool = new std::vector<bool>(colBool_);
	type=COLTYPE_LOGICAL;
    }
    FrameColumn(std::vector<FinDate>& colFinDate_) {
	colFinDate = new std::vector<FinDate>(colFinDate_);
	type=COLTYPE_FINDATE;
    }
    FrameColumn(std::vector<RcppDate>& colRcppDate_) {
	colRcppDate = new std::vector<RcppDate>(colRcppDate_);
	type=COLTYPE_RCPPDATE;
    }
    FrameColumn(std::vector<RcppDatetime>& colRcppDatetime_) {
	colRcppDatetime = new std::vector<RcppDatetime>(colRcppDatetime_);
	type=COLTYPE_RCPPDATETIME;
    }
    FrameColumn(Factor& colFactor_) {
	colFactor = new Factor(colFactor_);
	type=COLTYPE_FACTOR;
    }

    ColType getType() { return type; }
    
    int& getInt(int i) { 
	if(type != COLTYPE_INT) lookupError("Int");
	return (*colInt)[i];
    }
    double& getDouble(int i) {
	if(type != COLTYPE_DOUBLE) lookupError("Double");
	return (*colDouble)[i];
    }
    std::string& getString(int i) {
	if(type != COLTYPE_STRING) lookupError("String");
	return (*colString)[i];
    }
    bool getBool(int i) {
	if(type != COLTYPE_LOGICAL) lookupError("Bool");
	return (*colBool)[i];
    }
    FinDate& getFinDate(int i) {
	if(type != COLTYPE_FINDATE) lookupError("FinDate");
	return (*colFinDate)[i];
    }
    RcppDate& getRcppDate(int i) {
	if(type != COLTYPE_RCPPDATE) lookupError("RcppDate");
	return (*colRcppDate)[i];
    }
    RcppDatetime& getRcppDatetime(int i) {
	if(type != COLTYPE_RCPPDATETIME) lookupError("RcppDatetime");
	return (*colRcppDatetime)[i];
    }
    std::string getFactor(int i) { 
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
	case COLTYPE_FINDATE:
	    return colFinDate->size();
	case COLTYPE_RCPPDATE:
	    return colRcppDate->size();
	case COLTYPE_RCPPDATETIME:
	    return colRcppDatetime->size();
	case COLTYPE_NONE:
	    throw std::range_error("Bad COLTYPE in DataFraem.size()");
	}
	return -1;
    }

    void print();

};

/**
 * Models an R data frame as a vector of FrameColumn's, with a corresponding
 * vector of column names and row names consistent with R's representation.
 * When constructed from an R data frame SEXP input Dates are mapped to
 * FinDate by default; this can be changed to RcppDate by setting the
 * flag useRcppDate_. When constructed from native C++ structures both
 * FinDate and RcppDate columns can be included (but I'm not sure why
 * you would want to do this).
 */
class DataFrame {
    std::vector<std::string> rowNames;
    std::vector<std::string> colNames;
    std::vector<FrameColumn> cols;
    static bool useRcppDate_;
public:

    // Use this to map R Dates to RcppDate instead of FinDate.
    static void useRcppDate(bool flag) { useRcppDate_ = flag; }

    DataFrame(SEXP df);
    DataFrame(std::vector<std::string> rowNames_, std::vector<std::string> colNames_,
	       std::vector<FrameColumn> cols_) : rowNames(rowNames_),
					   colNames(colNames_),
					   cols(cols_) {
	// Validate the input data.
	bool badInput = (cols.size() <= 0) || (cols[0].size() <= 0);
	if(!badInput) {
	    if(cols.size() != colNames.size())
		badInput = true;
	    for(int i=0; i < (int)cols.size(); ++i)
		if(cols[i].size() != (int)rowNames.size())
		    badInput = true;
	}
	if(badInput)
	    throw std::range_error("Inconsistent dims in DataFrame constructor");
    }

    DataFrame(std::vector<std::string> rowNames_, std::vector<std::string> colNames_, std::vector<int> colTypes_) : rowNames(rowNames_),
														    colNames(colNames_) {

	// The purpose of this constructor is to eliminate the need to copy
	// all of the user's columns (could be quite long) into the 
	// newly constructed data frame. Instead the columns are allocated
	// by the frame and filled-in later. This works with all column
	// types except COLTYPE_LOGICAL and COLTYPE_FACTOR, so we disallow
	// these column types in this constructor. An easy work-around is
	// to use COLTYPE_INTEGER for columns of the latter two types;
	// these can be coerced to the correct type, with factor levels
	// named as desired, at the R level when this frame is passed to R.

	int nrows = rowNames.size();

	cols.resize((int)colNames.size());

	for(int i=0; i < (int)colTypes_.size(); ++i) {
	    cols[i] = FrameColumn(colTypes_[i], nrows);
	}

	// Validate the input data.
	bool badInput = (cols.size() <= 0) || (cols[0].size() <= 0);
	if(!badInput) {
	    if(cols.size() != colNames.size())
		badInput = true;
	    for(int i=0; i < (int)cols.size(); ++i) {
		if(cols[i].size() != (int)rowNames.size())
		    badInput = true;
	    }
	}
	if(badInput)
	    throw std::range_error("Inconsistent dims in DataFrame constructor");
    }

    operator SEXP();

    void print() { // for debugging
	for(int c = 0; c < (int)cols.size(); ++c) {
	    cols[c].print();
	}
    }
    int size() { return rowNames.size(); }
    int numRows() { return rowNames.size(); }
    int numCols() { return colNames.size(); }
    std::vector<std::string> getRowNames() { return rowNames; }
    std::vector<std::string> getColNames() { return colNames; }
    std::vector<FrameColumn>& getColumns() { return cols; }
    FrameColumn& operator[](std::string colname) {
	for(int i=0; i < (int)colNames.size(); ++i) {
	    if(colNames[i] == colname)
		return cols[i];
	}
	throw std::range_error("Invalid column name in FrameColumn[]");
    }
    FrameColumn& operator[](int i) {
	if(i < 0 || i >= (int)colNames.size())
	    throw std::range_error("Index out of range in FrameColumn[]");
	return cols[i];
    }
};

} // end cxxPack namespace

namespace Rcpp {

    // Work-aruond for the fact that Rcpp::wrap(obj) does not work when
    // obj is an object of a class that has operator SEXP().
    template<> SEXP wrap<cxxPack::DataFrame>(const cxxPack::DataFrame& x);

}

#endif

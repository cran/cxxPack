// DataFrame.cpp: Models and R data frame
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

#include <DataFrame.hpp>

namespace cxxPack {

FrameColumn::FrameColumn(const FrameColumn& col) {
    switch(col.type) {
    case FrameColumn::COLTYPE_INT:
	type = FrameColumn::COLTYPE_INT;
	colInt = new std::vector<int>(*col.colInt);
	break;
    case FrameColumn::COLTYPE_DOUBLE:
	type = COLTYPE_DOUBLE;
	colDouble = new std::vector<double>(*col.colDouble);
	break;
    case FrameColumn::COLTYPE_STRING:
	type = COLTYPE_STRING;
	colString = new std::vector<std::string>(*col.colString);
	break;
    case FrameColumn::COLTYPE_LOGICAL:
	type = COLTYPE_LOGICAL;
	colBool = new std::vector<bool>(*col.colBool);
	break;
    case FrameColumn::COLTYPE_FACTOR:
	type = COLTYPE_FACTOR;
	colFactor = new Factor(*col.colFactor);
	break;
    case FrameColumn::COLTYPE_FINDATE:
	type = COLTYPE_FINDATE;
	colFinDate = new std::vector<FinDate>(*col.colFinDate);
	break;
    case FrameColumn::COLTYPE_RCPPDATE:
	type = COLTYPE_RCPPDATE;
	colRcppDate = new std::vector<RcppDate>(*col.colRcppDate);
	break;
    case FrameColumn::COLTYPE_RCPPDATETIME:
	type = COLTYPE_RCPPDATETIME;
	colRcppDatetime = new std::vector<RcppDatetime>(*col.colRcppDatetime);	
	break;
    default:
	;
    }
}

FrameColumn& FrameColumn::operator=(const FrameColumn& col) {
    if(this == &col)
	return *this;

    switch(col.type) {
    case FrameColumn::COLTYPE_INT:
	type = COLTYPE_INT;
	colInt = new std::vector<int>(*col.colInt);
	break;
    case FrameColumn::COLTYPE_DOUBLE:
	type = COLTYPE_DOUBLE;
	colDouble = new std::vector<double>(*col.colDouble);
	break;
    case FrameColumn::COLTYPE_STRING:
	type = COLTYPE_STRING;
	colString = new std::vector<std::string>(*col.colString);
	break;
    case FrameColumn::COLTYPE_LOGICAL:
	type = COLTYPE_LOGICAL;
	colBool = new std::vector<bool>(*col.colBool);
	break;
    case FrameColumn::COLTYPE_FACTOR:
	type = COLTYPE_FACTOR;
	colFactor = new Factor(*col.colFactor);
	break;
    case FrameColumn::COLTYPE_FINDATE:
	type = COLTYPE_FINDATE;
	colFinDate = new std::vector<FinDate>(*col.colFinDate);
	break;
    case FrameColumn::COLTYPE_RCPPDATE:
	type = COLTYPE_RCPPDATE;
	colRcppDate = new std::vector<RcppDate>(*col.colRcppDate);
	break;
    case FrameColumn::COLTYPE_RCPPDATETIME:
	type = COLTYPE_RCPPDATETIME;
	colRcppDatetime = new std::vector<RcppDatetime>(*col.colRcppDatetime);	
	break;
    default:
	;
    }
    return *this;
}	

FrameColumn::~FrameColumn() {
    switch(type) {
    case FrameColumn::COLTYPE_INT:
	delete colInt;
	break;
    case FrameColumn::COLTYPE_DOUBLE:
	delete colDouble;
	break;
    case FrameColumn::COLTYPE_STRING:
	delete colString;
	break;
    case FrameColumn::COLTYPE_LOGICAL:
	delete colBool;
	break;
    case FrameColumn::COLTYPE_FACTOR:
	delete colFactor;
	break;
    case FrameColumn::COLTYPE_FINDATE:
	delete colFinDate;
	break;
    case FrameColumn::COLTYPE_RCPPDATE:
	delete colRcppDate;
	break;
    case FrameColumn::COLTYPE_RCPPDATETIME:
	delete colRcppDatetime;
	break;
    default:
	;
    }
}

void FrameColumn::print() {
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
    case COLTYPE_FINDATE:
	Rprintf("FINDATE:\n");
	for(int i = 0; i < (int)colFinDate->size(); ++i)
	    Rprintf("  %s\n", to_string((*colFinDate)[i]).c_str());
	break;
    case COLTYPE_RCPPDATE:
	Rprintf("RCPPDATE:\n");
	for(int i = 0; i < (int)colRcppDate->size(); ++i)
	    Rprintf("  %s\n", to_string((*colRcppDate)[i]).c_str());
	break;
    case COLTYPE_RCPPDATETIME:
	Rprintf("RCPPDATETIME:\n");
	for(int i = 0; i < (int)colRcppDatetime->size(); ++i)
	    Rprintf("  %s\n", to_string((*colRcppDatetime)[i]).c_str());
	break;
    case COLTYPE_NONE:
	throw std::range_error("Invalide COLTYPE in FrameColun::print");
    }
}

bool DataFrame::useRcppDate_ = false;

DataFrame::DataFrame(SEXP df) {

    Rcpp::RObject frame(df);

    // Check class.
    Rcpp::CharacterVector classAttr = frame.attr("class");
    if(std::string(classAttr[0]) != "data.frame")
	throw std::range_error("Invalid class in DataFrame constr");

    // Get column names.
    Rcpp::CharacterVector columnNames = frame.attr("names");
    int ncol = columnNames.size();
    colNames.resize(ncol);
    for(int i=0; i < ncol; ++i)
	colNames[i] = columnNames(i);


    // Get list of columns, and number of rows from first one.
    Rcpp::List columnList(frame);
    if(columnList.size() == 0)
	throw std::range_error("columnList has zero size in DataFrame constr");
    int nrow = Rf_length(columnList[0]);

    // Process columns.
    for(int i=0; i < ncol; ++i) {

	// Need cast to SEXP here due to ambiguity.
	Rcpp::RObject colObject((SEXP)columnList[i]);

	// Setup for checking column type.
	bool isDateClass = false, isPOSIXDate = false;

	SEXP classAttr = colObject.attr("class");
	if(classAttr != R_NilValue) { // only dates have this.
	    Rcpp::CharacterVector cv(classAttr);
	    isDateClass = std::string(cv[0]) == "Date";
	    isPOSIXDate = std::string(cv[0]).substr(0,5) == "POSIX";
	}
	
	if(Rf_isReal(colObject)) { // Used for numeric AND date types.
	    if(isDateClass) {
		if(useRcppDate_) { // Use RcppDate instead of FinDate
		    std::vector<RcppDate>* colDate = new std::vector<RcppDate>(nrow);
		    Rcpp::NumericVector nv(colObject);
		    for(int j=0; j < nrow; j++) // FrameColumn of RcppDate's
			(*colDate)[j] = RcppDate(nv(j));
		    cols.push_back(FrameColumn(*colDate));
		}
		else { // Use FinDate
		    std::vector<FinDate>* colDate = new std::vector<FinDate>(nrow);
		    Rcpp::NumericVector nv(colObject);
		    for(int j=0; j < nrow; j++) // FrameColumn of FinDate's
			(*colDate)[j] = FinDate((int)nv(j));
		    cols.push_back(FrameColumn(*colDate));
		}
	    }
	    else if(isPOSIXDate) {
		std::vector<RcppDatetime>* colRcppDatetime = new std::vector<RcppDatetime>(nrow);
		Rcpp::NumericVector nv(colObject);
		for(int j=0; j < nrow; j++) // FrameColumn of RcppDatetime's
		    (*colRcppDatetime)[j] = RcppDatetime(nv(j));
		cols.push_back(FrameColumn(*colRcppDatetime));
	    }
	    else { // FrameColumn of REAL's
		std::vector<double>* colDouble = new std::vector<double>(nrow);
		Rcpp::NumericVector nv(colObject);
		for(int j=0; j < nrow; j++)
		    (*colDouble)[j] = nv(j);
		cols.push_back(FrameColumn(*colDouble));
	    }
	}
	else if(Rf_isInteger(colObject)) {
	    Rcpp::IntegerVector iv(colObject);
	    std::vector<int>* colInt = new std::vector<int>(nrow);
	    for(int j=0; j < nrow; j++)
		(*colInt)[j] = iv(j);
	    cols.push_back(FrameColumn(*colInt));
	}
	else if(Rf_isString(colObject)) { // Non-factor string column
	    std::vector<std::string>* colString = new std::vector<std::string>(nrow);
	    Rcpp::CharacterVector cv(colObject);
	    for(int j=0; j < nrow; j++)
		(*colString)[j] = cv(j);
	    cols.push_back(FrameColumn(*colString));
	}
	else if (Rf_isFactor(colObject)) { // Factor column.
	    SEXP names = Rf_getAttrib(colObject, R_LevelsSymbol);
	    int numLevels = Rf_length(names);
	    std::vector<std::string> levelNames(numLevels);
	    std::vector<std::string> obsLevelNames(nrow);
	    Rcpp::CharacterVector cv(names);
	    for(int k=0; k < numLevels; k++)
		levelNames[k] = cv(k);
	    Rcpp::IntegerVector iv(colObject);
	    for(int j=0; j < nrow; ++j) {
		int obsLevel = iv(j);
		obsLevelNames[j] = levelNames[obsLevel-1];
	    }
	    Factor* colFactor = new Factor(obsLevelNames);
	    cols.push_back(FrameColumn(*colFactor));
	}
	else if(Rf_isLogical(colObject)) {
	    std::vector<bool>* colBool = new std::vector<bool>(nrow);
	    Rcpp::IntegerVector iv(colObject);
	    for(int j=0; j < nrow; j++) {
		(*colBool)[j] = (bool)iv(j);
	    }
	    cols.push_back(FrameColumn(*colBool));
	}
	else
	    throw std::range_error("DataFrame constr unsupported data frame column type.");
    }
    
    // Get row.names attribute and data
    SEXP rownamesAttr = frame.attr("row.names");
    if(rownamesAttr == R_NilValue)
	throw std::range_error("No row.names attribute in DataFrame");

    // Get row.names vector. R sometimes uses numeric names but we
    // convert to strings when this happens.
    rowNames.resize(Rf_length(rownamesAttr));
    if(Rf_isInteger(rownamesAttr)) {
	Rcpp::IntegerVector iv(rownamesAttr);
	for(int i = 0; i < Rf_length(rownamesAttr); ++i)
	    rowNames[i] = to_string(iv(i));
    }
    else if(Rf_isNumeric(rownamesAttr)) {
	Rcpp::NumericVector nv(rownamesAttr);
	for(int i = 0; i < Rf_length(rownamesAttr); ++i)
	    rowNames[i] = to_string(nv(i));
    }
    else if(Rf_isString(rownamesAttr)) {
	rowNames = Rcpp::RObject(rownamesAttr).attributeNames();
    }
}

DataFrame::operator SEXP() {

    int ncol = getColNames().size();
    int nrow = getRowNames().size();

    Rcpp::GenericVector frame(ncol);

    // Set row name vector and attribute.
    Rcpp::CharacterVector rowNames(nrow);
    for(int i=0; i < nrow; ++i)
	rowNames[i] = getRowNames()[i];
    Rcpp::RObject(rowNames).attr("class") = "row.names";

    // Set column names.
    Rcpp::CharacterVector colNames(ncol);
    for(int i=0; i < ncol; ++i)
	colNames[i] = getColNames()[i];

    for(int i=0; i < ncol; i++) {
	cxxPack::FrameColumn col = getColumns()[i];
	switch(col.getType()) {
	case cxxPack::FrameColumn::COLTYPE_DOUBLE: {
	    Rcpp::NumericVector nv(nrow);
	    for(int j=0; j < nrow; j++)
		nv(j) = (*col.colDouble)[j];
	    frame[i] = nv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_INT: {
	    Rcpp::IntegerVector iv(nrow);
	    for(int j=0; j < nrow; j++)
		iv[j] = (*col.colInt)[j];
	    frame[i] = iv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_FACTOR: {
	    // Get levels.
	    int levels = col.colFactor->getNumLevels();
	    Rcpp::CharacterVector cv(levels);
	    for(int k=0; k < levels; ++k)
		cv[k] = col.colFactor->getLevelName(k);
	    // Get observations.
	    Rcpp::IntegerVector iv(nrow);
	    for(int j=0; j < nrow; j++) {
		iv[j] = col.colFactor->getObservedLevelNum(j);
	    }
	    // Set attributes.
	    Rcpp::RObject(iv).attr("levels") = cv;
	    Rcpp::RObject(iv).attr("class") = "factor";
	    frame[i] = iv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_STRING: {
	    Rcpp::CharacterVector cv(nrow);
	    for(int j=0; j < nrow; ++j)
		cv[j] = (*col.colString)[j];
	    frame[i] = cv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_LOGICAL: {
	    Rcpp::LogicalVector lv(nrow);
	    for(int j=0; j < nrow; ++j)
		lv[j] = (*col.colBool)[j];
	    frame[i] = lv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_FINDATE: {
	    Rcpp::NumericVector nv(nrow);
	    for(int j=0; j < nrow; j++)
		nv[j] = (*col.colFinDate)[j].getRValue();
	    Rcpp::RObject(nv).attr("class") = "Date";
	    frame[i] = nv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_RCPPDATE: {
	    Rcpp::NumericVector nv(nrow);
	    for(int j=0; j < nrow; j++)
		nv[j] = (*col.colRcppDate)[j].getJulian();
	    Rcpp::RObject(nv).attr("class") = "Date";
	    frame[i] = nv;
	    }
	    break;
	case cxxPack::FrameColumn::COLTYPE_RCPPDATETIME: {
	    Rcpp::NumericVector nv(nrow);
	    Rcpp::CharacterVector cv(2);
	    cv[0] = Rcpp::datetimeClass[0];
	    cv[1] = Rcpp::datetimeClass[1];
	    Rcpp::RObject(nv).attr("class") = cv;
	    for(int j=0; j < nrow; j++)
		nv[j] = (*col.colRcppDatetime)[j].getFractionalTimestamp();
	    frame[i] = nv;
	    }
	    break;
	default:
	    throw std::range_error("Invalid column type in DataFrame wrap");
	}
    }

    // Set frame attributes.
    frame.attr("class") = "data.frame";
    frame.attr("row.names") = rowNames;
    frame.attr("names") = colNames;

    return frame;
}

} // end cxxPack namespace

namespace Rcpp {

template<> SEXP wrap<cxxPack::DataFrame>(const cxxPack::DataFrame& x) {
    cxxPack::DataFrame *p = const_cast<cxxPack::DataFrame*>(&x);
    return (SEXP)*p;
}

}

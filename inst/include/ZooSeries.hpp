// ZooSeries.hpp: Models R zoo time series.
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

#ifndef ZOOSERIES_HPP
#define ZOOSERIES_HPP

#include <vector>
#include <string>

#include <Rcpp.h>
#include <FinDate.hpp>
#include <ZooSeries.hpp>

namespace cxxPack {

/**
 * Models an R zoo time series object with an ordered but not necessarily
 * equally spaced index. The index can be integer, double, FinDate,
 * RcppDate, or RcppDatetime. The data part can be a vector or a 2D matrix.
 * Permits the user to modify the structure, so it may become unsorted.
 * The operator SEXP() function has been designed to sort on the index 
 * before returning
 * to R, and to flag the series as regular if a frequency has been
 * specified (and the data actually follows this frequency). 
 *
 * Note that frequency = 1/dt, where dt is the fixed time interval, so for
 * a regular series T2-T1 = k x dt, where k is an integer. When a regular
 * zoo series is displayed at the R level the index is usually displayed in
 * terms of the frequency, so the index might appear as follows: 
 * 6(1), 6(2), 6(3), 6(4), 7(1), 7(2), etc. Here 6(2) means t = 6 + (2-1)*dt.
 * When frequency < 1 this convention is not followed. For example, if
 * frequency = .5 a series might appear as: 2(1), 4(1), 6(1), etc., in which
 * case the number in parentheses provides no information.
 */
class ZooSeries {

    // Vectors of ordered types.
    // Only one of the following index vectors will be used per instance,
    // but C/C++ unions cannot be used (even std::string is not allowed).
    std::vector<int> indInt;
    std::vector<double> indDouble;
    std::vector<FinDate> indFinDate;
    std::vector<RcppDate> indRcppDate;
    std::vector<RcppDatetime> indRcppDatetime;

    // Data types supported: only double for now.
    std::vector<double> dataVec;
    std::vector<std::vector<double> > dataMat;
    int indexType_;
    double frequency_;
    bool isMatrix_, isRegular_;
    static bool useRcppDate_;

    int nrows, ncols;

    void lookupError(std::string funcname) {
	throw std::range_error("Invalid call to "+funcname);
    }
public:
    enum IndexType { IND_INTEGER, IND_DOUBLE, IND_FINDATE, 
		     IND_RCPPDATE, IND_RCPPDATETIME };

    // Use this to map from R Date to RcppDate instead of FinDate.
    static void useRcppDate(bool flag) { useRcppDate_ = flag; }

    // Constructor from R object.
    ZooSeries(SEXP zoosexp);

    // Constructors for use on the C++ side.
    ZooSeries(std::vector<double>& data, std::vector<int>& index, 
	      double freq=0);
    ZooSeries(std::vector<double>& data, std::vector<double>& index, 
	    double freq=0);
    ZooSeries(std::vector<double>& data, std::vector<FinDate>& index, 
	    double freq=0);
    ZooSeries(std::vector<double>& data, std::vector<RcppDate>& index, 
	    double freq=0);
    ZooSeries(std::vector<double>& data, std::vector<RcppDatetime>& index, 
	    double freq=0);
    ZooSeries(std::vector<std::vector<double> >& data, std::vector<int>& index, 
	    double freq=0);
    ZooSeries(std::vector<std::vector<double> >& data, std::vector<double>& index, 
	    double freq=0);
    ZooSeries(std::vector<std::vector<double> >& data, std::vector<FinDate>& index,double freq=0);
    ZooSeries(std::vector<std::vector<double> >& data, std::vector<RcppDate>& index,double freq=0);
    ZooSeries(std::vector<std::vector<double> >& data, std::vector<RcppDatetime>& index,double freq=0);

    operator SEXP();

    inline double& vectorDouble(int i) { 
	if(isMatrix_) lookupError("vectorDouble");
	return dataVec[i];
    }
    inline double& matrixDouble(int i, int j) { 
	if(!isMatrix_) lookupError("matrixDouble");
	return dataMat[i][j]; 
    }
    inline int& indexInt(int i) { 
	if(indexType_ != IND_INTEGER) lookupError("indexInt");
	return indInt[i]; 
    }
    inline double& indexDouble(int i) { 
	if(indexType_ != IND_DOUBLE) lookupError("indexDouble");
	return indDouble[i]; 
    }
    inline FinDate& indexFinDate(int i) { 
	if(indexType_ != IND_FINDATE) lookupError("indexFinDate");
	return indFinDate[i]; 
    }
    inline RcppDate& indexRcppDate(int i) { 
	if(indexType_ != IND_RCPPDATE) lookupError("indexRcppDate");
	return indRcppDate[i]; 
    }
    inline RcppDatetime& indexRcppDatetime(int i) { 
	if(indexType_ != IND_RCPPDATETIME) lookupError("indexRcppDatetime");
	return indRcppDatetime[i]; 
    }

    inline double& operator()(int i) { 
	if(isMatrix_)
	    throw std::range_error("ZooSeries: vector op on matrix");
	else if(i < 0 || i >= nrows)
	    throw std::range_error("ZooSeries: index out of range");
	return dataVec[i]; 
    }
    inline double& operator()(int i, int j) { 
	if(!isMatrix_)
	    throw std::range_error("ZooSeries: matrix op on vector");
	else if(i < 0 || j < 0 || i >= nrows || j >=ncols)
 	    throw std::range_error("ZooSeries:: index out of range");
	return dataMat[i][j]; 
    }

    int size() { return nrows; }
    int dataDim()   { return ncols; } // Greater than 1 when isMatrix().
    bool isMatrix() { return isMatrix_; }
    bool isRegular() { return isRegular_; }
    int getIndType() { return indexType_; }

    double getFrequency() { return frequency_; }
    void setFrequency(double freq) { frequency_ = freq; }

    std::vector<int>& getIndInt() { return indInt; }
    std::vector<double>& getIndDouble() { return indDouble; }
    std::vector<FinDate>& getIndFinDate() { return indFinDate; }
    std::vector<RcppDate>& getIndRcppDate() { return indRcppDate; }
    std::vector<RcppDatetime>& getIndRcppDatetime() { return indRcppDatetime; }

    void print();
};

} // end cxxPack namespace

namespace Rcpp {

    // Work-aruond for the fact that Rcpp::wrap(obj) does not work when
    // obj is an object of a class that has operator SEXP().
    template<> SEXP wrap<cxxPack::ZooSeries>(const cxxPack::ZooSeries& x);

}

#endif

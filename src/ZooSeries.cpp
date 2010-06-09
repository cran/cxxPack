// ZooSeries.cpp: models an R zoo time series
//
// Copyright (C) 2010 Dominick Samperi
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

#include <Rcpp.h>
#include <ZooSeries.hpp>

namespace cxxPack {

bool ZooSeries::useRcppDate_ = false;

void ZooSeries::print() { // For debugging purposes.

    Rprintf("ZooSeries frequency suggestion: %lf\n", getFrequency());
    switch(getIndType()) {
    case ZooSeries::IND_INTEGER: {
	if(isMatrix_) {
	    for(int i=0; i < nrows; ++i) {
		Rprintf("%d ", indexInt(i));
		for(int j=0; j < ncols; ++j)
		    Rprintf("%lf ", matrixDouble(i,j));
		Rprintf("\n");
	    }
	}
	else {
	    for(int i=0; i < nrows; ++i)
		Rprintf("%d ", indexInt(i));
	    Rprintf("\n");
	    for(int j=0; j < nrows; ++j)
		Rprintf("%lf ", vectorDouble(j));
	    Rprintf("\n");
	}
	break;
    }
    case ZooSeries::IND_DOUBLE: {
	if(isMatrix_) {
	    for(int i=0; i < nrows; ++i) {
		Rprintf("%lf ", indexDouble(i));
		for(int j=0; j < ncols; ++j)
		    Rprintf("%lf ",matrixDouble(i,j));
		Rprintf("\n");
	    }
	}
	else {
	    for(int i=0; i < nrows; ++i)
		Rprintf("%lf ", indexDouble(i));
	    Rprintf("\n");
	    for(int j=0; j < nrows; ++j)
		Rprintf("%lf ", vectorDouble(j));
	    Rprintf("\n");
	}
	break;
    }
    case ZooSeries::IND_FINDATE: {
	if(isMatrix_) {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexFinDate(i));
		Rprintf("%s ", temp.c_str());
		for(int j=0; j < ncols; ++j) {
		    Rprintf("%lf ", matrixDouble(i,j));
		}
		Rprintf("\n");
	    }
	}
	else {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexFinDate(i));
		Rprintf("%s ", temp.c_str());
	    }
	    Rprintf("\n");
	    for(int j=0; j < nrows; ++j)
		Rprintf("%lf ", vectorDouble(j));
	    Rprintf("\n");
	}
	break;
    }
    case ZooSeries::IND_RCPPDATE: {
	if(isMatrix_) {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexRcppDate(i));
		Rprintf("%s ", temp.c_str());
		for(int j=0; j < ncols; ++j) {
		    Rprintf("%lf ", matrixDouble(i,j));
		}
		Rprintf("\n");
	    }
	}
	else {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexRcppDate(i));
		Rprintf("%s ", temp.c_str());
	    }
	    Rprintf("\n");
	    for(int j=0; j < nrows; ++j)
		Rprintf("%lf ", vectorDouble(j));
	    Rprintf("\n");
	}
	break;
    }
    case ZooSeries::IND_RCPPDATETIME: {
	if(isMatrix_) {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexRcppDatetime(i));
		Rprintf("%s ", temp.c_str());
		for(int j=0; j < ncols; ++j)
		    Rprintf("%lf ", matrixDouble(i,j));
		Rprintf("\n");
	    }
	}
	else {
	    for(int i=0; i < nrows; ++i) {
		std::string temp = to_string(indexRcppDatetime(i));
		Rprintf("%s ", temp.c_str());
	    }
	    Rprintf("\n");
	    for(int j=0; j < nrows; ++j)
		Rprintf("%lf ", vectorDouble(j));
	    Rprintf("\n");
	}
	break;
    }
    }
}

/** 
 * At the R level a zoo time series is always displayed sorted by the
 * index, and this is how it appears on input from R. But this class
 * permits the user to modify the series, including the index, so it
 * may no longer be sorted. Nevertheless, it will be sorted again by
 * the index before it is returned to R (using wrap). If the user has
 * set a non-zero frequency on the C++ side the series will be returned to
 * R as regular, provided it really does follow this frequency. Otherwise,
 * the frequency hint will be ignored.
 */
ZooSeries::ZooSeries(SEXP zoots) {

    Rcpp::RObject zoo(zoots);

    // Check class.
    Rcpp::CharacterVector classAttr = zoo.attr("class");
    isRegular_ = (std::string(classAttr[0]) == "zooreg");
    if(!isRegular_ && !(std::string(classAttr[0]) == "zoo"))
	throw std::range_error("Invalid class in ZooSeries constructor");

    // Get frequency if regular (can be changed by user).
    frequency_ = 0; // frequency = 1/dt with index times a mutiple of dt.
    if(isRegular_)
	frequency_ = Rcpp::as<double>(zoo.attr("frequency"));

    // Check that data type is numeric.
    if(!Rf_isNumeric(zoo))
	throw std::range_error("Non-numeric data type in ZooSeries constructor");
    isMatrix_ = Rf_isMatrix(zoo);

    // Check for date index.
    Rcpp::RObject indexAttr = zoo.attr("index");
    Rcpp::RObject indexClass = Rcpp::RObject(indexAttr).attr("class");
    if(indexClass != R_NilValue) { // applies to date indexes
	Rcpp::CharacterVector ind(indexClass);
	if(std::string(ind[0]) == "Date")
	    indexType_ = useRcppDate_ ? IND_RCPPDATE : IND_FINDATE;
	else if(std::string(ind[0]).substr(0,5) == "POSIX")
	    indexType_ = IND_RCPPDATETIME;
    }

    // Get low-level index type.
    int indexLength = Rf_length(indexAttr);
    if(Rf_isInteger(indexAttr)) {
	indexType_ = IND_INTEGER;
    }
    else {
	if(indexType_ != IND_FINDATE
	   && indexType_ != IND_RCPPDATE
	   && indexType_ != IND_RCPPDATETIME)
	    indexType_ = IND_DOUBLE;
    }

    // Copy index values.
    switch(indexType_) {
    case IND_INTEGER: {
	Rcpp::IntegerVector iv(indexAttr);
	indInt.resize(indexLength);
	for(int i = 0; i < indexLength; ++i)
	    indInt[i] = iv(i);
        }
	break;
    case IND_DOUBLE: {
	Rcpp::NumericVector nv(indexAttr);
	indDouble.resize(indexLength);
	for(int i = 0; i < indexLength; ++i)
	    indDouble[i] = nv(i);
        }
  	break;
    case IND_FINDATE: {
	Rcpp::NumericVector nv(indexAttr);
	indFinDate.resize(indexLength);
	for(int i = 0; i < indexLength; ++i)
	    indFinDate[i] = cxxPack::FinDate(nv(i));
        }
	break;
    case IND_RCPPDATE: {
	Rcpp::NumericVector nv(indexAttr);
	indRcppDate.resize(indexLength);
	for(int i = 0; i < indexLength; ++i)
	    indRcppDate[i] = RcppDate(nv(i));
        }
	break;
    case IND_RCPPDATETIME: {
	Rcpp::NumericVector nv(indexAttr);
	indRcppDatetime.resize(indexLength);
	for(int i = 0; i < indexLength; ++i)
	    indRcppDatetime[i] = RcppDatetime(nv(i));
        }
	break;
    default:
	throw std::range_error("Invalid index type in ZooSeries constr");
    }

    // Copy data (integer data is coerced to double for now).
    if(isMatrix_) {
	Rcpp::NumericMatrix nm(zoo);
	nrows = nm.rows();
	ncols = nm.cols();
	dataMat.resize(nrows);
	for(int i=0; i < nrows; ++i) {
	    std::vector<double> col(ncols);
	    for(int j=0; j < ncols; ++j)
		col[j] = nm(i,j);
	    dataMat[i] = col;
	}
    }
    else { // vector data
	Rcpp::NumericVector vec(zoo);
	nrows = Rf_length(zoo);
	ncols = 1;
	dataVec.resize(nrows);
	for(int i=0; i < nrows; ++i)
	    dataVec[i] = vec(i);
    }		
}

// ZooSeriesValidator is used by the ZooSeries constructors that are intended to
// be used from the C++ side. Checks that the specfied (positive) 
// frequency is consistent with the index vector. Also computes the 
// perturbation vector that can be used to sort the index vector 
// and the rows of the data matrix on output to R. 
template <typename TIndex, typename TData>
class ZooSeriesValidator {
    std::vector<TIndex>& index;
    std::vector<TData>& data;
public:
    // Comparator is used to generate the perturbation necessary to
    // sort the index vector and the data rows.
    template <typename T>
    class Comparator {
	std::vector<T>& vi_;
    public:
	Comparator(std::vector<T>& v) : vi_(v) {}
	bool operator()(int i, int j) {
	    return vi_[i] < vi_[j];
	}
    };
    ZooSeriesValidator(std::vector<TIndex>& index_, std::vector<TData>& data_)
	: index(index_), data(data_) {}
    bool frequencyIsValid(double freq) {
	for(int i=1; i < (int)index.size(); ++i) {
	    double dist = (index[i] - index[i-1])*freq;
	    double frac = dist - (int)dist;
	    if(frac > 1.0e-8)
		return false;
	}
	return true;
    }
    std::vector<int> getSortPermutation() {
	std::vector<int> seq(index.size());
	for(int i=0; i < (int)index.size(); ++i)
	    seq[i] = i;
	std::sort(seq.begin(), seq.end(), Comparator<TIndex>(index));
	return seq;
    }
};

// Constructors for use on the C++ side.
ZooSeries::ZooSeries(std::vector<double>& data, std::vector<int>& index, 
		 double freq) 
    : indInt(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_INTEGER;
    nrows = data.size(); ncols = 1;
}
ZooSeries::ZooSeries(std::vector<double>& data, std::vector<double>& index, 
		 double freq)
    : indDouble(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_DOUBLE; 
    nrows = data.size(); ncols = 1;
}
ZooSeries::ZooSeries(std::vector<double>& data, std::vector<cxxPack::FinDate>& index, 
		 double freq)
    : indFinDate(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_FINDATE; 
    nrows = data.size(); ncols = 1;
    isRegular_ = false;
}
ZooSeries::ZooSeries(std::vector<double>& data, std::vector<RcppDate>& index, 
		 double freq)
    : indRcppDate(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_RCPPDATE; 
    nrows = data.size(); ncols = 1;
    isRegular_ = false;
}
ZooSeries::ZooSeries(std::vector<double>& data, std::vector<RcppDatetime>& index, 
		 double freq)
    : indRcppDatetime(index), dataVec(data), frequency_(freq), isMatrix_(false) { 
    indexType_ = IND_RCPPDATETIME; 
    nrows = data.size(); ncols = 1;
}
ZooSeries::ZooSeries(std::vector<std::vector<double> >& data, std::vector<int>& index, double freq)
    : indInt(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_INTEGER; 
    nrows = index.size(); ncols = data[0].size();
}
ZooSeries::ZooSeries(std::vector<std::vector<double> >& data, std::vector<double>& index, double freq)
    : indDouble(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_DOUBLE; 
    nrows = index.size(); ncols = data[0].size();
}
ZooSeries::ZooSeries(std::vector<std::vector<double> >& data, std::vector<cxxPack::FinDate>& index,double freq)
    : indFinDate(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_FINDATE; 
    nrows = index.size(); ncols = data[0].size();
}
ZooSeries::ZooSeries(std::vector<std::vector<double> >& data, std::vector<RcppDate>& index,double freq)
    : indRcppDate(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_RCPPDATE; 
    nrows = index.size(); ncols = data[0].size();
}
ZooSeries::ZooSeries(std::vector<std::vector<double> >& data, std::vector<RcppDatetime>& index,double freq)
    : indRcppDatetime(index), dataMat(data), frequency_(freq), isMatrix_(true) { 
    indexType_ = IND_RCPPDATETIME; 
    nrows = index.size(); ncols = data[0].size();
}

ZooSeries::operator SEXP() {

	// Possible data types.
	Rcpp::NumericMatrix nm;
	Rcpp::NumericVector nv;

	Rcpp::RObject zoo;
	if(isMatrix()) {
	    nm = Rcpp::NumericMatrix(nrows, ncols);
	    zoo = nm;
	}
	else {
	    nv = Rcpp::NumericVector(nrows);
	    zoo = nv;
	}

	// To begin we use ZooSeriesValidator to get the permutation needed to 
	// sort the data by the index (zoo expects this). We also check
	// if the data is regular and if the requested frequency is
	// consistent with this regularity. If so, we pass a regular
	// (sorted) time series to R.

	std::vector<int> perm;
	bool isRegular = false;
	double freq = getFrequency();

	switch(indexType_) {
	case cxxPack::ZooSeries::IND_INTEGER:
	    if(isMatrix()) {
		cxxPack::ZooSeriesValidator<int,std::vector<double> > 
		    v(indInt, dataMat);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
        }
        else { // vector
	    cxxPack::ZooSeriesValidator<int,double> v(indInt, 
				       dataVec);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	break;
        case cxxPack::ZooSeries::IND_DOUBLE:
	    if(isMatrix()) {
		cxxPack::ZooSeriesValidator<double,std::vector<double> > v(indDouble, 
		    dataMat);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
        }
	else { // vector
	    cxxPack::ZooSeriesValidator<double,double> v(indDouble, 
					  dataVec);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	break;
 case cxxPack::ZooSeries::IND_FINDATE:
	if(isMatrix()) {
	    cxxPack::ZooSeriesValidator<cxxPack::FinDate,std::vector<double> > v(indFinDate, 
		                                         dataMat);
            perm = v.getSortPermutation();
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	else { // vector
	    cxxPack::ZooSeriesValidator<cxxPack::FinDate,double> v(indFinDate, 
					   dataVec);
	    perm = v.getSortPermutation();
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	break;
 case cxxPack::ZooSeries::IND_RCPPDATE:
	if(isMatrix()) {
	    cxxPack::ZooSeriesValidator<RcppDate,std::vector<double> > v(indRcppDate, 
		                                          dataMat);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	else { // vector
	    cxxPack::ZooSeriesValidator<RcppDate,double> v(indRcppDate, 
					    dataVec);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	break;
 case cxxPack::ZooSeries::IND_RCPPDATETIME:
	if(isMatrix()) {
	    cxxPack::ZooSeriesValidator<RcppDatetime,std::vector<double> > 
		v(indRcppDatetime, dataMat);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	else { // vector
	    cxxPack::ZooSeriesValidator<RcppDatetime,double> 
		v(indRcppDatetime, dataVec);
	    perm = v.getSortPermutation();
	    if(freq > 0 && v.frequencyIsValid(freq))
		isRegular = true;
	}
	break;
    }

    // Add the class info at the top level (zoo, zooreg)
    if(isRegular) {
	// zooreg object.
	Rcpp::CharacterVector cv(2);
	cv[0] = "zooreg";
	cv[1] = "zoo";
	zoo.attr("class") = cv;

	// Add frequency attibute.
	zoo.attr("frequency") = Rcpp::wrap(getFrequency());
    }
    else {
	// Plain zoo object (no frequency).
	zoo.attr("class") = "zoo";
    }

    // Add the data part.
    if(isMatrix()) { // matrix Data
	int nx = nrows;
	int ny = ncols;
	for(int i = 0; i < nx; i++)
	    for(int j = 0; j < ny; j++) {
		nm(i,j) = matrixDouble(perm[i],j);
	    }
    }
    else { // vector Data
	int nx = nrows;
	for(int i = 0; i < nx; i++) {
	    nv(i)= vectorDouble(perm[i]);
	}
    }	


    // Add the index part.
    switch(indexType_) {
    case cxxPack::ZooSeries::IND_INTEGER: {
	Rcpp::IntegerVector iv(nrows);
	for(int i = 0; i < nrows; ++i)
	    iv(i) = indexInt(perm[i]);
	zoo.attr("index") = iv;
        }
	break;
    case cxxPack::ZooSeries::IND_DOUBLE: {
	Rcpp::NumericVector nv(nrows);
	for(int i = 0; i < nrows; ++i)
	    nv(i) = indexDouble(perm[i]);
	zoo.attr("index") = nv;
        }
	break;
    case cxxPack::ZooSeries::IND_FINDATE: {
	Rcpp::NumericVector nv(nrows);
	for(int i = 0; i < nrows; ++i)
	    nv(i) = indexFinDate(perm[i]).getRValue();
	zoo.attr("index") = nv;
        }
	break;
    case cxxPack::ZooSeries::IND_RCPPDATE: {
	Rcpp::NumericVector nv(nrows);
	for(int i = 0; i < nrows; ++i)
	    nv(i) = indexRcppDate(perm[i]).getJulian();
	zoo.attr("index") = nv;
        }
	break;
    case cxxPack::ZooSeries::IND_RCPPDATETIME: {
	std::vector<RcppDatetime> indvec = getIndRcppDatetime();
	if(indvec.size() == 0)
	    throw std::range_error("ZooSeries add; zero length vector<DateTime>");
	Rcpp::NumericVector nv(nrows);
	for(int i = 0; i < nrows; ++i)
	    nv(i) = indexRcppDatetime(perm[i]).getFractionalTimestamp();
	zoo.attr("index") = nv;
        } 
	break;
    default:
	throw std::range_error("Invalid index type in ZooSeries wrap");
    }

    // Set index attribute.
    Rcpp::RObject indexObj((SEXP)zoo.attr("index"));
    // Set index attribute class if non-numeric.
    if(!(indexType_ == cxxPack::ZooSeries::IND_INTEGER 
      || indexType_ == cxxPack::ZooSeries::IND_DOUBLE)) {
	if(indexType_ == cxxPack::ZooSeries::IND_FINDATE
	|| indexType_ == cxxPack::ZooSeries::IND_RCPPDATE) {
	    indexObj.attr("class") = Rcpp::CharacterVector("Date");
	}
	else if(indexType_ == cxxPack::ZooSeries::IND_RCPPDATETIME) {
	    Rcpp::CharacterVector cv(2);
	    cv[0] = Rcpp::datetimeClass[0];
	    cv[1] = Rcpp::datetimeClass[1];
	    indexObj.attr("class") = cv;
	}
    }

    return zoo;
    }

} // end cxxPack namespace

namespace Rcpp {

// Work-aruond for the fact that Rcpp::wrap(obj) does not work when
// obj is an object of a class that has operator SEXP().
template<> SEXP wrap<cxxPack::ZooSeries>(const cxxPack::ZooSeries& x) {
    cxxPack::ZooSeries *p = const_cast<cxxPack::ZooSeries*>(&x);
    return (SEXP)*p;
}

}

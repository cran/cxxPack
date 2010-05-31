// Factor.cpp: models and R factor
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

#include <Factor.hpp>

namespace cxxPack {

Factor::Factor(std::vector<std::string> names) {

    // Copy unique sorted list to levelNames.
    levelNames = names;
    std::vector<std::string>::iterator iter = levelNames.end();
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

void Factor::print() const {
    Rprintf("Factor levels:\n");
    for(int i=0; i < (int)levelNames.size(); ++i)
	Rprintf("  %s(%d)\n", levelNames[i].c_str(), i+1);
    Rprintf("Factor Observations:\n");
    for(int i=0; i < (int)observations.size(); ++i) {
	Rprintf("  %d  %s(%d)\n", i, getObservedLevelStr(i).c_str(),
		getObservedLevelNum(i));
    }
}

Factor::Factor(SEXP fac) {
    Rcpp::RObject x = Rcpp::RObject(fac);
    SEXP classAttr = x.attr("class");
    std::string className;
    if(classAttr != R_NilValue)
	className = Rcpp::as<std::string>(classAttr);
    if(className != "factor")
	throw std::range_error("Invalid SEXP in Factor constructor");
    levelNames = Rcpp::as<std::vector<std::string> >(x.attr("levels"));
    int nObs = Rf_length(fac);
    observations.resize(nObs);
    int *ip = INTEGER(fac);
    for(int j=0; j < nObs; ++j)
	observations[j] = ip[j]-1;
}

Factor::operator SEXP() {
    int numObs = getNumObservations();
    int numLevels = getNumLevels();
    Rcpp::IntegerVector iv(numObs); // allocates R memory
    Rcpp::CharacterVector cv(numLevels); // ditto
    SEXP RFactor = iv;
    for(int i=0; i < numObs; ++i)
	iv[i] = getObservedLevelNum(i);
    for(int i=0; i < numLevels; ++i)
	cv[i] = getLevelName(i);
    Rcpp::RObject ro(RFactor);
    ro.attr("class") = "factor";
    ro.attr("levels") = cv;
    return ro;
}

} // end cxxPack namespace

namespace Rcpp {

// Work-aruond for the fact that Rcpp::wrap(obj) does not work when
// obj is an object of a class that has operator SEXP().
template<> SEXP wrap<cxxPack::Factor>(const cxxPack::Factor& x) {
    cxxPack::Factor *p = const_cast<cxxPack::Factor*>(&x);
    return (SEXP)*p;
}

}

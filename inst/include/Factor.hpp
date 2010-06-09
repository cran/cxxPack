// Factor.hpp: models an R factor
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

#ifndef FACTOR_HPP
#define FACTOR_HPP

#include <vector>
#include <string>

#include <Rcpp.h>

namespace cxxPack {

/**
 * Models an R factor, that is, a vector of observations where the possible
 * outcomes take values in a finite set of string values (called levelNames
 * here).
 */
class Factor {
    // This is the unique list of level names (sorted) for an R factor.
    // The numeric value that R uses for levelNames[i] is i+1.
    std::vector<int> observations;  // observations are pointers into levelNames
public:
    std::vector<std::string> levelNames; // names indexed by observations.
    
    Factor(SEXP fac); // from R
    Factor(std::vector<std::string> names); // from C++

    operator SEXP();

    std::string operator[](int i) { return getObservedLevelStr(i); }
    int operator()(int i) { return getObservedLevelNum(i); }
    int size() { return observations.size(); }

    /**
     * Returns the index into the vector of level names corresponding to
     * the i-th observation. This vector is zero-based, so this is
     * one less than what R calls the level number for this
     * observation.
     */
    int getObservedLevelIndex(int i) const {
	if(i >= 0 && i < (int)observations.size())
	    return observations[i];
	else
	    throw std::range_error("Factor index out of range");
    }

    /**
     * Returns what R calls the level value for the i-th observation.
     */
    int getObservedLevelNum(int i) const { return getObservedLevelIndex(i)+1; }

    /**
     * Returns the string value of the i-th observation.
     */
    std::string getObservedLevelStr(int i) const { 
	return levelNames[getObservedLevelIndex(i)]; 
    }

    /**
     * Returns the j-th level name from the sorted vector of 
     * unique level names.
     */
    std::string getLevelName(int j) const { 
	if(j >= 0 && j < (int)levelNames.size())
	    return levelNames[j];
	else
	    throw std::range_error("Factor level number out of range");
    }

    int getNumObservations() const { return observations.size(); }
    int getNumLevels() const { return levelNames.size(); }
    void print() const; // useful for debugging.
};

} // end cxxPack namespace

namespace Rcpp {

    // Work-aruond for the fact that Rcpp::wrap(obj) does not work when
    // obj is an object of a class that has operator SEXP().
    template<> SEXP wrap<cxxPack::Factor>(const cxxPack::Factor& x);

}

#endif

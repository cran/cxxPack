#include <cxxPack.hpp>
/**
 * Construct a Factor from input object and from native data structures.
 */
RcppExport SEXP testFactor(SEXP factorin_) {
    BEGIN_RCPP
    cxxPack::Factor factorin(factorin_); // From R factor

    int nobs = 8;
    std::vector<std::string> obs(nobs);
    for(int i=0; i < nobs; ++i)
	obs[i] = "Level"+cxxPack::to_string((i+1)%3+1);
    cxxPack::Factor fac(obs); // Native constructor.

    Rcpp::List rl;
    rl["factorin"] = Rcpp::wrap(factorin);
    rl["fac"] = Rcpp::wrap(fac);
    return rl;
    END_RCPP
}

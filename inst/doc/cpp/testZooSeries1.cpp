#include <cxxPack.hpp>
/**
 * Test ZooSeries with scalar observations.
 */
RcppExport SEXP testZooSeries1(SEXP zooin_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP

    cxxPack::ZooSeries zooin(zooin_);

    int n = 3; // number of dates, one scalar observation per date.
    std::vector<cxxPack::FinDate> obsdates(n); // the index.
    std::vector<double>  obs(n); // the observations.

    for(int i=0; i < n; ++i) {
	obsdates[i] = cxxPack::FinDate(cxxPack::Month(4),15,2010) + i;
	obs[i] = 100.5 + i;
    }
    cxxPack::ZooSeries zoo(obs, obsdates);

    Rcpp::List rl;
    rl["zooin"] = Rcpp::wrap(zooin);
    rl["zoo"] = Rcpp::wrap(zoo);
    ret = rl;
    END_RCPP
    return ret;
}

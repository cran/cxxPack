#include <cxxPack.hpp>
/**
 * Test ZooSeries with vector observations.
 */
RcppExport SEXP testZooSeries2() {

    SEXP ret = R_NilValue;
    BEGIN_RCPP

    // Three observations per date.

    int n = 5; // number of dates.
    int m = 3; // number of observations per date.

    std::vector<cxxPack::FinDate> obsdates(n);
    std::vector<std::vector<double> > obs(n);

    int count = 0;
    for(int i=0; i < n; ++i) {
	obsdates[i] = cxxPack::FinDate(cxxPack::Month(4),15,2010) + i;
	std::vector<double> v(m);
	for(int j=0; j < m; ++j)
	    v[j] = count++;
	obs[i] = v;
    }

    cxxPack::ZooSeries zoo(obs, obsdates);

    zoo.setFrequency(1);

    ret = zoo;
    END_RCPP
    return ret;
}

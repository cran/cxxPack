#include <cxxPack.hpp>
RcppExport SEXP testHighFreqSeries(SEXP start_, SEXP end_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    RcppDatetime start(start_);
    RcppDatetime end(end_);
    std::vector<RcppDatetime> index;
    std::vector<double> obs;
    GetRNGstate(); // initialize R's random number generator.
    RcppDatetime datetime = start;
    int dt = 60*10; // 10 minute intervals
    while(datetime < end) {
	index.push_back(datetime);
	obs.push_back(norm_rand()); // standard normal
	datetime = datetime + dt;
    }
    PutRNGstate(); // finished with random number generator
    cxxPack::ZooSeries zoo(obs, index); // ordered but not regular
    cxxPack::ZooSeries zooreg(obs, index, 1.0/dt); // regular (liks ts)
    Rcpp::List rl;
    rl["zoo"] = Rcpp::wrap(zoo);
    rl["zooreg"] = Rcpp::wrap(zooreg);
    ret = rl;
    END_RCPP
    return ret;
}

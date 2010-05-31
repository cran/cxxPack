#include <cxxPack.hpp>
/**
 * Test C++ interface to R's root finder.
 */
RcppExport SEXP testRootFinder(SEXP x) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    double ysqr = Rcpp::as<double>(x);
    class PriceFunction : public cxxPack::Function1D {
	double ysqr;
    public:
	PriceFunction(double ysqr_) : cxxPack::Function1D(), ysqr(ysqr_) {}
	double value(double y) { return y*y - ysqr; }
    };
    cxxPack::RootFinder1D rootFinder;
    PriceFunction pr(ysqr);
    double root = rootFinder.solve(pr, 0, 100, 0.00001);
    ret = Rcpp::wrap(root);
    END_RCPP
    return ret;
}

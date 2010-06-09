#include <cxxPack.hpp>
/**
 * NumericVector copy semantics.
 */
RcppExport SEXP testNumericVector(SEXP x) {
    BEGIN_RCPP
    Rcpp::NumericVector nv(x);
    Rcpp::NumericVector av = nv;
    Rcpp::NumericVector cv = Rcpp::clone(Rcpp::NumericVector(x));
    nv(0) = 5; av(1) = 6; cv(2) = 7;
    Rcpp::List rl;
    rl["nv"] = Rcpp::wrap(nv);
    rl["av"] = Rcpp::wrap(av);
    rl["cv"] = Rcpp::wrap(cv);
    return rl;
    END_RCPP
}

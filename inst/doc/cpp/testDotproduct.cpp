#include <cxxPack.hpp>
RcppExport SEXP testDotproduct(SEXP x, SEXP y) {
    BEGIN_RCPP
    Rcpp::NumericVector nv1(x), nv2(y);
    double sum=0;
    for(int i=0; i < nv1.size(); ++i)
        sum += nv1(i)*nv2(i);
    return Rcpp::wrap(sum);
    END_RCPP
}

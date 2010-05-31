#include <cxxPack.hpp>

RcppExport SEXP My_Test(SEXP x_, SEXP df_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP

    double x = Rcpp::as<double>(x_);

    // Construct DataFrame object in two ways.
    // Both depend on DataFrame constructor:
    cxxPack::DataFrame df1 = Rcpp::as<cxxPack::DataFrame>(df_);
    cxxPack::DataFrame df2(df_);

    // Build return list.
    Rcpp::List rl;
    rl["x"] = Rcpp::wrap(x);
    rl["sqrtx"] = Rcpp::wrap(sqrt(x));
    rl["df1"] = Rcpp::wrap(df1);
    rl["df2"] = Rcpp::wrap(df2);
    ret = rl;
    END_RCPP
    return ret;
}

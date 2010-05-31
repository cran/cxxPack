#include <cxxPack.hpp>
/**
 * Safer hello world.
 */
RcppExport SEXP testSaferHello() {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
	ret = Rcpp::wrap("hello world");
        if(ret != R_NilValue) // logical error
	    throw std::range_error("SaferHello: wrap failed");
    END_RCPP
    return ret;
}

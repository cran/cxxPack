#include <cxxPack.hpp>
/**
 * Safer hello world.
 */
RcppExport SEXP testSaferHello() {
    BEGIN_RCPP
	SEXP ret = Rcpp::wrap("hello world");
        if(ret != R_NilValue) // logical error
	    throw std::range_error("SaferHello: wrap failed");
	return ret;
    END_RCPP
}

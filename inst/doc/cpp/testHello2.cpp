#include <cxxPack.hpp>
RcppExport SEXP testHello2() {
    SEXP ret = R_NilValue;
    try {
	ret = Rcpp::wrap("hello world");
    } catch(std::exception& ex) {
	Rf_error(ex.what());
    } catch(...) {
	Rf_error("Unknown exception");
    }
    return ret;
}
	

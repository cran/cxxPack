#include <cxxPack.hpp>
RcppExport SEXP testHello() {
    return Rcpp::wrap("hello world");
}

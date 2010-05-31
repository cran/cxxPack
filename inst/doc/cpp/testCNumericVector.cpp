#include <cxxPack.hpp>
/**
 * Test experimental CNumericVector, CNumericMatrix, CDateVector, etc.
 */
RcppExport SEXP testCNumericVector(SEXP vec_, SEXP mat_, SEXP dvec_, SEXP dtvec_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
	cxxPack::CNumericVector cv1(vec_);
        cxxPack::CNumericMatrix cm(mat_);
	cxxPack::CDateVector dvec(dvec_);
	cxxPack::CDatetimeVector dtvec(dtvec_);
	cxxPack::CNumericVector cv2 = cv1; // uses STL copy constructors
	cv1(0) = 98;
	cm(1,2) = 99;
	dvec(0) = cxxPack::FinDate(cxxPack::Month(4), 15, 2010);
	dtvec(0) = RcppDatetime(14714.25*60*60*24); // 4/15/2010, 6AM GMT.

	Rcpp::List rl;
	rl["cv1"] = Rcpp::wrap(cv1);
	rl["cv2"] = Rcpp::wrap(cv2);
	rl["cm"] = Rcpp::wrap(cm);
	rl["dvec"] = Rcpp::wrap(dvec);
	rl["dtvec"] = Rcpp::wrap(dtvec);
	ret = rl;
    END_RCPP
    return ret;
}

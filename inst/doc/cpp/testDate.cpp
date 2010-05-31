#include <cxxPack.hpp>
/**
 * Exercises the financial date library.
 */
RcppExport SEXP testDate(SEXP d1_, SEXP d2_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    cxxPack::FinDate d1(d1_), d2(d2_);
    cxxPack::FinDate d3(cxxPack::Month(2), 28, d1.getYear());
    int diff30360 = cxxPack::FinDate::diffDays(d1, d2, 
						  cxxPack::FinEnum::DC30360I);
    int diffACT = d2 - d1;
    cxxPack::FinDate nthFriday = d1.nthWeekday(3, cxxPack::Fri);
    double excelnum = cxxPack::serialNumber(d1, cxxPack::Excel1900);
    Rcpp::List rl;
    rl["d3"] = Rcpp::wrap(d3);
    rl["diff30360"] = Rcpp::wrap(diff30360);
    rl["diffACT"] = Rcpp::wrap(diffACT);
    rl["excelnum"] = Rcpp::wrap(excelnum);
    rl["nthFriday"] = Rcpp::wrap(nthFriday);
    ret = rl;
    END_RCPP
    return ret;
}

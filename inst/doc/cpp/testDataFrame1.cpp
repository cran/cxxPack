#include <cxxPack.hpp>
/**
 * DataFrame demo without constructing columns separately.
 */
RcppExport SEXP testDataFrame1(SEXP dfin_) {
    BEGIN_RCPP
    cxxPack::DataFrame dfin(dfin_);
    int ncols = 3;
    int nrows = 2;
    std::vector<std::string> colNames(ncols);
    std::vector<std::string> rowNames(nrows);
    std::vector<int> colTypes(ncols);
    colNames[0] = "id"; colTypes[0] = cxxPack::FrameColumn::COLTYPE_INT;
    colNames[1] = "amount"; colTypes[1] = cxxPack::FrameColumn::COLTYPE_DOUBLE;
    colNames[2] = "date"; colTypes[2] = cxxPack::FrameColumn::COLTYPE_FINDATE;
    rowNames[0] = "r1"; rowNames[1] = "r2";
    cxxPack::DataFrame df(rowNames, colNames, colTypes);

    // Fill in data (can also use df[0].getInt(i), etc.)
    for(int i=0; i < nrows; ++i) {
	df["id"].getInt(i) = i+100;
	df["amount"].getDouble(i) = i+100.5;
	df["date"].getFinDate(i) = cxxPack::FinDate(cxxPack::Month(4),15,2010)+i;
    }

    Rcpp::List rl;
    rl["df"] = Rcpp::wrap(df);
    rl["dfin"] = Rcpp::wrap(dfin);
    return rl;
    END_RCPP
}

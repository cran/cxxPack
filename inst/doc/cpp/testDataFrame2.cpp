#include <cxxPack.hpp>
/**
 * DataFrame demo with separate construction of each column.
 */
RcppExport SEXP testDataFrame2() {
    SEXP ret = R_NilValue;
    BEGIN_RCPP

    int ncols = 8; // use all possible column types.
    int nrows = 3;

    std::vector<std::string> colNames(ncols);
    std::vector<std::string> rowNames(nrows);
    
    std::vector<int> colInt(nrows);
    std::vector<double> colDouble(nrows);
    std::vector<std::string> colString(nrows);
    std::vector<std::string> factorobs(nrows);
    std::vector<bool> colBool(nrows);
    std::vector<cxxPack::FinDate> colFinDate(nrows);
    std::vector<RcppDate> colRcppDate(nrows);
    std::vector<RcppDatetime> colRcppDatetime(nrows);

    colNames[0] = "int";
    colNames[1] = "dbl";
    colNames[2] = "str";
    colNames[3] = "fac";
    colNames[4] = "bool";
    colNames[5] = "findate";
    colNames[6] = "rcppdate";
    colNames[7] = "rcppdatetime";

    RcppDatetime dt0(14714.25*60*60*24); // 4/15/2010, 6AM GMT.
    for(int i=0; i < nrows; ++i) {
	rowNames[i] = cxxPack::to_string(i+1);
	colInt[i] = i+1;
	colDouble[i] = i+1.5;
	colString[i] = "test"+cxxPack::to_string(i+1);
	colBool[i] = i%2 == 0;
	colFinDate[i] = cxxPack::FinDate(cxxPack::Month(4),15,2010) + i;
	colRcppDate[i] = RcppDate(cxxPack::Month(4),15,2010) + i;
	colRcppDatetime[i] = dt0+(.25+i)*60*60*24;
	factorobs[i] = "a"+cxxPack::to_string(i+1);
    }
    cxxPack::Factor factor(factorobs);

    std::vector<cxxPack::FrameColumn> cols(0);
    cols.push_back(cxxPack::FrameColumn(colInt));
    cols.push_back(cxxPack::FrameColumn(colDouble));
    cols.push_back(cxxPack::FrameColumn(colString));
    cols.push_back(cxxPack::FrameColumn(factor));
    cols.push_back(cxxPack::FrameColumn(colBool));
    cols.push_back(cxxPack::FrameColumn(colFinDate));
    cols.push_back(cxxPack::FrameColumn(colRcppDate));
    cols.push_back(cxxPack::FrameColumn(colRcppDatetime));

    cxxPack::DataFrame df(rowNames, colNames, cols);
    ret = df;
    END_RCPP
    return ret;
}

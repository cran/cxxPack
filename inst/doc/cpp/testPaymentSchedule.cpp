#include <cxxPack.hpp>
RcppExport SEXP testPaymentSchedule(SEXP params_) {
    BEGIN_RCPP

    // Fetch params.
    Rcpp::List params(params_);
    cxxPack::FinDate start(Rcpp::as<cxxPack::FinDate>(params["start"]));
    cxxPack::FinDate end = Rcpp::as<cxxPack::FinDate>(params["end"]);
    int nth = Rcpp::as<int>(params["nth"]);
    int weekday = Rcpp::as<int>(params["weekday"]);
    double coupon = Rcpp::as<double>(params["coupon"]);

    int nextMonth, nextYear;
    cxxPack::FinDate date = start.nthWeekday(nth, weekday);
    cxxPack::FinDate lastDate = date;
    std::vector<cxxPack::FinDate> dateVec;
    
    // Get schedule
    while(date <= end) {
	if(date >= start) // could have nthWeekday < start in first month.
	    dateVec.push_back(date);
	if(date.getMonth() == cxxPack::Dec) {
	    nextMonth = 1;
	    nextYear = date.getYear()+1;
	}
	else {
	    nextMonth = date.getMonth()+1;
	    nextYear = date.getYear();
	}
	date = cxxPack::FinDate(cxxPack::Month(nextMonth),1,nextYear);
	date = date.nthWeekday(nth, weekday);
    }

    // Computer payments and insert in data frame.
    std::vector<std::string> colNames(4);
    colNames[0] = "Date";
    colNames[1] = "Days";
    colNames[2] = "Pmt";
    colNames[3] = "Priority"; // High, Low, Med (factor column)
    int nrow = dateVec.size();
    std::vector<std::string> rowNames(nrow);
    std::vector<int> colDays(nrow);
    std::vector<double> colPmt(nrow);
    std::vector<std::string> priority(nrow); // factor observation
    rowNames[0] = "1";
    colDays[0] = 0; colPmt[0] = 0; priority[0] = "Low";
    for(int i=1; i < nrow; ++i) {
	rowNames[i] = cxxPack::to_string(i+1);
	colDays[i] = cxxPack::FinDate::diffDays(dateVec[i-1],dateVec[i],
						cxxPack::FinEnum::DC30360I);
	colPmt[i] = 100*coupon*colDays[i]/360.0;
	priority[i] = (i%2 == 0) ? "Med" : "High"; // arbitrary
    }

    cxxPack::Factor factor(priority);
    std::vector<cxxPack::FrameColumn> cols(0);
    cols.push_back(cxxPack::FrameColumn(dateVec));
    cols.push_back(cxxPack::FrameColumn(colDays));
    cols.push_back(cxxPack::FrameColumn(colPmt));
    cols.push_back(cxxPack::FrameColumn(factor));
    cxxPack::DataFrame df(rowNames, colNames, cols);

    return df;
    END_RCPP
}

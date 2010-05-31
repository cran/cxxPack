// FinEnum.cpp: misc finance-related enumerated types
//
// Copyright (C) 2005-2010 Dominick Samperi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 3 as 
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <string>
#include <vector>
#include <stdexcept>

#include "FinEnum.hpp"

namespace cxxPack {

std::string FinEnum::BondFeatureStr[] = { "None", "Maturity Anchor", 
    "Odd First Coupon", "Odd Last Coupon", "Odd First and Last", 
    "Moosmuller Conv.", "Braess-Fangmeyer Conv."
};
int FinEnum::numBondFeature = sizeof(BondFeatureStr)/sizeof(std::string);
    
std::string FinEnum::DayCountConventionStr[] = {
    "ACT/ACT", "ACT/360", "ACT/365", 
    "ACT/252","30/360 ISDA", "30/360 Euro",
    "30/360 PSA1", "30/360 PSA2", 
    "ACT/365 No Leap",
    "ACT/360 No Leap"
};
int FinEnum::numDayCountConvention = sizeof(DayCountConventionStr)/sizeof(std::string);

std::string FinEnum::MonthEndAdjustmentStr[] = { "None", "Adjust if EOM" };
int FinEnum::numMonthEndAdjustment = sizeof(MonthEndAdjustmentStr)/sizeof(std::string);

std::string FinEnum::AccrualConventionStr[] = { "Standard",
						"Year Fraction",
						"Year Fraction Plus 1" };
int FinEnum::numAccrualConvention = sizeof(AccrualConventionStr)/sizeof(std::string);

std::string FinEnum::CashFlowTypeStr[] = { "FX", "SETTLE", "COUPON", 
					   "PRINCIPAL", "OTHER" };
int FinEnum::numCashFlowType = sizeof(CashFlowTypeStr)/sizeof(std::string);

std::string FinEnum::CalcModeStr[] = { "Fixed Price", "Fixed Yield" };
int FinEnum::numCalcMode = sizeof(CalcModeStr)/sizeof(std::string);


std::string FinEnum::BondFeature_str(BondFeature t) {
    return BondFeatureStr[t];
}
FinEnum::BondFeature FinEnum::BondFeature_for(std::string s) {
    for(int i = 0; i < FinEnum::numBondFeature; ++i)
	if(s == FinEnum::BondFeatureStr[i])
	    return (FinEnum::BondFeature)i;
    throw std::range_error("Invalid string in BondFeature_for");
}

std::string FinEnum::DayCountConvention_str(DayCountConvention t) {
    return DayCountConventionStr[t];
}
FinEnum::DayCountConvention FinEnum::DayCountConvention_for(std::string s) {
    for(int i = 0; i < FinEnum::numDayCountConvention; ++i)
	if(s == FinEnum::DayCountConventionStr[i])
	    return (FinEnum::DayCountConvention)i;
    throw std::range_error("Invalid string in DayCountConvention_for");
}

std::string FinEnum::MonthEndAdjustment_str(MonthEndAdjustment t) {
    return MonthEndAdjustmentStr[t];
}
FinEnum::MonthEndAdjustment FinEnum::MonthEndAdjustment_for(std::string s) {
    for(int i = 0; i < FinEnum::numMonthEndAdjustment; ++i)
	if(s == FinEnum::MonthEndAdjustmentStr[i])
	    return (FinEnum::MonthEndAdjustment)i;
    throw std::range_error("Invalid string in MonthEndAdjustment_for");
}

std::string FinEnum::AccrualConvention_str(AccrualConvention t) {
    return AccrualConventionStr[t];
}
FinEnum::AccrualConvention FinEnum::AccrualConvention_for(std::string s) {
    for(int i = 0; i < FinEnum::numAccrualConvention; ++i)
	if(s == FinEnum::AccrualConventionStr[i])
	    return (FinEnum::AccrualConvention)i;
    throw std::range_error("Invalid string in AccrualConvention_for");
}

std::string FinEnum::CashFlowType_str(CashFlowType t) {
    return CashFlowTypeStr[t];
}
FinEnum::CashFlowType FinEnum::CashFlowType_for(std::string s) {
    for(int i = 0; i < FinEnum::numCashFlowType; ++i)
	if(s == FinEnum::CashFlowTypeStr[i])
	    return (FinEnum::CashFlowType)i;
    throw std::range_error("Invalid string in CashFlowType_for");
}

std::string FinEnum::CalcMode_str(CalcMode t) {
    return CalcModeStr[t];
}
FinEnum::CalcMode FinEnum::CalcMode_for(std::string s) {
    for(int i = 0; i < FinEnum::numCalcMode; ++i)
	if(s == FinEnum::CalcModeStr[i])
	    return (FinEnum::CalcMode)i;
    throw std::range_error("Invalid string in CalcMode_for");
}

} // end cxxPack namespace


// FinEnum.hpp: misc finance enums
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

#ifndef FinEnum_Include
#define FinEnum_Include

#include <string>

namespace cxxPack {

// This class was designed before there was a cxxPack namespace and
// is not really needed. These enums should probably be moved up a level.
class FinEnum {
public:

    enum BondFeature {
	NoFeature, MaturityAnchor, OddFirst, OddLast, OddBoth, 
	Moosmuller, BraessFangmeyer
    };
    enum DayCountConvention {
	DCAA, DCA360, DCA365, DCA252,
	DC30360I, DC30360E, DC30360P1, DC30360P2, DCA360NL, DCA365NL
    };
    enum MonthEndAdjustment { NoAdj, AdjustIfEOM };
    enum AccrualConvention { Standard, YearFraction, YearFractionPlus1 };
    enum CashFlowType {  FX, SETTLE, COUPON, PRINCIPAL, OTHER };
    enum CalcMode { FixedPrice, FixedYield };

    FinEnum() {}

    static std::string BondFeature_str(BondFeature t);
    static BondFeature BondFeature_for(std::string s);
    static std::string BondFeatureStr[];
    static int numBondFeature;

    static std::string DayCountConvention_str(DayCountConvention t);
    static DayCountConvention DayCountConvention_for(std::string s);
    static std::string DayCountConventionStr[];
    static int numDayCountConvention;

    static std::string MonthEndAdjustment_str(MonthEndAdjustment t);
    static MonthEndAdjustment MonthEndAdjustment_for(std::string s);
    static std::string MonthEndAdjustmentStr[];
    static int numMonthEndAdjustment;

    static std::string AccrualConvention_str(AccrualConvention t);
    static AccrualConvention AccrualConvention_for(std::string s);
    static std::string AccrualConventionStr[];
    static int numAccrualConvention;

    static std::string CashFlowType_str(CashFlowType t);
    static CashFlowType CashFlowType_for(std::string s);
    static std::string CashFlowTypeStr[];
    static int numCashFlowType;

    static std::string CalcMode_str(CalcMode t);
    static CalcMode CalcMode_for(std::string s);
    static std::string CalcModeStr[];
    static int numCalcMode;
};

} // end cxxPack namespace

#endif

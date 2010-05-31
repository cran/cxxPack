// FinDate.hpp: financial date library
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

#ifndef FINDATE_HPP
#define FINDATE_HPP

#include <Rcpp.h>

#include <FinEnum.hpp>

#include <cxxUtils.hpp>

namespace cxxPack {

    enum Weekday { Sun=0, Mon=1, Tue=2, Wed=3, Thu=4, Fri=5, Sat=6 };
    enum Month { Jan=1, Feb=2, Mar=3, Apr=4, May=5, Jun=6,
		 Jul=7, Aug=8, Sep=9, Oct=10, Nov=11, Dec=12 };
    enum SerialType { None=0, R=1, Excel1900=2, Excel1904=3, QuantLib=4,
		      IsdaCds=5, Julian=6 };

// Return value from jul2mdy()
class DateMDY {
public:
    int month, day, year;
    DateMDY(int month_, int day_, int year_) 
	: month(month_), day(day_), year(year_) {}
};

/**
 * Date class that models R's Date class with a number of enhancements
 * designed to faciliate work in finance. The underlying real representation
 * holds the number of seconds since 1/1/1970 with 1-day resolution. For 
 * finer resolution use RcppDatetime.
 */
class FinDate {

protected:

    // These are safe non-public methods so we take some liberties here.
    void mdy2jdn(), jdn2mdy() const;
    int jdn; // Julian day number
    mutable int month, day, year;
    bool parsed;

    static const int daysInMonthTable[2][13]; // 0=no-leap, 1=leap; 1<=month<=12.
public:

    static const int DAYS2SECS, R_Offset, 
	             Excel1900_Offset, Excel1904_Offset, IsdaCds_Offset;
    static int mdy2jdn(int month, int day, int year); // julian from mdy
    static DateMDY jdn2mdy(int jdn);
    static const std::string weekdayName[];
    static const std::string monthName[];
    static const int serialOffsets[];

    /**
     * Returns the value that R uses to represent this date type.
     */
    double getRValue() const {
	return jdn - R_Offset;
    }

    /**
     * SEXP based constructor.
     */
    FinDate(SEXP);

    /**
     * Construct from an R Date serial number or a julian day number (JDN).
     */
    FinDate(int serialNum, bool isJDN = false) {
	parsed = false;
	if(isJDN) {
	    jdn = serialNum;
	}
	else {
	    jdn = serialNum + R_Offset;
	}
    }

    /**
     * Prevent m/d/y and d/m/y confusion by requiring that the first
     * parameter to this constructor be of type cxxPack::Month.
     */
    FinDate(Month month, int day, int year) {
	parsed = true;
	this->day = day;
	this->month = month;
	this->year = year;
	if(month < 1 || month > 12 || day < 1 || day > 31)
	    throw std::range_error("FinDate: invalid date");
	jdn = mdy2jdn(month, day, year);
    }

    FinDate() {
	parsed = false;
	jdn = mdy2jdn(1,1,1970);
    }

    operator SEXP();

    int getMonth() const { if(!parsed) jdn2mdy(); return month; }
    int getDay()   const { if(!parsed) jdn2mdy(); return day; }
    int getYear()  const { if(!parsed) jdn2mdy(); return year; }
    std::string getWeekdayName() const;
    std::string getMonthName() const;

    static double divisor(FinEnum::DayCountConvention dc) {
	switch(dc) {
	case FinEnum::DCA360:
	case FinEnum::DC30360I:
	case FinEnum::DC30360E:
	case FinEnum::DC30360P1:
	case FinEnum::DC30360P2:
	case FinEnum::DCA360NL:
	    return 360.0;
	case FinEnum::DCA365:
	case FinEnum::DCA365NL:
	    return 365.0;
	case FinEnum::DCA252:
	    return 252.0;
	default:
	    return 1.0;
	}
    }

    friend bool     operator<(const FinDate &date1, const FinDate& date2);
    friend bool     operator>(const FinDate &date1, const FinDate& date2);
    friend bool     operator==(const FinDate &date1, const FinDate& date2);
    friend bool     operator!=(const FinDate &date1, const FinDate& date2);
    friend bool     operator>=(const FinDate &date1, const FinDate& date2);
    friend bool     operator<=(const FinDate &date1, const FinDate& date2);

    friend double   operator-(const FinDate& date1, const FinDate& date2);
    friend FinDate operator+(const FinDate &date, int offsetDays);
    FinDate& operator+=(int offsetDays);
    FinDate& operator-=(int offsetDays);
    FinDate& operator++();    // increment 1 day (prefix op)
    FinDate operator++(int);  // increment 1 day (postfix op)
    FinDate& operator--();    // decrement 1 day (prefix op)
    FinDate operator--(int);  // decrement 1 day (postfix op)

    /**
     * Get day of week for this date: FinDate::Mon, FinDate::Tue, etc.
     */
    int getWeekday() const;

    /**
     * Get the next weekday after today: dayNum == FinDate::Wed means
     * get the next Wednesday, not including the current day if it
     * happens to be a Wednesday.
     */
    FinDate nextWeekday(int dayNum) const;

    /**
     * Get the n-th occurence of the specified weekday in the
     * month and year determined by this date, where the first of
     * the month is counted if it happens to be the specified
     * weekday. Weekdays are FinDate::Mon, FinDate::Tue, etc.
     */
    FinDate nthWeekday(int n, int dayNum) const;

    /**
     * Is this date in a leap year?
     */
    bool isLeapYear() const;

    /**
     * Is the specified year a leap year?
     */
    static bool isLeapYear(int year);

    /**
     * How many days are in the specified month and year?
     */
    static int daysInMonth(int month, int year);

    /*
     * Returns the actual number of days between the dates specified (no
     * date count convention is applied).
     */
    static int diffDays(FinDate date1, FinDate date2){
        return (int)(date2 - date1);
    }

    static int diffDays(FinDate date1, FinDate date2, 
			FinEnum::DayCountConvention dc);

    static double yearFrac(FinDate date1, FinDate date2,
			   FinEnum::DayCountConvention dc) {
	return yearFrac(date1, date2, dc, 0);
    }	

    static double yearFrac(FinDate date1, FinDate date2, 
			   FinEnum::DayCountConvention dc, 
			   int extraDays);

    FinDate addMonths(int n, bool adjustEOM) const;
    bool isBusDay() const; // no holiday calendar support
    FinDate nextBusDay() const;
    FinDate prevBusDay() const;
    FinDate modNextBusDay() const;
    FinDate spxDate() const;
    FinDate immDate() const;


    // This flag may be helpful if we decide to support the strange SIA
    // day count convention where the day count depends on whether or not
    // there is a coupon on the last day of February. Mixing day count
    // conventions with security characteristics will result in ugly code, and
    // we ignore this for now. See the comments on DC30360P1 in diffDays() for
    // more information.
    static bool adjustFebruary;

    /**
     * Return the julian day number for this date.
     */
    int serialJulian() const { return jdn; }

    /**
     * Return the default representation used by Excel for the current 
     * date and time when running on a PC. This is called the "1900
     * date system," and it can be changed using an Excel option. 
     */
    double serialExcel1900() const { return jdn - FinDate::Excel1900_Offset; }

    /**
     * Return the default representation used by Excel for the current 
     * date and time when running on a Mac. This is called the "1904
     * date system," and it can be changed using an Excel option.
     */
    double serialExcel1904() const { return jdn - FinDate::Excel1904_Offset; }

    friend std::ostream& operator<<(std::ostream& os, const FinDate& date);
};

    double serialNumber(cxxPack::FinDate&, cxxPack::SerialType);
    double serialNumber(RcppDate&, cxxPack::SerialType);
    double serialNumber(RcppDatetime&, cxxPack::SerialType);

} // end cxxPack namespace

namespace Rcpp {

    // Work-aruond for the fact that Rcpp::wrap(obj) does not work when
    // obj is an object of a class that has operator SEXP().
    template<> SEXP wrap<cxxPack::FinDate>(const cxxPack::FinDate& x);

}

#endif

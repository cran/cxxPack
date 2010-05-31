// FinDate.cpp: financial date calculations.
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

#include <FinDate.hpp>

#include <ostream>

namespace cxxPack {

FinEnum::MonthEndAdjustment defaultAdjustment = FinEnum::NoAdj;
bool FinDate::adjustFebruary = true;

/**
 * SEXP constructor for FinDate.
 */
FinDate::FinDate(SEXP dateSEXP) {
    Rcpp::CharacterVector classAttr = Rcpp::RObject(dateSEXP).attr("class");
    if(std::string(classAttr[0]) != "Date")
	throw std::range_error("Invalid class in FinDate constr");
    jdn = (int)(Rcpp::as<double>(dateSEXP) + R_Offset);
    parsed = false;
}

/**
 * Returns the number of days between the dates specified, using the
 * specified day count convention.
 */
int FinDate::diffDays(FinDate date1, FinDate date2, 
		      FinEnum::DayCountConvention dc){
    int result = (int)(date2 - date1);
    FinEnum::DayCountConvention dateCount = dc;

    if(result == 0
       || dateCount == FinEnum::DCAA
       || dateCount == FinEnum::DCA360
       || dateCount == FinEnum::DCA365
       || dateCount == FinEnum::DCA252)
	return result;
    
    int d1, d2, m1, m2, y1, y2;
    if(result > 0) {
	m1 = date1.getMonth();
	m2 = date2.getMonth();
	d1 = date1.getDay();
	d2 = date2.getDay();
	y1 = date1.getYear();
	y2 = date2.getYear();
    }
    else {
	m1 = date2.getMonth();
	m2 = date1.getMonth();
	d1 = date2.getDay();
	d2 = date1.getDay();
	y1 = date2.getYear();
	y2 = date1.getYear();
    }
    
    FinDate leapDate;
    switch (dateCount){
	
    case FinEnum::DCA360NL:
    case FinEnum::DCA365NL:
	
	// Do not count leap days between date1 and date2.
	// We assume that |year1 - year2| <= 1, so one of the dates
	// must fall in a leap year if there is a leap day between
	// the dates.
	if(FinDate::isLeapYear(y1))
	    leapDate = FinDate(Month(2), 29, y1);
	else if(FinDate::isLeapYear(y2))
	    leapDate = FinDate(Month(2), 29, y2);
	else
	    break;
	
	if(date1 <= leapDate
	   && date2 >= leapDate)
	    result--;
	else if(date1 >= leapDate
		&& date2 <= leapDate)
	    result++;
	break;
	
    case FinEnum::DC30360I: // 30/360 ISDA --- Standard
	
	// ISDA 30/360 convention (most often used).
	// If d1 = min(d1,30); if d2 == 31 and d1 == 30, d2 = 30.
	if(d1 == 31) d1 = 30;
	if(d1 == 30 && d2 == 31) 
	    d2 = 30;
	if (result > 0 )
	    result = (y2-y1)*360 + (m2-m1)*30 + d2-d1;
	else 
	    result = -((y2-y1)*360 + (m2-m1)*30 + d2-d1);
	break;
	
    case FinEnum::DC30360E: // 30E/360 --- Symmetric
	
	// Eurobond convention (or Symmetric 30/360 convention)
	// d1 = min(d1,30); d2 = min(d2,30);
	if(d1 == 31) d1 = 30;
	if(d2 == 31) d2 = 30;
	if (result > 0 )
	    result = (y2-y1)*360 + (m2-m1)*30 + d2-d1;
	else 
	    result = -((y2-y1)*360 + (m2-m1)*30 + d2-d1);
	break;
	
    case FinEnum::DC30360P1: // 30/360 PSA, Feb. adjust for d1
	
	// Bond basis or PSA 30/360 convention. 
	// If d1 == 31 or d1 == last day of Feb., d1 = 30;
	// If d2 == 31 and d1 (adjusted) == 30, d2 = 30.
	// SIA modification: Don't adjust d1 in case
	// d1 = last day of Feb. unless there is a coupon on this day.
	if ((m1 == cxxPack::Feb) && adjustFebruary) {
	    if((isLeapYear(y1) && d1 == 29) || d1 == 28)
		d1 = 30;
	}
	else if(d1 > 30) 
	    d1 = 30;
	if(d1 == 30 && d2 == 31)
	    d2 = 30;
	if(result > 0)
	    result = (y2-y1)*360 + (m2-m1)*30 + d2-d1;
	else 
	    result = -((y2-y1)*360 + (m2-m1)*30 + d2-d1);
	break;
	
    case FinEnum::DC30360P2: // 30/360 PSA, Feb. adjust for d1 and d2
	
	// This is a kind of symmetric PSA convention where Feb. is
	// given special treatment for both d1 and d2, with an optional
	// SIA modification applied to d2. (See previous case.)
	if (m1 == cxxPack::Feb) {
	    if((isLeapYear(y1) && d1 == 29) || d1 == 28)
		d1 = 30;
	}
	else if(d1 == 31) 
	    d1 = 30;
	if ((m2 == cxxPack::Feb) && adjustFebruary) {
	    if((isLeapYear(y2) && d2 == 29) || d2 == 28)
		d2 = 30;
	}
	else if(d2 == 31) 
	    d2 = 30;
	if (result > 0)
	    result = (y2-y1)*360 + (m2-m1)*30 + d2-d1;
	else 
	    result = -((y2-y1)*360 + (m2-m1)*30 + d2-d1);
	break;
	
    default:
	break;
    }
    return result;
}


/**
 * Computes year fraction based on specified dates and day count
 * convention. The optional extraDays parameter can be used to handle
 * cases like some Italian bonds where both endpoints are included in
 * the accrual period, so we must add 1 to the numerator.
 */
double FinDate::yearFrac(FinDate date1, FinDate date2, 
			 FinEnum::DayCountConvention dc, 
			 int extraDays) {
    double frac = 0.;
    if(date1 >= date2) { // Should throw an exception here...
	throw std::range_error("Error: date1 >= date2 in yearfrac");
    }
    int diff = diffDays(date1, date2, dc);
    if (dc != FinEnum::DCAA) {
	switch(dc) {
	case FinEnum::DCAA:        
	    break;
	case FinEnum::DCA365:
	case FinEnum::DCA365NL:
	    frac = (diff+extraDays) / 365.0;
	    break;
	case FinEnum::DCA252:
	    frac = (diff+extraDays) / 252.0;
	    break;
	case FinEnum::DCA360:
	case FinEnum::DC30360I:
	case FinEnum::DC30360E:
	case FinEnum::DC30360P1:
	case FinEnum::DC30360P2:
	case FinEnum::DCA360NL:
	default:
	    frac = (diff+extraDays) / 360.0;
	    break;
	}
    }
    else { /* DCAA */
	  
	// Actual number of days divided by the actual number 
	// of days in the year(s) where the interval between 
	// date1 and date2 falls.The algorithm here permits the two dates to
	// be separated by two or more years, and each subinterval is
	// divided by the approriate number based on leap year status.
	if(date1.getYear() == date2.getYear())
	    return diff/(date1.isLeapYear()?366.0:365.0);
	
	FinDate eoy1 = FinDate(Month(12), 31, date1.getYear());
	FinDate eoy2 = FinDate(Month(12), 31, date1.getYear()+1);
	frac += diffDays(date1, eoy1)/(date1.isLeapYear()?366.0:365.0);
	while(date2 > eoy2) {
	    frac += diffDays(eoy1,eoy2)/(eoy2.isLeapYear()?366.0:365.0);
	    eoy1 = eoy2;
	    eoy2 = FinDate(Month(12),31,eoy1.getYear()+1);
	}
	frac += diffDays(eoy1, date2)/(date2.isLeapYear()?366.0:365.0);
    }
    return(frac);
}    

/**
 * True if not Sat or Sun, false otherwise.
 */
bool FinDate::isBusDay() const { // no holiday calendar support yet.
    int day = getWeekday();
    if(day == cxxPack::Sat || day == cxxPack::Sun)
	return false;
    else
	return true;
}

/**
 * Return next business day (no holiday support).
 */
FinDate FinDate::nextBusDay() const {
    int jdn = serialJulian();
    FinDate temp = FinDate(++jdn);
    while(!temp.isBusDay())
	temp = FinDate(++jdn);
    temp.parsed = false;
    return temp;
}

/**
 * Returns previous business date.
 */
FinDate FinDate::prevBusDay() const {
    int jdn = serialJulian();
    FinDate temp = FinDate(--jdn);
    while(!temp.isBusDay())
	temp = FinDate(--jdn);
    temp.parsed = false;
    return temp;
}

/**
 * Returns next business date, unless it would fall
 * in the following month, in which case the previous
 * business date is returned.
 */
FinDate FinDate::modNextBusDay() const {
    FinDate temp = nextBusDay();
    if(temp.getMonth() != getMonth())
	temp = prevBusDay();
    temp.parsed = false;
    return temp;
}

/**
 * Returns the expiration date of an S&P 500 index option that
 * expires in the current month and year. This is the Saturday
 * immediately following the third Friday.
 */
FinDate FinDate::spxDate() const {
    FinDate expiry(Month(getMonth()),1,getYear());
    expiry = expiry.nthWeekday(3, cxxPack::Fri);
    expiry = FinDate(expiry.serialJulian()+1);
    expiry.parsed = false;
    return expiry;
}

/**
 * Returns the expiration date of a Eurodollar futures contract
 * in the current month and year. This is the second London business day
 * before the third Wednesday. TODO: Support for London holidays.
 */
FinDate FinDate::immDate() const {
    FinDate expiry(Month(getMonth()),1,getYear());
    expiry = expiry.nthWeekday(3, cxxPack::Wed);// Third Wednesday.
    expiry = expiry.prevBusDay().prevBusDay();// Two business days prior.
    expiry.parsed = false;
    return expiry;
}

/**
 * Returns the date n months from today with the same day
 * number if possible, and with the date forced to the last
 * day of the target month if adjustEOM is true, and if
 * the starting date is the last day of the starting month.
 */
FinDate FinDate::addMonths(int n, bool adjustEOM) const {
    int m1, d1, y1, m2=0, d2=0, y2=0;

    m1 = getMonth(), d1 = getDay(), y1 = getYear();

    if(n == 0)
	return *this;

    // Force target date to EOM if starting date is EOM.
    bool forceEOM = adjustEOM
	&& (d1 == daysInMonthTable[this->isLeapYear()?1:0][m1]);
	
    if(m1-1+n >= 0) {
	m2 = 1+(m1-1+n)%12;
	y2 = y1+(m1-1+n)/12;
    }
    else {
	m2 = 12-((12-m1-n)%12);
	y2 = y1-((12-m1-n)/12);
    }
    
    FinDate targetYear(Month(1),1,y2);
    int numDays2 = daysInMonthTable[targetYear.isLeapYear()?1:0][m2];
    if(numDays2 >= d1)
	d2 = d1; // no adjustment
    else
	d2 = numDays2; // not enough days
    if(forceEOM) d2 = numDays2; // adjust
    
    return FinDate(Month(m2),d2,y2);
}

// Print an FinDate.
std::ostream& operator<<(std::ostream& os, const FinDate& date) {
    int weekday = date.getWeekday();
    if(weekday < 0 || weekday > 6)
	throw std::range_error("weekday out of range");
    os << FinDate::weekdayName[weekday] << " "
       << date.getMonth() << "/" << date.getDay() << "/" << date.getYear();
    return os;
}
    
double operator-(const FinDate& date2, const FinDate& date1) {
    return date2.jdn - date1.jdn;
}

FinDate operator+(const FinDate& date, int offsetDays) {
    FinDate temp(date);
    temp.jdn += offsetDays;
    temp.parsed = false;
    return temp;
}
FinDate& FinDate::operator+=(int offsetDays) {
    jdn += offsetDays;
    parsed = false;
    return *this;
}
FinDate& FinDate::operator-=(int offsetDays) {
    jdn -= offsetDays;
    parsed = false;
    return *this;
}
FinDate& FinDate::operator++() { // prefix
    jdn++;
    parsed = false;
    return *this;
}
FinDate FinDate::operator++(int) { // postfix
    FinDate temp(*this);
    jdn++;
    parsed = false;
    return temp;
}
FinDate& FinDate::operator--() { // prefix
    jdn--;
    parsed = false;
    return *this;
}
FinDate FinDate::operator--(int) { // postfix
    FinDate temp(*this);
    jdn--;
    parsed = false;
    return temp;
}

// There is the possibility that these comparisons may not work
// as expected due to roundoff, but this is not an issue for
// R type Date because there is no fractional part and different
// Date's will have rdtnum values that differ by at least DAYS2SECS.
bool  operator<(const FinDate &date1, const FinDate& date2) {
    return date1.jdn < date2.jdn;
}

bool  operator>(const FinDate &date1, const FinDate& date2) {
    return date1.jdn > date2.jdn;
}

bool  operator>=(const FinDate &date1, const FinDate& date2) {
    return date1.jdn >= date2.jdn;
}

bool  operator<=(const FinDate &date1, const FinDate& date2) {
    return date1.jdn <= date2.jdn;
}

bool  operator==(const FinDate &date1, const FinDate& date2) {
    return date1.jdn == date2.jdn;
}

bool operator!=(const FinDate &date1, const FinDate& date2) {
    return date1.jdn != date2.jdn;
}

std::string FinDate::getWeekdayName() const {
    int weekday = getWeekday();
    if(weekday < 0 || weekday > 6)
	throw std::range_error(std::string("getWeekdayName: invalid weekday: ")
			  +to_string(weekday));
    return FinDate::weekdayName[weekday];
}
std::string FinDate::getMonthName() const {
    if(month < 1 || month > 12)
	throw std::range_error(std::string("getMonthName: invalid month")
			  +to_string(month));
    return FinDate::monthName[month-1];
}

// Class constants.
const int FinDate::R_Offset = 2440588; // Julian day number for 1/1/1970, JDN(1/1/1970).
const int FinDate::Excel1900_Offset = 2415019; // JDN(1/1/1900) - 2
const int FinDate::Excel1904_Offset = 2416481; // JDN(1/1/1904)
const int FinDate::IsdaCds_Offset   = 2305814; // JDN(1/1/1601)
const int FinDate::DAYS2SECS = 60*60*24;
    const int FinDate::serialOffsets[] = { 0, // NULL
				       FinDate::R_Offset,         // R
				       FinDate::Excel1900_Offset, // PC
				       FinDate::Excel1904_Offset, // MacOS
				       FinDate::Excel1900_Offset, // QuantLib
				       FinDate::IsdaCds_Offset,   // ISDA CDS
				       0 }; // Julian
const int FinDate::daysInMonthTable[2][13] = {
	    {0,31,28,31,30,31,30,31,31,30,31,30,31},
	    {0,31,29,31,30,31,30,31,31,30,31,30,31}
};
const std::string FinDate::weekdayName[] = { "Sun", "Mon", "Tue", "Wed", 
					     "Thu", "Fri", "Sat", "Sun" };
const std::string FinDate::monthName[] = { "Jan", "Feb", "Mar", "Apr",
					   "May", "Jun", "Jul", "Aug", "Sep",
					   "Oct", "Nov", "Dec" };

// The Julian day number (jdn) is the number of days since Monday,
// Jan 1, 4713BC (year = -4712). Here 1BC is year 0, 2BC is year -1, etc.
// On the other hand, R measures days since Jan 1, 1970, and these dates are
// converted to jdn's by adding R_Offset.
//
// mdy2jdn and jdn2mdy are inverse functions for dates back to 
// year = -4799 (4800BC).
//
// See the Wikipedia entry on Julian day number for more information 
// on these algorithms.
//

/**
 * Transform month/day/year to Julian day number.
 */
int FinDate::mdy2jdn(int month, int day, int year) {
    int m = month, d = day, y = year;
    int a = (14 - m)/12;
    y += 4800 - a;
    m += 12*a - 3;
    int jdn = (d + (153*m + 2)/5 + 365*y
	       + y/4 - y/100 + y/400 - 32045);
    return(jdn);
}

/**
 * Returns the day of the week for the current date. A
 * favorite trick done effortlessly by savants, probably
 * by another method!
 */
int FinDate::getWeekday() const { // Sun=0, Mon=1, ..., Sat=6
    int jdn = serialJulian();
    return (jdn+1)%7;
}

/**
 * Returns date of next weekday (FinDate::Mon, 
 * FinDate::Tue, etc.), not including the current date.
 */
FinDate FinDate::nextWeekday(int dayNum) const {
    int jdn = serialJulian();
    int weekday = (jdn+1)%7;
    int offset = (dayNum - weekday + 7) % 7;
    if(offset == 0)
	offset += 7;
    FinDate tmp(jdn+offset, true);
    tmp.parsed = false;
    return tmp;
}

/**
 * Returns the nth-occurence of the specified weekday in the
 * current month and year, where the first day of the month is
 * counted if it happens to be the specified weekday.
 */
FinDate FinDate::nthWeekday(int n, int dayNum) const {
    int count = 0;
    FinDate date(Month(getMonth()), 1, getYear());
    if(date.getWeekday() != dayNum)
	date = date.nextWeekday(dayNum);
    for(count = 1; count < n; count++)
	date = FinDate(date.serialJulian()+7,true);
    date.parsed = false;
    return date;
}

/**
 * Returns true if the current date falls in a leap year.
 */
bool FinDate::isLeapYear() const {
    int year = getYear();
    return ((year%4 == 0)||(year%400 == 0)) && !(year%100 == 0);
}

/**
 * Corresponding static function.
 */
bool FinDate::isLeapYear(int year) {
    return ((year%4 == 0)||(year%400 == 0)) && !(year%100 == 0);
}

/**
 * Returns number of days in specified month and year.
 */
int FinDate::daysInMonth(int month, int year) {
    if(month < 1 || month > 12)
	throw std::range_error("Invalid month/year in daysInMonth");
    return daysInMonthTable[isLeapYear(year)?1:0][month];
}

// The Julian day number (jdn) is the number of days since Monday,
// Jan 1, 4713BC (year = -4712). Here 1BC is year 0, 2BC is year -1, etc.
// On the other hand, R measures days since Jan 1, 1970, and these dates are
// converted to jdn's by adding Jan1970Offset.
//
// mdy2jdn and jdn2mdy are inverse functions for dates back to 
// year = -4799 (4800BC).
//
// See the Wikipedia entry on Julian day number for more information 
// on these algorithms.
//

/**
 * Transform month/day/year to Julian day number.
 */
void FinDate::mdy2jdn() {
    int m = month, d = day, y = year;
    int a = (14 - m)/12;
    y += 4800 - a;
    m += 12*a - 3;
    jdn = (d + (153*m + 2)/5 + 365*y
	   + y/4 - y/100 + y/400 - 32045);
}

// Transform from Julian day number to month/day/year.
void FinDate::jdn2mdy() const {
    int jul = jdn + 32044;
    int g = jul/146097;
    int dg = jul % 146097;
    int c = (dg/36524 + 1)*3/4;
    int dc = dg - c*36524;
    int b = dc/1461;
    int db = dc % 1461;
    int a = (db/365 + 1)*3/4;
    int da = db - a*365;
    int y = g*400 + c*100 + b*4 + a;
    int m = (da*5 + 308)/153 - 2;
    int d = da - (m + 4)*153 /5 + 122;
    y = y - 4800 + (m + 2)/12;
    m = (m + 2) % 12 + 1;
    d = d + 1;
    month = m;
    day   = d;
    year  = y;
}

/**
 * Transform from Julian day number to month/day/year.
 * (More useful than above.)
 */
DateMDY FinDate::jdn2mdy(int jdn) {
    int jul = jdn + 32044;
    int g = jul/146097;
    int dg = jul % 146097;
    int c = (dg/36524 + 1)*3/4;
    int dc = dg - c*36524;
    int b = dc/1461;
    int db = dc % 1461;
    int a = (db/365 + 1)*3/4;
    int da = db - a*365;
    int y = g*400 + c*100 + b*4 + a;
    int m = (da*5 + 308)/153 - 2;
    int d = da - (m + 4)*153 /5 + 122;
    y = y - 4800 + (m + 2)/12;
    m = (m + 2) % 12 + 1;
    d = d + 1;
    return DateMDY(m,d,y);
}

    double serialNumber(cxxPack::FinDate& d, cxxPack::SerialType type) {
	if(type < 1 || type > cxxPack::Julian)
	    throw std::range_error("Invalid serial number type");
	int offset = FinDate::serialOffsets[type];
	return d.serialJulian() - offset;
    }

    double serialNumber(RcppDate& d, cxxPack::SerialType type) {
	if(type < 1 || type > cxxPack::Julian)
	    throw std::range_error("Invalid serial number type");
	int Roffset = FinDate::serialOffsets[1];
	int offset = FinDate::serialOffsets[type];
	return d.getJulian() + Roffset - offset;
    }

    double serialNumber(RcppDatetime& dt, cxxPack::SerialType type) {
	if(type < 1 || type > cxxPack::Julian)
	    throw std::range_error("Invalid serial number type");
	double Rserial = dt.getFractionalTimestamp();
	if(type == cxxPack::R)
	    return Rserial; // measured in seconds, not days
	int offset = FinDate::serialOffsets[type];
	double val = Rserial/cxxPack::FinDate::DAYS2SECS 
	    + cxxPack::FinDate::R_Offset - offset;
	if(   type == cxxPack::Julian
	   || type == cxxPack::IsdaCds
	   || type == cxxPack::QuantLib)
	    val = (int)val;
	return val;
    }

FinDate::operator SEXP() {
    Rcpp::NumericVector value(1);
    value[0] = getRValue();
    Rcpp::RObject robj((SEXP)value);
    robj.attr("class") = "Date";
    return value;
}

} // end cxxPack namespace

namespace Rcpp {

// Work-aruond for the fact that Rcpp::wrap(obj) does not work when
// obj is an object of a class that has operator SEXP().
template<> SEXP wrap<cxxPack::FinDate>(const cxxPack::FinDate& x) {
    cxxPack::FinDate *p = const_cast<cxxPack::FinDate*>(&x);
    return (SEXP)*p;
}

}

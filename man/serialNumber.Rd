\name{serialNumber}
\alias{serialNumber}
\title{Returns the serial number used to represent dates on various systems.}
\description{
  Returns the serial number used to represent dates on Excel (PC and
  Mac variants), QuantLib, the ISDA CDS Standard Model, R, and the
  Julian day number system.
}
\usage{
serialNumber(date, type='Excel1900')
}
\arguments{
  \item{date}{a date object of class Date or POSIXt.}
  \item{type}{a character string with possible values: \code{Excel1900},
    \code{Excel1904}, \code{QuantLib}, \code{IsdaCds}, \code{R}, and
    \code{Julian}.}
}
\details{

  The Julian day number is a commonly used measure of time that measures
  the number of days since Monday, Jan 1, 4713 BC (year = -4712). Here
  1BC is year 0, 2BC is year -1, etc. (the C++ conversion functions in
  cxxPack map Julian day numbers to m/d/y for dates going back to
  year -4799, or 4800 BC).

  The serial numbers differ based on what reference date is used, and on
  whether or not fractional days are included. All of the serial numbers
  are measured in days with the exception of R POSIXt dates where time
  is measued in seconds (R's Date type measures time in days)

  The reference date for R dates is 1/1/1970 (so dates before this date
  will have negative serial numbers). The refernce date used for the
  ISDA CDS Standard Model is 1/1/1601.

  QuantLib uses the same serial number that Excel does on a Windows PC.

  Excel supports two date systems called the "1900 date system" and
  that "1904 date system," respectively. The first is the default under
  Windows, and the second is the default under MacOS. (There are
  Excel options to change the default behavior.) The 1900 date system
  is 1-based, that is, Jan 1, 1900 is assigned serial number 1. The
  1904 system is 0-based, so Jan 1, 1904 is assigned serial number 0.
  Serial number type \code{Excel1900} selects the 1900 date system, and
  type \code{Excel1904} selects the 1904 date system.

  These confusing conventions stem from the fact that Lotus 1-2-3 and
  Microsoft Excel contained a bug that caused 1900 to be seen
  (incorrectly) as a leap year. Thus serial numbers after Mar 1, 1900
  were one unit larger than they should be, and the Excel WEEKDAY
  function was patched to compensate for this problem.

  The 1900 date system measures days from 1/1/1900, but the
  serial number for recent dates is two units larger than the
  corresponding Julian day number difference between the date
  and 1/1/1900 because: 1. the 1900 date system
  is 1-based, not 0-based, and 2. the Excel serial number jumps
  by 1 as we pass Feb 29, 1900, a date that does not exist.
  The 1904 date system does not suffer from either of these
  problems, so its serial number is equal to the Julian day
  number for that date minus the Julian day number for 1/1/1904.

  To summarize, the serial number for date \code{D} is equal to
  \code{JDN(D) - JDN(BaseDate)}, where \code{JDN()} computes
  the Julian day number, with two exceptions. First, in the
  case of \code{Excel1900}, the serial number (for recent dates) is
  two units larger than this. Second, for R's POSIXt date type
  this difference must be multiplied by the number of seconds
  in one day (\code{D} has a fractional part in this case).
}
\value{
  Returns the serial number that applies. The Julian day number is
  always an integer, but the other serial numbers normally include
  a fractional part to represent fractions of a day (down to the
  fraction of a second).
}
  
\author{Dominick Samperi}

\examples{
  d <- as.Date('1970-01-01')
  serialNumber(d, 'Julian') # returns 2440588
  serialNumber(d, 'R') # returns 0
  dt <- Sys.time() # current date and time
  serialNumber(dt, 'Julian') # a large integer (no fraction)
  serialNumber(dt, 'Excel1900') # includes fractional days
}

\keyword{models}

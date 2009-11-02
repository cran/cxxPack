\name{DateNum}
\alias{DateNum}
\alias{numJulian}
\alias{numExcelPC}
\alias{numExcelMac}
\alias{numWeekday}
\alias{numR}
\title{Julian day numbers and other date serial numbers.}
\description{
  Various date utility functions that return date serial numbers
  used by R, Excel, Julian system, etc.
}
\usage{
numJulian(date)
numExcelPC(date)
numExcelMac(date)
numWeekday(date)
}
\arguments{
  \item{date}{a date object of class Date or POSIXt}
}
\details{

  The Julian day number is a standardized measure of time starting from a
  distant date in the past. The starting date for R's Date type,
  1/1/1970, has Julian day number \code{ROffset} (2440588).
  
  The function \code{numWeekday()} returns the weekday corresponding to a
  particular date: Sun=0, Mon=1, etc. This is computed from the Julian
  day number as: (JDN + 1) modulo 7.

  The default date represention used by Excel on a PC is returned by
  \code{numExcelPC()}, and the default representation used by Excel
  on a Mac is returned by \code{numExcelMac()}. To get the time right
  these functions should be applied to POSIXt type date-times; otherwise
  the time-of-day component is truncated.

}
\value{
  Returns the serial number that applies. The julian day number is
  always an integer, but the other serial numbers normally include
  a fractional part to represent fractions of a day (down to the
  fraction of a second).
}
  
\author{Dominick Samperi}

\examples{
  d <- as.Date('1970-01-01')
  numJulian(d) # returns 2440588
  numR(d) # returns 0
  numWeekday(d) # returns 4 (Thursday)
  dt <- Sys.time() # current date and time
  numJulian(dt) # a large integer (fractional part dropped)
  numExcelPC(dt) # includes fractional days
}

\keyword{models}

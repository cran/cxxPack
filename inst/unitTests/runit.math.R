# Test math functions in cxxPack

# Check complex gamma function
test.cgamma <- function() {
  n <- 10
  z <- as.complex(1:n)
  vec <- .Call('cgamma_', z) # complex gamma: Gamma(k) = (k-1)!
  checkEquals(vec, as.complex(gamma(1:n))) # check using R's real gamma
}

# Check date utilities (defined on R side)
test.date.serialNumber.R <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'R')
  checkEquals(x, 14714 + 0:10)
}

test.date.serialNumber.QuantLib <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'QuantLib')
  checkEquals(x, 40283 + 0:10)
}

test.date.serialNumber.IsdaCds <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'IsdaCds')
  checkEquals(x, 149488 + 0:10)
}

test.date.serialNumber.Excel1900 <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'Excel1900')
  checkEquals(x, 40283 + 0:10)
}

test.date.serialNumber.Excel1904 <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'Excel1904')
  checkEquals(x, 38821 + 0:10)
}

test.date.serialNumber.Julian <- function() {
  d <- as.Date('2010-04-15') + 0:10
  x <- serialNumber(d, 'Julian')
  checkEquals(x, 2455302 + 0:10)
}

test.date.serialNumber.Exception <- function() {
  d <- as.Date('2010-04-15') + 0:10
  checkException(serialNumber(d, 'Junk'))
}

test.datetime.serialNumber.Excel1900 <- function() {
  d <-as.Date('2010-04-14') + 0:10
  dt <- as.POSIXct(d) + .25*24*60*60 # add 1/4 day
  x <- serialNumber(dt, 'Excel1900')
  checkEquals(x, 40283.25 + 0:10, tolerance=1.0e-4)
}

test.datetime.serialNumber.Excel1904 <- function() {
  d <-as.Date('2010-04-14') + 0:10
  dt <- as.POSIXct(d) + .25*24*60*60 # add 1/4 day
  x <- serialNumber(dt, 'Excel1904')
  checkEquals(x, 38821.25 + 0:10, tolerance=1.0e-4)
}

test.datetime.serialNumber.R <- function() {
  d <-as.Date('2010-04-14') + 0:10
  dt <- as.POSIXct(d) + .25*24*60*60 # add 1/4 day
  x <- serialNumber(dt, 'R')
  checkEquals(x, 14714.25 + 0:10, tolerance=1.0e-4)
}

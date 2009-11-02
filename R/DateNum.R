# Private constants (not exported)
ROffset <- 2440588 # JDN(1/1/1970)
Excel1900Offset <- 2415019 # JDN(1/1/1900) - 2
Excel1904Offset <- 2416481 # JDN(1/1/1904)
DAYS2SECS <- 60*60*24

numR <- function(date) {
  return(as.numeric(date))
}

numJulian <- function(date) {
  dateclass <- class(date)
  if(length(dateclass) == 2 && dateclass[1] == "POSIXt") {
    return(as.numeric(date)/DAYS2SECS + ROffset)
  }
  else {
    if(length(dateclass) == 1 && dateclass == "Date") {
      return(as.numeric(date) + ROffset)
    }
    else {
      stop('Invalid date')
    }
  }
}

numExcelPC <- function(date) {
  dateclass <- class(date)
  if(length(dateclass) == 2 && dateclass[1] == "POSIXt") {
    return(as.numeric(date)/DAYS2SECS + ROffset - Excel1900Offset)
  }
  else {
    if(length(dateclass) == 1 && dateclass == "Date") {
      return(as.numeric(date) + ROffset - Excel1900Offset)
    }
    else {
      stop('Invalid date')
    }
  }
}

numExcelMac <- function(date) {
  dateclass <- class(date)
  if(length(dateclass) == 2 && dateclass[1] == "POSIXt") {
    return(as.numeric(date)/DAYS2SECS + ROffset - Excel1904Offset)
  }
  else {
    if(length(dateclass) == 1 && dateclass == "Date") {
      return(as.numeric(date) + ROffset - Excel1904Offset)
    }
    else {
      stop('Invalid date')
    }
  }
}

# Sun=0, Mon=1, etc.
numWeekday <- function(date) {
  (numJulian(date)+1)%%7
}


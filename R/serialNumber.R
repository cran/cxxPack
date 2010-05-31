# Date serial number offsets (see man page on serialNumber)

#ROffset <- 2440588 # JDN(1/1/1970)
#Excel1900Offset <- 2415019 # JDN(1/1/1900) - 2
#Excel1904Offset <- 2416481 # JDN(1/1/1904)
#QuantLibOffset <- Excel1900Offset 
#IsdaCdsOffset <- 2305814 # JDN(1/1/1601)

serialtypes <- c('R', 'Excel1900', 'Excel1904', 'QuantLib', 'IsdaCds', 'Julian')
serialoffsets <- c(2440588, 2415019, 2416481, 2415019, 2305814, 0)
DAYS2SECS <- 60*60*24

serialNumber <- function(date, type='Excel1900') {
  ind <- which(type == serialtypes)
  if(length(ind)) {
    Roffset <- serialoffsets[1]
    offset <- serialoffsets[ind]
    dateclass <- class(date)
    if("POSIXt" %in% dateclass) {
      return(as.numeric(date)/DAYS2SECS + Roffset - offset)
    }
    else {
      if("Date" %in% dateclass) {
        return(as.numeric(date) + Roffset - offset)
      }
      else {
        stop('Invalid date')
      }
    }
  }
  else {
    stop('invalid serial number type')
  }
}


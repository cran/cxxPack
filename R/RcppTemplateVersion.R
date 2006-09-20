RcppTemplateVersion <- function() {

  version <- installed.packages()["RcppTemplate","Version"]
                                 
  # Print version of this package
  cat('\nRcppTemplate package Version ', version, '\n\n')

  # Print version of Rcpp/RcppTemplate used to build this package
  licenseFile <- file(system.file(".","LICENSE-Rcpp.txt",package="RcppTemplate"),"r")
  writeLines(readLines(licenseFile)[1:4])
}

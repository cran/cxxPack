\name{RcppExample}
\alias{RcppExample}
\alias{print.RcppExample}
\title{Rcpp R/C++ interface example}
\description{
  \code{RcppExample} illustrates how the \code{Rcpp} R/C++
  interface class library is used.
}
\usage{
RcppExample(params, nlist, numvec, nummat, df, datevec,
datetimevec, stringvec, func, hypot, zoots=NULL, myfactor=NULL)
\method{print}{RcppExample}(x,...)
}
\arguments{
  \item{params}{A heterogeneous list of name-value pairs where the
    values can be of different types (double, int, Date, etc.). The
    names used will be referred to explicitly on the C++ side and an
    exception is thrown if a name is not found.}
  \item{nlist}{a list of named numeric values (double or int). Use of
    this is deprecated and support for this may be discontinued in the
    future (use params instead).}
  \item{numvec}{a numeric 1D vector (double or int).}
  \item{nummat}{a numeric 2D matrix (double or int).}
  \item{df}{a data frame.}
  \item{zoots}{a zoo time series object.}
  \item{myfactor}{an R factor}
  \item{datevec}{a vector of Date's.}
  \item{datetimevec}{a vector of POSIXt dates.}
  \item{stringvec}{a vector of strings.}
  \item{func}{an R function that takes complex params and returns a vector}
  \item{hypot}{a function that computes the distance to the origin}
  \item{x}{Object of type \code{RcppExample}.}
  \item{...}{Extra named parameters.}
  }
\details{

  The source files for this
  example and for the \code{Rcpp} class library
  can be found in the RcppTemplate package source archive (the .tar.gz file).
}
\value{
  \code{RcppExample} returns a list containing name-value pairs where
  the values are the R objects corresponding to selected C++ objects.
  The original input parameter params is also returned.
  }

\author{Dominick Samperi}

\references{
  Samperi, D., (2009) \emph{Rcpp: A Class Library for R/C++
    Object Mapping and R Package Development}. Located in the
  package \code{doc} subdirectory. Can be displayed using the
  \code{showRcppDoc()} command.
  
  Samperi, D., (2009) \emph{Rcpp Quick Reference}, located
  in the package \code{doc} subdirectory. Can be displayed using the
  \code{showRcppQuickRef()} command.
    
  \emph{Writing R Extensions}, available at \url{http:www.r-project.org}.
}
\examples{

Sys.getenv("R_PLATFORM")

currentTime <- Sys.time() # POSIXt type (date and time)
startDate <- as.Date('2009-10-01')
endDate <- as.Date('2011-05-01')

params <- list(method='BFGS',
               tolerance=1.0e-8,
               maxIter=1000,
               currentTime=currentTime,
               startDate=startDate,
               endDate=endDate,
               weekday=5, # Friday (Sun=0, Mon=1, etc.)
               nthOccurrence=3)

nlist <- list(ibm = 80.50, hp = 53.64, c = 45.41)

numvec <- as.double(seq(1,5)) # numerical vector

nummat <- matrix(as.double(seq(1,20)),4,5) # numerical matrix

stringvec <- c("hello", "world", "fractal") # string vector

datetimevec <- currentTime + (1:3)*60*60*24 # 1-day increments

datevec <- as.Date(currentTime) + 1:3 # drop fractional day part, scale rep.

myfactor <- as.factor(c('good','bad','bad','good','good','great'))

# Tell R not to consider boy/dog/cat a factor here
df <- data.frame(factr=c('beta','beta','gamma'),
                 words=I(c('boy','dog','cat')),
                 nums=c(1.1,3.14,5.6),
                 bools=c(TRUE,TRUE, FALSE),
                 dates=datevec,
                 times=datetimevec)

# Create zoo time series object indexed by irregular dates.
zoots <- zoo(matrix(1:12,3,4), as.Date('2009-10-01') + c(1,5,2))

# Define R function to be called from C++.
func <- function(df.dat, ts.dat, x) {
  if(class(df.dat) == "data.frame") {
    cat('func got valid data frame\n')
  }
  else {
    stop('func got invalid data frame\n')
  }
  if(length(class(ts.dat)) == 2 && class(ts.dat) == "zooreg") {
    cat('func got valid regular zoo time series\n')
  }
  else {
    if(class(ts.dat) == "zoo") {
      cat('func got valid irregular zoo time series\n')
    }
    else {
      stop('func got invalid zoo time series\n')
    }
  }
  as.double(x^2)
}

hypot <- function(a, b) {
  sqrt(a^2 + b^2)
}

fnvec <- function(x) { sum(x) } # Add up components of vector
fnlist <- function(l) { # Return vector with 1 added to each component
  vec <- c(l$alpha + 1, l$beta + 1, l$gamma + 1)
  vec
}

result <- RcppExample(params, nlist, numvec, nummat, df, datevec,
                      datetimevec, stringvec, func, hypot,
                      zoots, myfactor)                       
result

}

\keyword{models}

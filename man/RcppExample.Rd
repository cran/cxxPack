\name{RcppExample}
\alias{RcppExample}
\title{Rcpp R/C++ interface example}
\description{
  \code{RcppExample} illustrates how the \code{Rcpp} R/C++
  interface class library is used.
}
\usage{
RcppExample(params, nlist, vec, mat)
}
\arguments{
  \item{params}{A parameter list specifying \code{method} (string),
    \code{tolerance} (double), \code{maxIter} (int)}
  \item{nlist}{a list of named numeric values (double or int)}
  \item{vec}{a numeric 1D vector (double or int)}
  \item{mat}{a numeric 2D matrix (double or int)}
  }
\value{
  \code{RcppExample} returns a list containing:
  \item{nlFirstName}{first name in nlist}
  \item{nlFirstValue}{first value in nlist}
  \item{matD}{an RcppMatrix<double> object}
  \item{vecD}{a RcppVector<double> object}
  \item{a}{A matrix in C/C++ format}
  \item{v}{A vector in C/C++ format}
  \item{settleDays}{An integer-valued parameter}
  \item{price}{A real-valued parameter}
  \item{riskFreeRate}{A real-valued parameter}
  \item{dividendYield}{A real-valued parameter}
  \item{params}{The input parameter list}
}
\details{
  See the document \code{Rcpp: R/C++ Interface Classes: Using C++
    Libraries from R} for more information. The source files for this example
    can be found in the  \code{doc} package subdirectory, and the source code
    for the \code{Rcpp} library can be found in the \code{lib}
    subdirectory.
}

\author{Dominick Samperi}

\examples{

params <- list(optType='call',
	       tradeDate=c(5,15,1998),
	       settleDays=2,
	       exerciseDate=c(5,17,1999),
	       strikePrice=8,
	       price= 7,
	       volatility=0.10,
	       riskFreeRate=0.05,
	       dividendYield=0.05)

nlist <- list(ibm = 80.50, hp = 53.64, c = 45.41)

vec <- seq(1,5)

mat <- matrix(seq(1,20),4,5)

result <- RcppExample(params, nlist, vec, mat)

result

}
\keyword{models}

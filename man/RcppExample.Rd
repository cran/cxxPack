\name{RcppExample}
\alias{RcppExample}
\alias{print.RcppExample}
\title{Rcpp R/C++ interface example}
\description{
  \code{RcppExample} illustrates how the \code{Rcpp} R/C++
  interface class library is used.
}
\usage{
RcppExample(params, nlist, vec, mat, df)
print.RcppExample()
}
\arguments{
  \item{params}{A heterogeneous list specifying \code{method} (string),
    \code{tolerance} (double), \code{maxIter} (int)}
  \item{nlist}{a list of named numeric values (double or int)}
  \item{vec}{a numeric 1D vector (double or int)}
  \item{mat}{a numeric 2D matrix (double or int)}
  \item{df}{a data frame}
  }
\details{

  The C++ represention of data frames are not passed back to R in a form
  that R recognizes as a data frame, but it is a simple matter to do the
  conversion. For example, the return value named \code{PreDF} (see
  return values below) is not seen as a data frame on the R side (thus
  the name "pre-data frame"), but it can be converted to a data frame
  using \code{df <- data.frame(result$PreDF)}.
  
  The print.RcppExample() function is defined so that we can control
  what gets printed when a variable assigned the return value is entered
  on a line by itself. It is defined to simply list the names of the
  fields returned (see RcppExample.R).

  The source files for this
  example and for the \code{Rcpp} class library
  can be found in the RcppTemplate package source archive (the .tar.gz file).
}
\value{
  \code{RcppExample} returns a list containing:
  \item{method}{string input paramter}
  \item{tolerance}{double input paramter}
  \item{maxIter}{int input parameter}
  \item{nlFirstName}{first name in nlist}
  \item{nlFirstValue}{first value in nlist}
  \item{matD}{R matrix from an RcppMatrix<double> object}
  \item{stlvec}{R vector from a vector<double> object}
  \item{stlmat}{R matrix from a vector<vector<double> > object}
  \item{a}{R matrix from C/C++ matrix}
  \item{v}{R vector from C/C++ vector}
  \item{strings}{R vector of strings from vector<string> object}
  \item{InputDF}{a data frame passed in from R}
  \item{PreDF}{a data frame created on C++ side to be passed back to R}
  \item{params}{input parameter list (this is redundant because we returned the input parameters above)}
  }

\author{Dominick Samperi}

\references{
  Samperi, D., (2006) \emph{Rcpp: R/C++ Interface Classes:
      Using C++ Libraries from R},
    available as a package vignette, or in the \code{RcppTemplate}
    package \code{doc} subdirectory
  (\code{RcppAPI.pdf}).
    
  \emph{Writing R Extensions}, available at \url{http:www.r-project.org}.
}
\examples{

params <- list(method='BFGS',
               tolerance=1.0e-8,
               maxIter=1000)

nlist <- list(ibm = 80.50, hp = 53.64, c = 45.41)

vec <- seq(1,5)

mat <- matrix(seq(1,20),4,5)

df <- data.frame(a=c(TRUE, TRUE, FALSE), b=I(c('a','b','c')), c=c('beta', 'beta', 'gamma'))

result <- RcppExample(params, nlist, vec, mat, df)

result

}

\keyword{models}

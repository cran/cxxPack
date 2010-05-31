\name{cgamma}
\alias{cgamma}
\title{Complex gamma function}
\description{
  Computes the complex gamma function.
}
\usage{
cgamma(z)
}
\arguments{
  \item{z}{a vector or matrix of numeric or complex values}
}
\details{
  Computes the complex gamma function using the Lanczos approximation
  (see Wikipedia).
}
\value{
  Returns a vector or matrix of complex values of the gamma function
  corresponding to the input numeric or complex values.
}
  
\author{Dominick Samperi}

\examples{
m <- matrix(1:12,3,4)
cgamma(m)
}

\keyword{models}

\name{MyTest}
\alias{MyTest}
\title{A simple test function}
\description{
  Sample function defined by code in \code{src} and \code{man}.
  }
  \usage{
MyTest(x=2.0, df=data.frame(a=c(1,2,3),b=c(4,5,6)))
}
\arguments{
  \item{x}{a numeric value}
  \item{df}{a data frame (with at lease one column)}
  }
  \details{
    Details about MyTest.
}
\value{
  A list containing computed results.
  }

\examples{
x <- 2.0
df <- data.frame(a=c(1,2,3,4), b=as.Date('2010-04-15') + 1:4)
MyTest(x, df)
}

\keyword{models}

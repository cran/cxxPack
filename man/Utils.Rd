\name{demoutil}
\alias{demoutil}
\alias{demomenu}
\title{Menu interface to package demos}
\description{
  Instead of running package demos individually using \code{demo(name)}
  the function \code{demomenu()} can be used to bring up a menu
  interface for all demos that are part of the specified package.
}
\usage{
demomenu(packageName)
}
\arguments{
\item{packageName}{the string name of a package.}
}
\details{
  Demos are selected by number from the menu.

  If 0 is entered a list of demos plus descriptions is displayed.

  Any other input terminates.
}
\value{
  demos normally do not return values (but can cause side-effects if
  not enclosed in a function or local environment).
}
  
\author{Dominick Samperi}

\examples{
# Bring up demo menu for \code{cxxPack}
demomenu('cxxPack')
}

\keyword{models}

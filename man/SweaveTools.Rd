\name{SweaveTools}
\alias{loadcppchunk}
\alias{unloadcppchunk}
\title{Tools used to add C++ support to Sweave}
\description{
  The function \code{loadcppchunk} compiles and loads a chunk of
  C++ code in an Sweave document (vignette). \code{unloadchunk} unloads
  a previously loaded chunk.
}
\usage{
loadcppchunk(name,compile=TRUE,quiet=TRUE)
unloadcppchunk(name)
}
\arguments{
\item{name}{root name of \code{name.cpp.tex}.}
\item{compile}{if TRUE, compile \code{.cpp} file. if FALSE, assume that
  it has already been compiled, just load.}
\item{quiet}{if TRUE do not show compile commands.}
}
\details{
  This functions is normally called from R code chunks in a vignette.
  
  All C++ source files that are part of the Sweave document are
  in the subdirectory \code{inst/doc/cpp}.

  Note that a separate shared library will be created for each
  code chunk, and these libraries will be large if static linking
  is enabled (disk space is cheap, and the libs can be deleted after
  generating the finished document).
}
\value{
  No return values.
}
\references{Samperi, D. (2010), \emph{cxxPack Users Guide}}
  
\author{Dominick Samperi}

\keyword{misc}

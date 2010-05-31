\name{HillPlot}
\alias{HillPlot}
\title{Plots Hill Tail Estimates}
\description{
  Uses an unbiased variant of Hill's estimator to approximate the exponent
  alpha for a distribution with a power-law tail.
}
\usage{
HillPlot(x, start = 15, end = NA, nskip = 1,
         title = "Hill Tail Estimates",...)}
\arguments{
  \item{x}{Vector of values to estimate tail for.}
  \item{start}{Start with this many order statistics.}
  \item{end}{Finish with this many order statistics.}
  \item{nskip}{Number of points to skip to thin out plot.}
  \item{title}{Title for plot.}
  \item{...}{Other parameters for plot.}
}
\details{
  The objective is to estimate \eqn{\alpha}{alpha} when the
  data comes from a distribution with a heavy right tail, more
  precisely, when the right tail approximately satisfies
  \deqn{P[X > x] = C x^{-\alpha},}{P[X > x] = C x^(-alpha),}
  for sufficiently large (positive) x.

  There is some variation in the literature on the definition of Hill's
  estimator. We use the unbiased estimator studied in
  Aban and Meerschaert (2004), namely, we estimate 1/alpha using
  \deqn{\frac{1}{r-1}\sum_{i=1}^{r-1} (\log X_{(n-i+1)} - \log
    X_{(n-r+1)}),}{1/(r-1) sum\_1^(r-1) (log X\_(n-i+1) - log
    X\_(n-r+1)),}
  where \eqn{X_{(i)}}{X\_(i)} is the i-th order statistic (i-th largest
  value), and \code{r} is the number of order statistics that is used
  for each estimate. (Note that while the sum contains only \code{r-1}
  terms, it involves \code{r} order statistics.)

  If the data vector includes negative values they are deleted before
  the analysis begins, so the first order statistic is always
  positive (and larger order statistics are obviously not changed).
  To focus on the process obtained by using a higher threshold
  simply replace the vector parameter \code{x} with
  \code{x[x>cutoff]}. Of course, this will only work if there are values
  larger than \code{cutoff} in \code{x}, and the results obtained will
  depend on the cutoff selected (still potentially useful for
  reinsurance applications, for example).

  This estimator is obtained using linear regression methods in
  Aban and Meerschaert (2004). The idea of estimating the tail in
  this way was introduced in Mandelbrot (1963).
}
\value{
  No return values.
}
\author{Dominick Samperi}
\references{
  Hill, B. (1975), \emph{A simple general approach to inference about
    the tail of a distribution}, Annals of Statistics, 3, 1163--1173.

  Mandelbrot, B. B. (1963), \emph{The variation of certain speculative
    prices}, Journal of Business, 36, 394--419.
  
  Aban, I. B., Meerschaert, M. M. (2004), \emph{Generalized
    least-squares estimators for the thickness of heavy tails},
  Journal of Statistical Planning and Inference, 119,
  341--352.

  Embrechts, P., Kluppelberg, C., Mikosch, T. (1997), \emph{Modelling
    Extremal Events}, Spinger-Verlag, New York.
  }
  \examples{
  HillPlot(rnorm(1000))
}
\keyword{misc}

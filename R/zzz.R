.onLoad <- function(libname, pkgname) {
  #cat('Loading: ', pkgname, '\n')
}

.onUnload <- function(lib,pkgname) {
}

# A version of seq() that returns c() when there are no steps possible.
# Use this to get the normal (C/C++) behavior of for-loops: nothing happens when
# starting index is already beyond the stopping index.
seqStrict <- function(beg, end, step=1) {
  if((beg > end && step > 0) || (beg < end && step < 0))
    val <- c() # null vector
  else
    val <- seq(beg,end,step)
  val
}

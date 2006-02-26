RcppExample <- function(params, nlist, vec, mat) {

# Most of the input parameter checking here is not really
# necessary because it is done in the Rcpp code.
  
  # Check that params is properly formatted.
  if(!is.list(params) || length(params) == 0) {
    stop("The params parameter must be a non-empty list");
  }

  # Check nlist
  if(!is.list(nlist) || length(nlist) == 0) {
    stop("The nlist parameter must be a non-empty list");
  }
  if(length(nlist) != length(names(nlist))) {
    stop("The values in nlist must be named")
  }
  if(!is.numeric(unlist(nlist))) {
    stop("The values in nlist must be numeric")
  }

  # Check vec argument
  if(!is.vector(vec)) {
    stop("vec must be a vector");
  }

  # Check mat argument
  if(!is.matrix(mat)) {
    stop("mat must be a matrix");
  }
  
  # Finally ready to make the call...
  val <- .Call("Rcpp_Example", params, nlist, vec, mat,
               PACKAGE="RcppTemplate")
  val
}

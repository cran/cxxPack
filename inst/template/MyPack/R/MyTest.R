# Trivial example of a function that defines an S3 class that
# has its own print method (defined below). Both must be
# declared in MyPack/NAMESPACE.
MyTest <- function(x=2.0, df=data.frame(a=c(1,2,3),b=c(4,5,6))) {
  res <- .Call('My_Test', x, df, PACKAGE='MyPack')
  class(res) <- "MyTest"
  res
}

print.MyTest <- function(x,...) {
  cat('x = ', x$x, '\n')
  cat('sqrtx = ', x$sqrtx, '\n')
  cat('\ndf1:\n')
  print(x$df1) # uses print.data.frame()
  cat('\ndf2:\n')
  print(x$df2)
}


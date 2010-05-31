# Unit tests for MyPack
#

# Normally you would do something non-trivial here...
test.myfunc1 <- function() {
  x <- sqrt(2)
  checkEquals(x, sqrt(2))
}

test.myfunc2 <- function() {
  y <- sin(1)
  checkEquals(y, sin(1))
}


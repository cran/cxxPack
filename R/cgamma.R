# Complex gamma function (z can be a matrix)
cgamma <- function(z) {
  .Call('cgamma_', z, PACKAGE='cxxPack')
}

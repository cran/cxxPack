The file EquityOption.cpp is from the Examples directory in the QuantLib
distribution. Only the line containing main() was changed, and the
return values were changed to R_NilValue.

In this way the example can be called from R. To do this first install
the QuantLib and Boost libraries. Then copy Makevars.in, Makevars.win,
and EquityOption.cpp from this directory to RcppTemplate/src, and uncomment
the lines in src/RcppExample.cpp that call the EquityOption() function.

Finally, build the modified package and run RcppExample via
example(RcppExample). See the Rcpp User's Guide for more information.

The RQuantLib package automates some of this and provides a more
comprehensive example.

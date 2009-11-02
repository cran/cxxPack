EquityOption.cpp is an example from the QuantLib distribution. The main()
declaration has been replaced with RcppExport SEXP EquityOption(), and
the return value has been changed to R_NilValue.

In this way this example can be called from R. To do this first install
the QuantLib and Boost libraries. Then copy Makevars.in, Makevars.win,
and EquityOption.cpp from this directory to RcppTemplate/src, and uncomment
the lines in src/RcppExample.cpp that call the EquityOption() function.

Finally, build the modified package and run RcppExample via
example(RcppExample). See the Rcpp User's Guide for more information.

The RQuantLib package automates some of this and provides a more
comprehensive example.

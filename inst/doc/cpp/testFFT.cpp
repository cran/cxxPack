#include <cxxPack.hpp>
/**
 * Calls R's fft() with step function input.
 * Also works with Rcpp::ComplexVector.
 */
RcppExport SEXP testFFT() {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    int N = 128;
    double u0 = -1.5, du = 3.0/N, dx=2*3.14159265/N/du, x0 = -N*dx/2;
    std::vector<double> u(N);
    std::vector<double> x(N);
    std::vector<std::complex<double> > f(N); // f(u) = 1 on [-.5,.5]
    int fac = 1;
    for(int i=0; i < N; ++i) {
	u[i] = u0 + i*du;
	x[i] = x0 + i*dx;
	f[i].real() = fac * ((u[i] >= -0.5 && u[i] < 0.5) ? 1.0 : 0.0);
	fac = -fac;
	f[i].imag() = 0.0;
    }
    std::vector<std::complex<double> > cresult = cxxPack::fft1d(f);
    std::vector<double> result(N);
    fac = (N/2 % 2 == 0) ? 1 : -1;
    for(int i=0; i < N; ++i) {
	result[i] = fac*du*cresult[i].real();
	fac = -fac;
    }
    Rcpp::List rl;
    rl["x"] = Rcpp::wrap(x);
    rl["ft"] = Rcpp::wrap(result);
    ret = rl;
    END_RCPP
    return ret;
}

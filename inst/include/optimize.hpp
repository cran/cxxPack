// optimize.hpp: interface file for R's optimizer/root finder
//
// Copyright (C) 2005-2010 Dominick Samperi
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 3 as 
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef OPTIMIZE_HPP
#define OPTIMIZE_HPP

// optimize.hpp: Defs for interface to R optimizers including L-BFGS-B.
//
// Copyright (C) 2005-2010 Dominick Samperi
//
// License: GPL(>=2), see file LICENSE.

namespace cxxPack {

// Abstract function classes used for minimization and root finding.
class Function {
public:
    Function() {}
    virtual void evaluate(int n, double *x, double *f) = 0;
};

class DifferentiableFunction {
public:
    DifferentiableFunction() {}
    virtual void evaluate(int n, double *x, double *f, double *df) = 0;
};

class Function1D {
public:
    Function1D() {}
    virtual double value(double x) = 0;
};

class DifferentiableFunction1D {
public:
    DifferentiableFunction1D() {}
    virtual double value(double x) = 0;
    virtual double deriv(double x) = 0;
};

class RootFinder1D { // Wraps Brent zeroin root finder.
    const static int MAXITER;
    const static double EPS;
public:
    const static double xlo, xhi;
    RootFinder1D() {}
    double solve(Function1D& func) {
	return zeroin(func, xlo, xhi, EPS, MAXITER);
    }
    double solve(Function1D& func, double xlo_, double xhi_, 
		 double tol=EPS, int maxIter=MAXITER) {
	return zeroin(func, xlo_, xhi_, tol, maxIter);
    }
    double zeroin(Function1D& func, double x1, double x2, double tol,
		  int maxIter);
};

class ConstrainedMinimizer { // Uses L-BFGS-B minimizer.

    int n;             // Number of independent variables.
    double *xlo, *xhi; // Variable bounds.
    double *x, F, *DF; // Independent variables, function value, derivatives.

    // Working storage and parameters.
    double fnscale;
    double factr; // Accuracy: 1.e+12 low, 1.e+1 very high, 1.e+7 works ok.
   double pgtol; // Projected gradient tolerance.
    int trace, maxit, REPORT;
    double *parscale, *dpar, *wa;
    int    *iwa, *nbd, m;
public:
    ConstrainedMinimizer(int n, double *xlo, double *xhi);
    ~ConstrainedMinimizer();
    double minimize(DifferentiableFunction& func, double *x0) {
	for(int i=0; i < n; i++)
	    x[i] = x0[i];
	return minimize(func);
    }
    double minimize(DifferentiableFunction& func);
    double getF() { return F; }
    double getDF(int i) { return DF[i]; }
    double getX(int i) { return x[i]; }
    int getNumVars() { return n; }
    void setTolerance(double tol) { factr = tol; }
    void setMaxIter(int maxIter) { maxit = maxIter; }
};

} // end cxxPack namespace

#endif




// optimize.cpp: Interface to R optimizers (L-BFGS-B) and root finder.
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

#include <iostream>
#include <math.h>
#include <stdexcept>

#include <Rcpp.h>
#include <R_ext/Applic.h>

#include "optimize.hpp"

namespace cxxPack {

const int RootFinder1D::MAXITER = 1000;
const double RootFinder1D::EPS = 1.0e-8;
const double RootFinder1D::xlo = 0.0, RootFinder1D::xhi = 1000.0;

/**
 * Uses L-BFGS-B constrained minimizer (R version).
 */
ConstrainedMinimizer::ConstrainedMinimizer(int n, double *xlo, double *xhi)
    : n(n), xlo(xlo), xhi(xhi) {

    // Allocate and initialize storage.
    factr = 1.e+7; // Accuracy: 1.e+12 low, 1.e+1 very high.
    maxit = 100; // Max iterations.
    pgtol = 0; // Projected gradient tolerance.
    fnscale = 1; // Scaling for function value.
    trace = 3;   // Level of reporting.
    REPORT = 10; // Level of reporting.
    x = new double[n]; // Independent variables.
    DF = new double[n];// Derivative of function.
    dpar = new double[n]; // Scaled independent vars.
    parscale = new double[n]; // Scale factors for latter.
    nbd = new int[n]; // Type of bounds for each variable (=2)

    m = 5; // L-BFGS-B parameter (no. of variable metric corrections).
    iwa = new int[3*n]; // Integer storage used by L-BFGS-B.
    int wksize = 2*m*n + 4*n + 11*m*m + 8*m;
    wa = new double[wksize]; // Double storage used by L-BFGS-B.
    for(int i=0; i < wksize; i++)
	wa[i] = 0;
    
    for(int i=0; i < n; i++) {
	x[i] = 0.5*(xlo[i] + xhi[i]);
	DF[i] = 0;
    }
}

ConstrainedMinimizer::~ConstrainedMinimizer() {

    // Cleanup.
    delete [] wa;
    delete [] nbd;
    delete [] iwa;
    delete [] parscale;
    delete [] dpar;
    delete [] DF;
    delete [] x;
}

/**
 * Minimize a differentiable scalar function of several variables subject
 * to bounds on the independent variables. Uses L-BFGS-B software. Adapted
 * from R code src/main/optim.c (see lbfgsb function), and modified to fetch
 * function value and gradient at the same time (more efficient for our
 * purposes).
 */
double ConstrainedMinimizer::minimize(DifferentiableFunction& func) {

    int ifail = 0;
    double dsave[29];
    int tr = -1, iter = 0, isave[44], lsave[4];
    char task[60], msg[60];
    double fmin;
    int fncount, grcount;

    // Pre-process.
    for(int i=0; i < n; i++) {
	parscale[i] = 1;
	nbd[i] = 2;
    }
    for(int i=0; i < n; i++)
	dpar[i] = x[i]/parscale[i];

    switch(trace) {
    case 2: tr = 0; break;
    case 3: tr = REPORT; break;
    case 4: tr = 99; break;
    case 5: tr = 100; break;
    case 6: tr = 101; break;
    default: tr = -1; break;
    }

    strcpy(task, "START");
    while(1) {
	/* Main workhorse setulb() from ../appl/lbfgsb.c : */
	setulb(n, m, dpar, xlo, xhi, nbd, &F, DF, factr, &pgtol, wa, iwa, task,
	       tr, lsave, isave, dsave);
	// int n: Number of independent variables.
	// int m: Number of variable metric corrections (5).
	// double *x: Current approximation to solution.
	// double *l: Lower bounds of dimension n.
	// doulbe *u: Upper bounds of dimension n.
	// int *nbd:  Type of bounds of dimension n (2 = both).
	// double *f: Value of f at x.
	// double *df: Value of df at x.
	// double factr: 1.d+12 (low accuracy), 1.d+1 (very high accuracy).
	// double *pgtol: max(proj g_i) <= pgtol.
	// double *wa: working array of size 2*m*n + 4*n + 11*m*m + 8m.
	// int *iwa: working array of size 3*n.
	// task: string of length 60 used to control job.
	// int tr (iprint): <0 means no output
	//      iprint=0	print only one line at the last iteration.
	//	0<iprint<99	print also f and |proj g| every iprint iter.
	//	iprint=99	print details of every iteration except n-vecs.
	//	iprint=100	print also changes of active set and final x.
	//	iprint>100	print details of every iteration including x/g.
	// lsave: logical (int) array of dim 4 (output info...see docs)
	// isave: int array of dim 44 (ouput info)
	// dsave: double working array of dim 29 containing output info.
	if (strncmp(task, "FG", 2) == 0) {
	    func.evaluate(n, dpar, &F, DF);
	    if(!R_FINITE(F))
		throw std::range_error("Non-finite func. value in L-BFGS-B");
	} else if (strncmp(task, "NEW_X", 5) == 0) {
	    if(trace == 1 && (iter % REPORT == 0)) {
		Rprintf("iter %4d value %f\n", iter, F);
	    }
	    if (++iter > maxit) {
		ifail = 1;
		break;
	    }
	} else if (strncmp(task, "WARN", 4) == 0) {
	    Rprintf("WARN: %s\n", task);
	    ifail = 51;
	    break;
	} else if (strncmp(task, "CONV", 4) == 0) {
	    Rprintf("CONV: %s\n", task);
	    break;
	} else if (strncmp(task, "ERROR", 5) == 0) {
	    Rprintf("ERROR: %s\n", task);
	    ifail = 52;
	    break;
	} else { /* some other condition that is not supposed to happen */
	    Rprintf("UNKNOWN ERROR: %s\n", task);
	    ifail = 52;
	    break;
	}
    }
    fmin = F;
    fncount = grcount = isave[33];
    if (trace) {
	if (iter < maxit && ifail == 0) Rprintf("converged\n");
	else Rprintf("stopped after %i iterations\n", iter);
    }
    strcpy(msg, task);

    // Post-process.
    for(int i=0; i < n; i++)
	x[i] = dpar[i] * parscale[i];
    double value = fmin * fnscale;

    // Output: ifail, fncount, grcount ignored for now.

    return value;
}

/**
 * C function call wrapper for Function1D objects.
 */
extern "C" {
    static double callFunction1D(double x, void *fp) {
	return ((Function1D *)fp)->value(x);
    }
}

/**
 * Wrapper to call Brent's zeroin via R's R_zeroin.
 */
double RootFinder1D::zeroin(Function1D& func, double x1, double x2, 
			    double tol, int maxIter)
{
    double tolerance = tol;
    int iter = maxIter;
    if(x1 >= x2)
	throw std::range_error("RootFinder1D:zeroin: xlo must be less than xhi");
    else if(func.value(x1)*func.value(x2) >= 0) {
	std::ostringstream oss;
	oss << "RootFinder1D::zeroin: roots not bracketed:\n";
	oss << "f(xmin = " << x1 << ") = " << func.value(x1) << "\n";
	oss << "f(xmax = " << x2 << ") = " << func.value(x2);
	throw std::range_error(oss.str());
    }
	    
    double val = R_zeroin(x1, x2, callFunction1D, (void *)&func, 
			  &tolerance, &iter);
    if(iter < 0) {
	std::ostringstream oss;
	oss << "RootFinder1D::zeroin: no convergence after " << MAXITER;
	oss << " iterations";
	throw std::range_error(oss.str());
    }
    return val;
}

} // end cxxPack namespace

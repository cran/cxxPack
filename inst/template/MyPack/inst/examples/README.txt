* Configuring to use external libraries (like FFTW)

To explain how to link against external libraries it may be helpful
to work with a particular case, the optimized Fast Fourier Transform
library FFTW. This library can be downloaded from http://www.fftw.org
(or installed using the OS package installer in the case of RedHat
Linux, for example).

There are two ways to specfy this library. The simplest is to
set PKG_EXTRACPP and PKG_EXTRALIB in YourPack/src/Makevars and
YourPack/src/Makevars.win. See the commented lines in these files
for an example. Note that in the Windows case an environment
variable is used to specify the include file and library
directory (the same directory in the case of this library; for
other libraries we might specify -I$(LIB_JPEG)/include, say).

This environment variable must be defined before the build
process is run. In the case of LIB_FFTW (and LIB_JPEG) it turns
out that these environment variables are defined by CRAN, so
it is possible to release packages with dependencies on these
libraries for which CRAN can generate binaries (see the comments
in Makevars.win).

(Incidentally, a simple interface to the FFTW library from 
both C++ and R is implemented in fftw.{cpp|hpp} in the 
CreditRiskPack package, where an application can also be found. 
Why use this library since R has a built-in FFT? Some say it is 
because FFTW is faster, but I have not checked. Currently 
the biOps package uses FFTW; maybe the authors have checked.)

The advantage of this method is that it works for both Linux
and Windows (and MacOS since it is like Linux), and if CRAN
defines the environment variable, it works there as well.

The second method involves the use of the UNIX/Linux autoconf
tool together with a configure.in file. Here a configure script
is used to automatically detect required libs and header files,
and based on this information, it makes variable substitutions
in a file named Makevars.in, with output file Makevars. Note
that this method only applies to Linux (and MacOS), not to Windows.

Example configure.in and Makevars.in files that are configured
to detect FFTW libs and headers can be found in YourPack/inst/examples.
To use them copy configure.in to
YourPack/configure.in and copy Makevars.in to
YourPack/src/Makevars.in. Then cd to YourPack and run autoconf.
This will generate a configure script. The options available can
be viewed by typing ./configure --help. 

But configure is normally run by the R CMD INSTALL command, not by 
the user directly. To specify configure options through 
R CMD INSTALL use something like this:

R CMD INSTALL --configure-args=-with-fftw-include=/usr/share/fftw YourPack

Working with configure.in files and autoconf is somewhat
obscure. Furthermore, it does not work under Windows.
Thus it is probably better to use the first method described
above.

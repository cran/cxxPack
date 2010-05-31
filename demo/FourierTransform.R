# Computes the FFT of the unit step function supported on [-.5,.5] using
# fft from R and from C++, and compares with the exact answer 2 sin(x/2)/x.
N = 128 # number of samples
u0 = 1.5
u <- c(-u0,-.5,.5,u0)
y <- c(0,1,0,0)
f <- approxfun(u,y,method='constant') # unit step supported on [-.5,.5]
du = 3.0/N
dx = 2*pi/N/du
u = -u0 + du*(0:(N-1))
fac <- c(1,-1); fac <- rep(fac,N/2)
fac1 = ifelse(as.integer(N/2) %% 2 == 0,1,-1)
#plot(u, f(u),type='l',main='f(x)')
x0 = N*dx/2
x = -x0 + dx*(0:(N-1))
ftdirect <- du*fac1*fac*fft(fac*f(u))
ftindirect <- du*fac1*fac*.Call('fft1d',fac*f(u),FALSE) # testing, not recommended.
ftexact <- ifelse(x==0,1.0,2*sin(x/2)/x)
oldpar = par(mfrow=c(3,1))
plot(x,Re(ftdirect),type='l',col='blue',ylim=c(-.5,1.0),main='FFT of step from R')
plot(x,Re(ftindirect),type='l',col='red',ylim=c(-.5,1.0),main='FFT of step from C++')
plot(x,ftexact,type='l',col='green',ylim=c(-.5,1.0),main='Exact Fourier Transform')
par(oldpar)

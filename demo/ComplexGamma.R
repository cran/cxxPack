#pdf('cgamma.pdf')

  complexify <- function(x,y) {
    complex(real=x, imaginary=y)
  }
  Nreal <- 50
  Nimag <- 100
  rl <- seq(-4,4,length.out=Nreal)
  im <- seq(-2,2,length.out=Nimag)
  z <- outer(rl, im, complexify)
  gamma <- cxxPack::cgamma(z)

  #There is a cgamma() in fAsianOptions package but it drops the dims.
  #gamma <- fAsianOptions::cgamma(z)
  #dim(gamma) <- c(length(rl), length(im))

  persp(rl, im, abs(gamma),ticktype='detailed',theta=-20,
        main='Modulus of Complex Gamma Function',col='cyan',
        xlab='real',ylab='imag',zlab='|Gamma(z)|')

#dev.off()

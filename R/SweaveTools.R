# Used to dynamically compile and load C++ functions in a vignette.
loadcppchunk <- function(name, compile=TRUE, quiet=TRUE) {
  if(compile) {
    # Under Windows this should use Rtools/bin/sh.exe
    # quiet mode does not work for Windows
    xstat = ifelse(quiet==TRUE && .Platform$OS.type != "windows",
            system(paste('sh ./makedll.sh ', name, ' > /dev/null',sep="")),
            system(paste('sh ./makedll.sh ', name, sep="")))
    if(xstat) {
      stop(paste('loadcppchunk() failed for ',name,'\n'))
    }
  }
  dyn.load(paste('./cpp/', name,.Platform$dynlib.ext,sep=""))
}

unloadcppchunk <- function(name) {
  dyn.unload(paste('./cpp/', name, .Platform$dynlib.ext,sep=""))
}


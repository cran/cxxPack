# Used to dynamically compile and load C++ functions in a vignette.
loadcppchunk <- function(name, compile=TRUE, logfile="compile.log") {
  if(compile) {
    # Under Windows this should use Rtools/bin/sh.exe
    xstat = system(paste('sh ./makedll.sh ', name, ' "', logfile, '"',sep=""))
    if(xstat) {
      stop(paste('loadcppchunk() failed for ',name,'\n'))
    }
  }
  dyn.load(paste('./cpp/', name,.Platform$dynlib.ext,sep=""))
}

unloadcppchunk <- function(name) {
  dyn.unload(paste('./cpp/', name, .Platform$dynlib.ext,sep=""))
}


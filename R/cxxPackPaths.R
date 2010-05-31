# Defines functions that return OS/system-dependent compiler and linker
# flags needed for use with cxxPack and Rcpp. This is an adaptation
# of RcppLdpath.R from the Rcpp package.

# cxxPack:::CxxFlags() returns the flags needed to link against BOTH
# cxxPack and Rcpp. Rcpp:::CxxFlags() is not needed. (::: is required
# instead of :: because these functions are not exported).

# Similarly, cxxPack:::LdFlags() returns linker flags needed for BOTH
# cxxPack and Rcpp.

# Tools from Rcpp package...
staticLinking <- Rcpp:::staticLinking # only Linux defaults to dynamic
sysfile <- function(...) { # system.file() with tilde expansion
  tools:::file_path_as_absolute(base:::system.file(...,package='cxxPack'))
}
Cxx0xFlags <- Rcpp:::Cxx0xFlags # flags when Cxx0x is used
RcppCapabilities <- Rcpp:::RcppCapabilities # based on GNU compiler version?

# Get the path to the lib directory for this package.
cxxPackLdPath <- function() {
    if (nzchar(.Platform$r_arch)) { ## eg amd64, ia64, mips
        path <- sysfile("lib",.Platform$r_arch)
    } else {
        path <- sysfile("lib")
    }
    path
}

# Linker flags (cxxPack only)
cxxPackLdFlags <- function(static=staticLinking()) {
    cxxPackdir <- cxxPackLdPath()
    if (static) {
      flags <- paste(cxxPackdir, "/libcxxPack.a", sep="")
    } else { # dynamic linking (Linux default)
      flags <- paste(" -L", cxxPackdir, " -lcxxPack", sep="")
        if ((.Platform$OS.type == "unix") &&
            (length(grep("^linux",R.version$os)))) {
            flags <- paste(flags, " -Wl,-rpath,", cxxPackdir, sep="")
        }
    }
    invisible(flags)
}

# Compiler flags (cxxPack only)
cxxPackCxxFlags <- function(cxx0x=FALSE) {
  path <- sysfile("include")
  paste("-I", path, if(cxx0x && Rcpp:::canUseCXX0X()) " -std=c++0x" else "",sep="")
}

# Concatenate cxxPack and Rcpp compiler flags.
CxxFlags <- function(cxx0x=FALSE) {
    cat(paste(cxxPackCxxFlags(cxx0x=cxx0x),
              Rcpp:::RcppCxxFlags(cxx0x=cxx0x),
              sep=" "))
}

# Concatenate cxxPack and Rcpp linker flags.
LdFlags <- function(static=staticLinking()) {
    cat(paste(cxxPackLdFlags(static=static), Rcpp:::RcppLdFlags(static=static),sep=" "))
}






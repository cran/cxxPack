# Displays PDF file as an R demo
#
pkgName <- 'RcppTemplate'
pkgDir  <- 'doc'
pdfFile <- 'RcppAPI.pdf'
isWindows <- (Sys.info()['sysname'] == 'Windows')
pkgLoc <- system.file(".", ".", package=pkgName)
pkgLoc <- substring(pkgLoc, 1, nchar(pkgLoc)-nchar(pkgName)-5)
file <- system.file(pkgDir, pdfFile, package=pkgName,lib.loc=pkgLoc)
if(isWindows) { # Windows automatically finds executable based on file type.
  system(paste("CMD /C ", file, "\n"))
} else { # Change this to use path to Adobe reader if desired.
  system(paste(options("pdfviewer"), file, "\n"))
}




# Displays RcppDoc.pdf file
#
showRcppDoc <- function() {
  file <- system.file('doc', 'RcppDoc.pdf', package='RcppTemplate')
  system(paste(options("pdfviewer"), file),wait=FALSE)
}


# Displays QuickReference.txt
#
showRcppQuickRef <- function() {
  file <- system.file('doc', 'QuickReference.txt', package='RcppTemplate')
  file.show(file)
}




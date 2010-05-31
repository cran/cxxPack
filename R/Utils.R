# Bring up a menu interface for the demos of a package.

demomenu <- function(packageName) {
  demoCode <- demo(package=packageName)$results[,3]
  if(length(demoCode) == 0) {
    return('No demos found for package ', packageName, '\n')
  }
  while(TRUE) {
    for(i in seq(1,length(demoCode))) {
      cat(i, ': ', demoCode[i], '\n')
    }
    suppressWarnings(demoNum <- as.integer(readline(prompt="Enter demo number to run [0 to view descriptions, RETURN to exit]: ")))
    
    if(is.na(demoNum)) { # didn't get a number, exiting
      break
    }
    else if(demoNum == 0) {
      print(demo(package=packageName))
    }
    else {
      if(demoNum < 0 || demoNum > length(demoCode)) {
        cat('Number out of range, try again...\n')
        next
      }
      demo(demoCode[demoNum], character.only=TRUE)
    }
  }
}


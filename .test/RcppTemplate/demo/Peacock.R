if(require(ReadImages)) {
  file <- system.file("images", "peacock.jpg", package="RcppTemplate")
  image <- read.jpeg(file)
  plot(image, ask=FALSE)
}

# Unbiased Hill tail estimator following Aban and Meerschaert (2004).
HillPlot <- function (x, start = 15, end = NA, nskip = 1,
                      title = "Hill Tail Estimates",...) {
  ordered <- rev(sort(x)) # Reverse sort.
  ordered <- ordered[ordered > 0] # Positive order statistics.
  n <- length(ordered)     # Number of positive order statistics.
  logstats <- log(ordered) # Log of order statistics.
  avesumlog <- cumsum(logstats)/(1:n) # Cumulative averages.
  alphahat <- 1/(avesumlog[1:(n-1)] - logstats[2:n]) # Unbiased estimate.
  numstats <- 2:n # Number of order stats used for each element of alphahat.
  n <- n-1 # We lost one element due to shift.
  if (is.na(end))
    end = n
  else
    end <- min(end,n)
  index <- start:end
  alphahat <- alphahat[index]
  numstats <- numstats[index]
  n <- length(alphahat)
  yrange = range(alphahat)
  alphahat <- alphahat[seq(1,n,nskip)] # Thin out sample if nskip > 1.
  numstats <- numstats[seq(1,n,nskip)]
  plot(numstats, alphahat, ylim = yrange, type = "l",
       main = title, xlab = "Order Statistics", ylab = "alpha",...)
}


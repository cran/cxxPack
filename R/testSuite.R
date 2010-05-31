# Test suite for serialNumber function
testsuite.math <- defineTestSuite("math",
         dirs = system.file('unitTests', package='cxxPack'),
         #dirs = "/u/dev/stats/library.test/cxxPack/unitTests",
         testFileRegexp = "^runit.+\\.[Rr]$",
         testFuncRegexp = "^test.+")

runcxxPackTests <- function() {
  runTestSuite(cxxPack:::testsuite.math)
}

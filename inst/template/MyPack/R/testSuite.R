
# Note: this function is NOT exported in NAMESPACE to limit conflicts with
# other packages. Thus use: runTestSuite(MyPack:::testsuite.math)
testsuite.math <- defineTestSuite("math",
         dirs = system.file('unitTests',package='MyPack'),
         testFileRegexp = "^runit.+\\.[Rr]$",
         testFuncRegexp = "^test.+")





shared:
	g++ -fpic -c cpptest.cpp
	g++ -shared -o foo.so cpptest.o
	gcc -o foo ctest.c foo.so

static:
	g++ -c -fpic cpptest.cpp
	g++ -c ctest.c
	g++ -o foo ctest.o cpptest.o

clean:
	rm -f *.o *.so *~

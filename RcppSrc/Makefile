include ${R_HOME}/etc${R_ARCH}/Makeconf
SRC = Rcpp.cpp Rcpp.hpp
OBJ = Rcpp.o
LIB = libRcpp.a

#To use shared library add location of libRcpp.so to LD_LIBRARY_PATH
#LIB = libRcpp.so

.PHONY: $(SHLIB) all

all: $(LIB)

clean:
	rm -f $(OBJ) $(LIB)

$(OBJ): $(SRC)

$(LIB): $(OBJ)
	ar crs $(LIB) $(OBJ)
#	$(SHLIB_LINK) $(OBJ) -o $(LIB)
#	cp $(LIB) ../inst/libs

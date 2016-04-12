CXX			=	mpicxx
CC          =   mpicc
CFLAGS      =   -fopenmp -g -O2 -Wall -Werror -DJACOBY_SYNC -std=gnu++11
LD          =   mpicc
LDFLAGS     =   -fopenmp -lm

all: tests Step.o

tests: matrix-test

matrix-test: matrix-test.o
	$(CXX) $(LDFLAGS) -o $@ $^

matrix-test.o: Matrix-test.cpp Matrix.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

Step.o: Step.cpp Step.hpp Net.hpp Matrix.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf matrix-test *.o

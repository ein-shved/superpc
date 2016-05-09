CXX			=	mpicxx
CC          =   mpicc
CFLAGS      =   -fopenmp -g -O2 -Wall -Werror -DJACOBY_SYNC -std=gnu++11
LD          =   mpicc
LDFLAGS     =   -fopenmp -lm

all: tests Step.o

tests: matrix-test step-test exchanger-test mpi-test

matrix-test.o: Matrix-test.cpp Matrix.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

matrix-test: matrix-test.o
	$(CXX) $(LDFLAGS) -o $@ $^


Step.o: Step.cpp Step.hpp Net.hpp Matrix.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

step-test.o: Step-test.cpp Step.hpp Matrix.hpp Net.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

step-test: step-test.o Step.o
	$(CXX) $(LDFLAGS) -o $@ $^


Exchanger.o: Exchanger.cpp Exchanger.hpp Step.hpp Net.hpp Matrix.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

exchanger-test.o: Exchanger-test.cpp Exchanger.hpp Step.hpp Matrix.hpp Net.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

exchanger-test: exchanger-test.o Exchanger.o Step.o
	$(CXX) $(LDFLAGS) -o $@ $^

MPI.o: MPI.cpp MPI.hpp MPI.hpp MPI.hpp MPI.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

mpi-test.o: MPI-test.cpp MPI.hpp Exchanger.hpp Step.hpp Matrix.hpp Net.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

mpi-test: mpi-test.o Exchanger.o Step.o MPI.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -rf matrix-test *.o

CXX			=	mpicxx
CC          =   mpicc
CFLAGS      =   -fopenmp -g -O2 -Wall -Werror -DJACOBY_SYNC -std=gnu++11
LD          =   mpicc
LDFLAGS     =   -fopenmp -lm
METIS		= 	-lmetis

all: decompositor calculation

tests: matrix-test step-test exchanger-test mpi-test jacoby-test

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

Manifest.o: Manifest.cpp Manifest.hpp MPI.hpp Exchanger.hpp Step.hpp Matrix.hpp Net.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

Jacoby-test.o: Jacoby-test.cpp Jacoby-Holes.hpp Hole.hpp Jacoby.hpp Manifest.hpp MPI.hpp Exchanger.hpp Step.hpp Matrix.hpp Net.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

jacoby-test: Jacoby-test.o Manifest.o Exchanger.o Step.o MPI.o
	$(CXX) $(LDFLAGS) -o $@ $^

main.o: main.cpp Jacoby-Holes.hpp Hole.hpp Jacoby.hpp Manifest.hpp MPI.hpp Exchanger.hpp Step.hpp Matrix.hpp Net.hpp Heat.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

main: main.o Manifest.o Exchanger.o Step.o MPI.o
	$(CXX) $(LDFLAGS) -o $@ $^

heat.o: Heat.cpp Heat.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

heat: Heat.o
	$(CXX) $(LDFLAGS) -o $@ $^

Params.o: Params.cpp Params.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

decompositor.o: decompositor.cpp Decompositor.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

decompositor: decompositor.o Params.o
	$(CXX) $(LDFLAGS) $(METIS) -o $@ $^

Vertex.o : Vertex.cpp Vertex.hpp Neighbour.hpp Chunk.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

Neighbour.o : Neighbour.cpp Neighbour.hpp Vertex.hpp Chunk.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

Chunk.o : Chunk.cpp Chunk.hpp Neighbour.hpp Vertex.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

Calculation.o : Calculation.cpp Chunk.hpp Neighbour.hpp Vertex.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

calculation : Calculation.o Chunk.o Neighbour.o Vertex.o Params.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -rf matrix-test *.o

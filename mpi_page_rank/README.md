--------------------
MPI Page Rank Programming Assignment

==================

## Build instructions
1. `make` builds the code for both serial and parallel executables
1. `pagerank.serial` and `pagerank.parallel` are the executables
1. `./pagerank.serial <graph_file>` runs the serial code for a given graph file
1. `mpirun -machinefile machines -np <#p> pagerank.parallel <graph_file> <partition_file>` 
run the parallel code with given graph file and partition file
1. The header files are in [Include](include), the serial source files in [src/cpp](src/cpp)
and the mpi source files can be found in [src/mpi](src/mpi)
1. The common main file can be found at [src/main.cpp](src/main.cpp) which is compiled and
linked to create both binaries.

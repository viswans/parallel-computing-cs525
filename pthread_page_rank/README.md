PThread Page Rank Programming Assignment
--------------------

## Build instructions
1. `make` builds the code for both serial and parallel executables
1. `pagerank.serial` and `pagerank.parallel` are the executables
1. `./pagerank.serial <graph_file>` runs the serial code for a given graph file
1. `./pagerank.parallel <graph_file> <num_threads> runs the parallel code for a given 
graph file and number of threads
1. The header files are in [Include](include), the serial source files in [src/cpp](src/cpp)
and the pthread source files can be found in [src/pthread](src/pthread)
1. The common main file can be found at [src/main.cpp](src/main.cpp) which is compiled and
linked to create both binaries.
1. The files [mainSerial.cpp](src/cpp/mainSerial.cpp) and [mainPthread.cpp](src/cpp/mainPthread.cpp) contain
the contain the main functions that are called from `main()`.
1. The output of pagerank is written in `pagerank.result.serial` and `pagerank.result.pthread`
for serial and parallel executions respectively.
1. Time is measured using the system function call `gettimeofday()` in different parts
of the program. The time reported is only the time spent in the matrix multiplication
parts.

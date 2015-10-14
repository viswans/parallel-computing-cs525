MPI Page Rank Programming Assignment
--------------------

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
1. The files [mainSerial.cpp](src/cpp/mainSerial.cpp) and [mainMPI.cpp](src/cpp/mainMPI.cpp) contain
the contain the main functions that are called from `main()`.
1. The output of pagerank is written in `pagerank.result.serial` and `pagerank.result.parallel`
for serial and parallel executions respectively.
1. There are debug outputs for both serial and parallel executions used to keep track of
level of execution - they can be ignored by redirecting the stdout to NULL.
1. Time is measured using the system function call `gettimeofday()` in different parts
of the program. The time reported is only the time spent in the matrix multiplication
parts.

## Notes
1. The output of serial and parallel pagerank computations might slightly differ
because of the involvement floating point arithmetic and the dissimilar methods of addition
in calculating norm between the 2 implementations. At the time of writing there were minor
differences in the pagerank.result to the tune of 100 nodes differing by atmost 2 ranks
in 200k node test case, and in the 4M node test case about 30,000 difference in ranks with
atmost 2 rank difference. This can be observed between the output from different outputs
from different partitioning test runs.
1. Normalization and tolerane difference computation - On verifying times spent in different
parts of the code - it was seen that the normalization in every round was consuming a lot of
time. Hence to avoid the communication costs involved in normalization, there is an
implementation hack which does the following where only once every 16 iterations, there
are 2 normalizations and one check for tolerance difference. The other rounds, there is
a pseudo normalizer = `num_nodes_original_matrix/num_columns_original_matrix` that gives
a reasonable assurance that the matrix vector product doesn't shoot up or get divided into
smaller numbers. [pagerankMPI.cpp](src/mpi/pagerankMPI.cpp#L115)
```C++
    if( ( iterations & 0xf ) == 0 ) normalize( output );
    else if ( ( iterations & 0xf  ) == 1 )
    {
        normalize( output );
        toldiff = calcTolDiff( input, offset, output );
    }
    else divideVector( output, pseudo_normalizer );
```

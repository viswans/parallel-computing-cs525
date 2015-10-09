
#include <main.h>

int main( int argc, const char* argv[] )
{
    #ifndef PAGERANK_MPI
    return mainSerial( argc, argv );
    #else
    return mainMPI( argc, argv );
    #endif
}

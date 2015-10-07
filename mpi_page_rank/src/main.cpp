
#include <main.h>

int main( int argv, const char* argc[] )
{
    #ifndef PAGERANK_MPI
    return mainSerial( argv, argc );
    #else
    return mainMPI( argv, argc );
    #endif
}


#include <main.h>

int main( int argc, char* argv[] )
{
    #ifndef PAGERANK_PTHREAD
    return mainSerial( argc, argv );
    #else
    return mainPthread( argc, argv );
    #endif
}

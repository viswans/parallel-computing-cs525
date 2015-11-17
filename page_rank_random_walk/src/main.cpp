
#include <randomwalker.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <pagerank.h>
#include <utils.h>

enum HelpTypes { eFormat, eFileNotExist };
void help( HelpTypes e )
{
    if( e == eFormat )
    {
        std::cout << "Format: ./randomwalker <graph.txt> <num_iterations> <num_threads>\n";
    }
    else if( e == eFileNotExist )
        std::cout << "Error: Either graph file or partition file could not be opened\n";
}

int main( int argc, char* argv[] )
{
    using namespace PageRank;
    if( argc != 4 ) { help(eFormat); return 0; }
    std::fstream graphFile(argv[1]);
    N num_iters = atoi( argv[2] ), num_threads = atoi( argv[3] );
    if( !graphFile ) { help(eFileNotExist); return 0; }
    std::fstream resultFile( "randomwalk.result", std::ios::out );

    AdjacencyList::CPtr adj_list = AdjacencyList::create(
            CSRMatrix::readFromStream( graphFile ) );
    // std::cout << "DEBUG: Matrix was succesfully read into DS\n";
    // fill initial vector with all equal values = 1/ncolumns
    N num_nodes = adj_list->getNumNodes();
    NVec counter( num_nodes, 0 );
    Timer t;
    t.start();
    RandomWalker::walk( adj_list, num_iters, num_threads, counter);
    t.stop();
    resultFile << "time: " <<  t.getTimeSpent() << "s\n";
    std::cout << "DEBUG: time: " << t.getTimeSpent() << "s\n";
    Utils::writePageRank( resultFile, counter );

    return 0;
}

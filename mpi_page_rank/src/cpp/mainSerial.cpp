
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
        std::cout << "Format: ./pagerank.serial <graph.txt>\n";
        std::cout << "Format: ./pagerank.parallel <graph.txt> <graph-partition.txt>\n";
    }
    else if( e == eFileNotExist )
        std::cout << "Error: Either graph file or partition file could not be opened\n";
}

int mainSerial( int argc, char* argv[] )
{
    if( argc != 2 ) { help(eFormat); return 0; }
    std::fstream graphFile(argv[1]);
    if( !graphFile ) { help(eFileNotExist); return 0; }
    std::fstream resultFile( "pagerank.result.serial", std::ios::out );

    // enter page rank program
    using namespace PageRank;
    CSRMatrix::CPtr matrix( CSRMatrix::readFromStream( graphFile ) );
    // std::cout << "DEBUG: Matrix was succesfully read into DS\n";
    // fill initial vector with all equal values = 1/ncolumns
    N num_nodes = matrix->numColumns();
    RVec eigen_vect( num_nodes, 1.0/sqrt( num_nodes ) );
    timeval start_time, end_time;
    gettimeofday( &start_time, NULL);
    PageRankSerial::calculatePageRank( *matrix, eigen_vect );
    gettimeofday( &end_time, NULL);
    // std::ofstream vec_dump( "vec1.out");
    // Utils::showVector( vec_dump, eigen_vect, "\n" );
    // std::cout << "DEBUG: " << start_time.tv_sec << ' ' << start_time.tv_usec << "\n";
    // std::cout << "DEBUG: " << end_time.tv_sec << ' ' << end_time.tv_usec << "\n";
    R time_taken = (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;
    resultFile << "time: " <<  time_taken << "s\n";
    std::cout << "DEBUG: time: " << time_taken << "s\n";
    Utils::writePageRank( resultFile, eigen_vect );

    return 0;
}

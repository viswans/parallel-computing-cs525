
#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <pagerankPth.h>
#include <utils.h>

enum HelpTypes { eFormat, eFileNotExist };
void help( HelpTypes h );

int mainPthread( int argc, char* argv[] )
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
    RVec eigen_vect( num_nodes, 1.0/num_nodes );
    Timer t;
    ConvergenceCriterion c;
    t.start();
    PageRankPthread::calculatePageRank( matrix, eigen_vect, 16, c );
    t.stop();
    // std::ofstream vec_dump( "vec1.out");
    // Utils::showVector( vec_dump, eigen_vect, "\n" );
    // std::cout << "DEBUG: " << start_time.tv_sec << ' ' << start_time.tv_usec << "\n";
    // std::cout << "DEBUG: " << end_time.tv_sec << ' ' << end_time.tv_usec << "\n";
    resultFile << "time: " <<  t.getTimeSpent() << "s\n";
    std::cout << "DEBUG: time: " << t.getTimeSpent() << "s\n";
    Utils::writePageRank( resultFile, eigen_vect );

    return 0;
}

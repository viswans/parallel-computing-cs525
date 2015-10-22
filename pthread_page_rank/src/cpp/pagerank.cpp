#include <pagerank.h>
#include <cmath>
#include <cassert>
#include <utils.h>

#include <iostream>

using namespace PageRank;

void PageRankSerial::calculatePageRank (
    const CSRMatrix& matrix,
    RVec& input,
    const ConvergenceCriterion& criterion)
{
    assert( matrix.numColumns() == input.size() );
    double toldiff = 1e5;
    std::shared_ptr< RVec >
        input_buf( new RVec( input ) ), output_buf( new RVec( input.size() ) ), temp;
    unsigned int i = 0;
    while( ++i < criterion.max_iterations ) {
        matrix.multiply( *(input_buf), *(output_buf) ) ;
        Utils::normalize( *(output_buf) );
        toldiff = Utils::normOfDiff( *(input_buf), *(output_buf) );
        std::cout << "DEBUG: iterations = " << i <<
            " toldiff = " << toldiff << "\n";
        std::swap( input_buf, output_buf );
        if( toldiff <= criterion.tolerance ) break;
    };
    input = *(input_buf);
    if( i == criterion.max_iterations )
        std::cout << "DEBUG: Terminated because of maxiterations with " <<
            " tolerancediff = " << toldiff << " and max_iterations = "
            << criterion.max_iterations << "\n";
    else
        std::cout << "DEBUG: Finished and converged on pagerank vector"
            " in " << i << " iterations with ||Ax - x||_2 = " << toldiff << "\n";


}

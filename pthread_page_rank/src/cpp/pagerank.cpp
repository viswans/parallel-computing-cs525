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
    R old_norm = 1e5, new_norm;
    double toldiff = 1e5;
    std::shared_ptr< RVec >
        input_buf( new RVec( input ) ), output_buf( new RVec( input.size() ) ), temp;
    unsigned int i = 0;
    while( ++i < criterion.max_iterations ) {
        matrix.multiply( *(input_buf), *(output_buf) ) ;
        // Utils::normalize( *(output_buf) );
        new_norm = sqrt( Utils::sumOfSquares( *(output_buf) ));
        toldiff = std::abs( new_norm - old_norm );
        std::cout << "DEBUG: iterations = " << i <<
            " toldiff = " << toldiff << "\n";
        std::swap( input_buf, output_buf );
        old_norm = new_norm;
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

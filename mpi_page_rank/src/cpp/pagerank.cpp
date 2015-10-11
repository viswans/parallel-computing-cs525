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
    RVec output(input.size());
    double toldiff = 1e5;
    unsigned int i = 0;
    while( ++i < criterion.maxIterations &&
            toldiff > criterion.tolerance ) {
        matrix.multiply( input, output );
        Utils::normalize( output );
        toldiff = Utils::normOfDiff( input, output );
        std::cout << "DEBUG: iterations = " << i <<
            " toldiff = " << toldiff << "\n";
        input = output;
        return;
    };
    if( i == criterion.maxIterations )
        std::cout << "DEBUG: Terminated because of maxiterations with " <<
            " tolerancediff = " << toldiff << " and maxIterations = "
            << criterion.maxIterations << "\n";
    else
        std::cout << "DEBUG: Finished and converged on pagerank vector"
            " in " << i << " iterations with ||Ax - x||_2 = " << toldiff << "\n";


}

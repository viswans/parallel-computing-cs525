#include <pagerank.h>
#include <cmath>
#include <cassert>

#include <iostream>

using namespace PageRank;

namespace {
    // utility functions
    void normalize( RVec& vect ) {
        double squared_norm = 0;
        for(unsigned int i= 0; i < vect.size(); ++i)
            squared_norm += vect[i]* vect[i];

        double norm_2 = sqrt(squared_norm);
        for(unsigned int i= 0; i < vect.size(); ++i)
            vect[i] /= norm_2;
    }

    double norm_diff(
        const RVec& v1,
        const RVec& v2 )
    {
        double norm_diff = 0;
        for( unsigned int i = 0; i < v1.size(); ++i )
        {
             double diff = v1[i] - v2[i];
             norm_diff += diff * diff;
        }
        return sqrt( norm_diff );
    }
}

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
        normalize( output );
        toldiff = norm_diff( input, output );
        // std::cout << "DEBUG: output = " << output <<
        //    "toldiff = " << toldiff << "\n";
        input = output;
    };
    if( i == criterion.maxIterations )
        std::cout << "DEBUG: Terminated because of maxiterations with " <<
            " tolerancediff = " << toldiff << " and maxIterations = "
            << criterion.maxIterations << "\n";
    else
        std::cout << "DEBUG: Finished and converged on pagerank vector"
            " in " << i << " iterations with ||Ax - x||_2 = " << toldiff << "\n";


}

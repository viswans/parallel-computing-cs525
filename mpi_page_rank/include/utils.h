#ifndef __PAGERANK_UTILS__
#define __PAGERANK_UTILS__

#include <types.h>

namespace PageRank {
    const int defaultMaxIters = 1000;
    const double defaultTolerance = 1e-10;

    struct ConvergenceCriterion
    {
        double tolerance;
        unsigned int maxIterations;
        ConvergenceCriterion(
            int maxiters = defaultMaxIters,
            double tol = defaultTolerance )
            : tolerance( tol ), maxIterations( maxiters ) {}
    };

    class Utils
    {
     public:
        static void writePageRank(
                std::ostream& oss,
                const RVec& page_rank_weight );

        static R sumOfSquares( const RVec& input );
        // normOfDiff only for vectors of same size
        static R normOfDiff( const RVec& v1, const RVec& v2 );
        static void normalize( RVec& input );

    };
}


#endif   // end of utils

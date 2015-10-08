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

void writePageRank( std::ostream& oss, const RVec& page_rank_weight );
}


#endif   // end of utils

#ifndef __PAGERANK_PAGERANK__
#define __PAGERANK_PAGERANK__

#include <csr.h>

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

    class PageRankSerial
    {
     public:
        // calculate page rank and store it in
        // the given input vector
        static void calculatePageRank (
            const CSRMatrix& matrix,
            RVec& input,
            const ConvergenceCriterion& crit = ConvergenceCriterion());
    };


} // end namespace PageRank

#endif // __PAGERANK_PAGERANK__

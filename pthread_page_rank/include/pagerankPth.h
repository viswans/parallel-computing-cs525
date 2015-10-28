#ifndef __PAGERANK_PAGERANKPTH__
#define __PAGERANK_PAGERANKPTH__

#include <csr.h>
#include <utils.h>

namespace PageRank {

    class PageRankPthread
    {
     public:
        // calculate page rank and store it in
        // the given input vector
        static void calculatePageRank (
            const CSRMatrix::CPtr& matrix,
            RVec& input,
            N num_threads,
            const ConvergenceCriterion& crit = ConvergenceCriterion());
    };


} // end namespace PageRank

#endif // __PAGERANK_PAGERANK__

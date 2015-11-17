#ifndef __PAGERANK_RANDOMWALK__
#define __PAGERANK_RANDOMWALK__

#include <adjlist.h>

namespace PageRank {

    class RandomWalker
    {
     public:
        static void walk(
            const CSRMatrix::CPtr& matrix,
            N num_iterations,
            N num_threads,
            NVec& counter);

    };
}

#endif

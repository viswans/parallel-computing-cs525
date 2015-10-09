#ifndef __PAGERANK_PREPROCESS__
#define __PAGERANK_PREPROCESS__

#include <csr.h>
#include <partition.h>
#include <utils.h>
#include <typesMPI.h>

namespace PageRank {

    struct PreProcOutput
    {
        CSRMatrix::CPtr matrix;
        ProcessPartitionInfo::CPtr partition_info;
    };

    class PageRankMPI
    {
     public:
        static PreProcOutput preprocess(
            const CSRMatrix::CPtr matrix,
            const Partition::CPtr partition,
            std::vector< NodePartitionInfo >& partition_map );

        static void calculatePageRank (
            const CSRMatrix& matrix,
            const ProcessPartitionInfo& partition,
            RVec& input,
            const ConvergenceCriterion& crit = ConvergenceCriterion());

    };
}

#endif

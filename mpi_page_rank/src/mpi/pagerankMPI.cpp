#include <pagerankMPI.h>
#include <mpi.h>


using namespace PageRank;

PreProcOutput PageRankMPI::preprocess(
    const CSRMatrix::CPtr matrix,
    const Partition::CPtr partition)
{

}

void PageRankMPI::calculatePageRank(
    const CSRMatrix& matrix,
    const ProcessPartitionInfo& partition,
    RVec& input,
    const ConvergenceCriterion& crit )
{

}

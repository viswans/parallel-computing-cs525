#ifndef __PAGERANK_CSR_PTHREAD__
#define __PAGERANK_CSR_PTHREAD__

#include <csr.h>
#include <pthread.h>

namespace PageRank {

    class CSRMatrixPthread
    {
     public:
        void multiply(
            const RVec& input_vector, RVec& output_vector ) const;

        CSRMatrixPthread(
            const std::vector< CSRMatrixEntry >* entries_,
            const NVec* row_ptr, N start_, N end_ );

     private:
        const std::vector< CSRMatrixEntry >* entries;
        const NVec* row_ptr;
        N start_row, end_row;
    };

}

#endif

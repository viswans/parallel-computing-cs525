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
            const CSRMatrix* matrix, N start_, N end_ );

     private:
        const CSRMatrix* matrix;
        N start_row, end_row;
    };

}

#endif

#ifndef __PAGERANK_PREPROCESS__
#define __PAGERANK_PREPROCESS__

#include <csr.h>
#include <partition.h>
#include <utils.h>

namespace PageRank {

    class PageRankMPI
    {
        static std::pair< CSRMatrix::CPtr , ProcessParitionInfo::CPtr >
            preprocess(
            const std::string& matrix_file_name,
            const std::string& partition_file_name );

        static void
    }
}

#endif

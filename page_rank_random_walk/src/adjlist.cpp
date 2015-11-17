#include <adjlist.h>

using namespace PageRank;

namespace {

using VecNVec = AdjacencyList::VecNVec;
std::shared_ptr< VecNVec > fill_list( const CSRMatrix::CPtr& matrix )
{
    N nrows = matrix->numRows();
    const NVec* row_idx = matrix->getRowPtr();
    const std::vector< CSRMatrixEntry >* mat_entries = matrix->getMatrixEntries();
    std::shared_ptr < VecNVec > ret = std::make_shared< VecNVec >( nrows );
    for( N i = 0; i < nrows; ++i )
    {
        for( N j = row_idx->at(i); j < row_idx->at(i+1); ++j )
        {
            CSRMatrixEntry e = mat_entries->at(j);
            N col = e.column_idx;
            ret->at(col).push_back(i);
        }
    }
    return ret;
}

}

AdjacencyList::CPtr AdjacencyList::create( const CSRMatrix::CPtr& matrix )
{
    return std::make_shared< const AdjacencyList >( matrix );
}

AdjacencyList::AdjacencyList( const CSRMatrix::CPtr& matrix )
    : adj_list( fill_list( matrix ) )
{
}

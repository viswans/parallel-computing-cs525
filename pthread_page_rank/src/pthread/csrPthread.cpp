#include <csrPthread.h>

using namespace PageRank;

CSRMatrixPthread::CSRMatrixPthread(
    const std::vector< CSRMatrixEntry >* entries_,
    const NVec* row_ptr_, N start_, N end_ )
: entries( entries_ ), row_ptr( row_ptr_ ),
    start_row( start_), end_row( end_ )
{}

void CSRMatrixPthread::multiply(
    const RVec& input_vector,
    RVec& output_vector ) const
{
    for( N i = start_row; i < end_row; ++i )
    {
        output_vector[i] = 0;
        for( N j = row_ptr->at(i);
               j < row_ptr->at(i+1); ++j )
        {
            output_vector[i] += entries->at(j).value *
                input_vector[ entries->at(j).column_idx ];
        }
    }

}

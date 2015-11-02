#include <csrPthread.h>

using namespace PageRank;

CSRMatrixPthread::CSRMatrixPthread(
    const CSRMatrix* matrix_, N start_, N end_ )
: matrix( matrix_ ), start_row( start_), end_row( end_ )
{
    // std::cout << "DEBUG: start = " << start_row << " end = " << end_row
    //     << " Matrix = " << matrix << "\n";
}

void CSRMatrixPthread::multiply(
    const RVec& input_vector,
    RVec& output_vector ) const
{
    // std::cout << "DEBUG: Matrix = " << matrix << "\n";
    for( N i = start_row; i < end_row; ++i )
    {
        output_vector[i] = 0;
        for( N j = matrix->getRowPtr()->at(i);
               j < matrix->getRowPtr()->at(i+1); ++j )
        {
            const std::vector< CSRMatrixEntry >*
                entries =  matrix->getMatrixEntries();
            output_vector[i] += entries->at(j).value *
                input_vector[ entries->at(j).column_idx ];
        }
    }

}

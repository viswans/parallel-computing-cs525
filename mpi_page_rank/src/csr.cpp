
#include <csr.h>
#include <cmath>
#include <sstream>
#include <cassert>

// can/should be removed later
#include <iostream>

using namespace PageRank;

CSRMatrixEntry::CSRMatrixEntry(
    unsigned int column_idx_,
    double value_ ):
    column_idx( column_idx_ ),value( value_ )
{
}

CSRMatrix::CSRMatrix(
    N nrows_, N ncolumns_,
    std::shared_ptr< std::vector<CSRMatrixEntry> >& entries_,
    const NVecPtr& row_ptr_ )
    : nrows( nrows_ ), ncolumns( ncolumns_ ),
      entries( entries_ ), row_ptr( row_ptr_ )
{
    ASSERT_EQUALS( nrows, row_ptr_->size() - 1);
    writeToStream( std::cout  );
}

void CSRMatrix::writeToStream( std::ostream& oss ) const
{
    oss << "DEBUG: # Rows = " << nrows << " # Cols = " << ncolumns << "\n";
    // for(N i = 0; i < nrows; ++i )
    // {
    //     for( N j = row_ptr->at(i); j < row_ptr->at(i+1); ++j )
    //     {
    //         CSRMatrixEntry temp = entries->at(j);
    //         oss << "DEBUG: @(" << i << "," << temp.column_idx << ") -> "
    //             << temp.value << "\n";
    //     }
    // }
}



void CSRMatrix::multiply(
    const std::vector<double>& input_vector,
    std::vector<double>& output_vector
) const
{
    // calculate output_vector = matrix*input_vector
    for(N i = 0; i < nrows; ++i )
    {
        output_vector[i] = 0;
         // for each row iterate through
         // entries and do vector multiplication
         for(unsigned int j = (*row_ptr)[i];
                 j < (*row_ptr)[i+1]; ++j )
         {
             output_vector[i] += (*entries)[j].value *
                 input_vector[(*entries)[j].column_idx];
         }
    }
}

std::shared_ptr< const CSRMatrix >
CSRMatrix::readFromStream( std::istream& iss  )
{
    using namespace std;
    string line;
    getline( iss, line );
    string temp;
    istringstream sss(line);
    shared_ptr< vector< CSRMatrixEntry > > entries_ptr(
            new vector<CSRMatrixEntry>());
    sss >> temp;      // first string val
    // std::cout << "DEBUG: Doing " << temp << "\n";
    R input;
    while( sss >> input )
        entries_ptr->push_back( CSRMatrixEntry( 0, input ) );
    // added values to vector of entries
    getline( iss, line );
    sss.clear();
    sss.str(line);
    sss >> temp;        // first string col_idx
    // std::cout << "DEBUG: Doing " << temp << "\n";
    N input1, i = 0, columns = 0;
    for( ; sss >> input1 && i < entries_ptr->size(); ++i )
    {
        (*entries_ptr)[i].column_idx = input1;
        columns = ( columns < input1 ) ? input1: columns;
    }

    // number of column entries is same as number of values
    ASSERT_EQUALS( i , entries_ptr->size() );

    // get input for row indices
    getline( iss, line );
    sss.clear();
    sss.str(line);
    // std::cout << "DEBUG: Doing " << temp << "\n";
    sss >> temp;        // first string col_idx
    NVecPtr row_ptr( new NVec() );
    for( i = 0; sss >> input1; ++i )
        row_ptr->push_back( input1 );

    ASSERT_EQUALS_MSG( row_ptr->size() - 1, columns + 1,
            "# Rows != # Columns. Page rank on a rect matrix?\n" );

    return shared_ptr<const CSRMatrix>(
            new CSRMatrix( row_ptr->size() - 1, columns + 1, entries_ptr, row_ptr) );
}

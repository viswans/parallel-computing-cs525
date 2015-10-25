#ifndef __PAGERANK_CSR__
#define __PAGERANK_CSR__

#include <vector>
#include <types.h>

namespace PageRank {

    struct CSRMatrixEntry
    {
        N column_idx;
        R value;
        CSRMatrixEntry(): column_idx(0), value(0.0) {}
        CSRMatrixEntry( N column_idx_, R value );
    };

    class CSRMatrix
    {
     public:
        typedef std::shared_ptr< const CSRMatrix > CPtr;
        static CPtr readFromStream( std::istream& iss );
        static CPtr create(
            N num_cols,
            const RVec& values,
            const NVec& cols,
            const NVec& row_ptrs );

        N numRows() const { return nrows; }
        N numColumns() const { return ncolumns; }

        // Debugging
        void writeToStream( std::ostream& sss ) const;

        void multiply(
            const RVec& input_vector, RVec& output_vector ) const;

        // Not good software practice!
        // Ideally Matrix multiply functions must be split
        // while the data holding responsibility must be handled
        // here.
        void multiplyPthread(
            const RVec& input_vector,
            N num_threads,
            RVec& output_vector) const;

        std::shared_ptr< std::vector< CSRMatrixEntry > >
        getMatrixEntries() const { return entries; }

        NVecPtr getRowPtr() const { return row_ptr; }

     private:
        CSRMatrix(
            N nrows_, N ncolumns_,
            std::shared_ptr< std::vector<CSRMatrixEntry> >& entries_,
            const NVecPtr& row_ptr);

         unsigned int nrows, ncolumns;
         std::shared_ptr< std::vector<CSRMatrixEntry> > entries;
         NVecPtr row_ptr;
    };

} // end namespace pagerank

#endif    // __PAGERANK_CSR_

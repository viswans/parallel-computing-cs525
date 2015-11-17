#ifndef __PAGERANK_ADJLIST__
#define __PAGERANK_ADJLIST__

#include <csr.h>
#include <types.h>

namespace PageRank
{
    class AdjacencyList
    {
     public:
        typedef std::shared_ptr< const AdjacencyList > CPtr;
        typedef std::vector< NVec > VecNVec;
        static AdjacencyList::CPtr create( const CSRMatrix::CPtr& matrix );
        N getNumNodes() const { return adj_list->size(); }
        N numNeigbours( N i ) const { return adj_list->at(i).size(); }
        N getNthNeighbour( N i, N n ) const { return adj_list->at(i)[n]; }

        AdjacencyList( const CSRMatrix::CPtr& matrix );

     private:
        std::shared_ptr< VecNVec > adj_list;

    };
}

#endif

#include <utils.h>
#include <algorithm>

using namespace PageRank;

namespace {
struct PageRankSorter
{
    const RVec* basis;
    typedef PageRank::N N;
    typedef std::pair< N, N > NPair;
    PageRankSorter( const RVec* basis_ ): basis(basis_) {}
    bool operator()( NPair i, NPair j)
    {
        return (basis->at(i.first) >= basis->at(j.first)) ;
    }
};
} // end of anon namespace

namespace PageRank {
void writePageRank( std::ostream& oss, const RVec& page_rank_weight )
{
    N num_nodes = page_rank_weight.size();
    std::vector< std::pair< N, N > > page_rank( num_nodes );
    N i;
    for( i =0; i < num_nodes; ++i )
    {
        page_rank[i].first = i;
        page_rank[i].second = 0;
    }
    sort( page_rank.begin(), page_rank.end(),
            PageRankSorter( &page_rank_weight) );
    for( N i = 0; i < num_nodes; ++i ) page_rank[i].second = i;
    sort( page_rank.begin(), page_rank.end() );

    for( N i = 0; i < num_nodes; ++i )
        oss << page_rank[i].first << " " << page_rank[i].second << "\n";

}
}

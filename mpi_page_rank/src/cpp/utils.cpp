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

R Utils::sumOfSquares( const RVec& input )
{
    R squared_norm = 0;
    for(N i= 0; i < input.size(); ++i)
        squared_norm += input[i]* input[i];
    return squared_norm;
}

void Utils::normalize( RVec& vec ) {
    R norm_2 = sqrt(sumOfSquares( vec ));
    for(unsigned int i= 0; i < vec.size(); ++i)
        vec[i] /= norm_2;
}

R Utils::normOfDiff(
    const RVec& v1,
    const RVec& v2 )
{
    ASSERT_EQUALS( v1.size(), v2.size() );
    R norm_diff = 0;
    for( N i = 0; i < v1.size(); ++i )
    {
         R diff = v1[i] - v2[i];
         norm_diff += diff * diff;
    }
    return sqrt( norm_diff );
}

void Utils::writePageRank( std::ostream& oss, const RVec& page_rank_weight )
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

#include <utils.h>
#include <iomanip>
#include <algorithm>

using namespace PageRank;

namespace {
struct PageRankSorter
{
    typedef PageRank::N N;
    typedef std::pair< N, N > NPair;
    bool operator()( NPair i, NPair j)
    {
        return ( i.second > j.second );
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

void Utils::writePageRank( std::ostream& oss, const NVec& counts )
{
    N num_nodes = counts.size();
    std::vector< std::pair< N, N > > page_rank( num_nodes );
    N i;
    for( i =0; i < num_nodes; ++i )
    {
        page_rank[i].first = i;
        page_rank[i].second = counts[i];
    }
    std::sort( page_rank.begin(), page_rank.end(),
            PageRankSorter() );
    std::cout << "DEBUG: Sort based on rank done\n";
    for( N i = 0; i < num_nodes; ++i )
        oss << page_rank[i].first << " " << page_rank[i].second << "\n";

}

void Utils::calcCountFromDisp( const NVec& disp, N total, NVec& counts )
{
    assert( counts.size() == disp.size() );
    N sz = counts.size();
    for( N i=0 ; i < sz - 1; ++i )
        counts[i] = disp[i+1] - disp[i];
    counts[ sz-1 ] = total - disp[sz-1];
}


void Timer::start()
{
    gettimeofday( &start_time, NULL );
}

void Timer::stop()
{
    gettimeofday( &end_time, NULL );
    time_spent += (  end_time.tv_sec - start_time.tv_sec ) +
    + ( end_time.tv_usec - start_time.tv_usec )/1e6;
}


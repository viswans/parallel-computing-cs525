#include <pagerankMPI.h>
#include <mpi.h>
#include <cmath>

using namespace PageRank;

namespace {

    void calcCountFromDisp( const NVec& disp, NVec& counts )
    {
        assert( counts.size() == disp.size() );
        N sz = counts.size();
        counts[0] = 0;
        for( N i=1 ; i < sz; ++i )
            counts[i] = disp[i] - disp[i-1];
    }

    // utiltiy function to use just for this case and avoid copy
    R normSqDiffTemp ( const RVec& input, const RVec& output, int offset )
    {
        N i = 0, sz = output.size(); R norm_diff = 0;
        for( ; i < sz; ++i )
        {
            R temp = (input[offset+i] - output[i]);
            norm_diff += temp * temp;
        }
        return norm_diff;
    }

    void normalize( RVec& input, N root = 0 )
    {
        R squared_norm = Utils::sumOfSquares( input );
        R squared_norm_sum;
        MPI::COMM_WORLD.Reduce( &squared_norm, &squared_norm_sum,
            1, MPI::DOUBLE, MPI::SUM, root);
        R norm = sqrt( squared_norm_sum );
        MPI::COMM_WORLD.Bcast( &norm, 1, MPI::DOUBLE, root);
        N size = input.size();
        for( N i = 0; i < size; ++i )
            input[i] = input[i]/norm;
    }

    R calcTolDiff(
        const RVec& input, const RVec& output,
        N offset, N root = 0 )
    {
        R norm_diff_local = normSqDiffTemp( input, output, offset );
        R norm_diff;
        MPI::COMM_WORLD.Reduce( &norm_diff_local, &norm_diff,
            1, MPI::DOUBLE, MPI::SUM, root);
        R tol_diff = sqrt( norm_diff );
        MPI::COMM_WORLD.Bcast( &tol_diff, 1, MPI::DOUBLE, root);
        return tol_diff;
    }

    void copyToSendBuffer(
        const RVec& input,
        const NVec& snd_vals,
        RVec& snd_buf )
    {
        ASSERT_EQUALS( snd_vals.size(), snd_buf.size() );
        N i = 0, end = snd_vals.size();
        for( ; i < end ; ++i ) snd_buf[i] = input[snd_vals[i]];
    }

    void sanityCheck( const NVec& snd_vals, N num_rows )
    {
        N max_val = 0;
        for(N i = 0 ; i < snd_vals.size(); ++i)
            max_val = ( max_val < snd_vals[i] ) ? snd_vals[i] : max_val;
        assert( max_val < num_rows );
    }
}

void PageRankMPI::calculatePageRank(
    const CSRMatrix& matrix,
    const ProcessPartitionInfo& partition,
    RVec& input,
    const ConvergenceCriterion& criterion )
{
    // do matrix multiplication
    // send vector values to respective guys
    // reduce norm
    // bcast new norm
    // check for convergence
    // if converged send output to 0
    sanityCheck( partition.snd_vals, matrix.numRows());
    N iterations = 0;
    // this is the offset of the current rank vector
    // the input vector
    N offset = partition.snd_disp[proc_info.rank];
    N num_partitions = partition.snd_disp.size();
    R toldiff= 1e5;
    RVec output( matrix.numRows(), 0 ), send_buffer( partition.snd_vals.size() );
    NVec send_count( num_partitions ), rx_count( num_partitions );
    calcCountFromDisp( partition.snd_disp , send_count );
    calcCountFromDisp( partition.rx_disp, rx_count ) ;

    while( ++iterations < criterion.maxIterations )
    {
        matrix.multiply( input, output );
        normalize( output );
        // calculate toldiff
        toldiff = calcTolDiff( input, output, offset );
        std::cout << "DEBUG: Iterations = " << iterations
            <<  "Tolerance = " << toldiff << std::endl;
        if( toldiff <= criterion.tolerance ) break;
        ++iterations;
        copyToSendBuffer( output, partition.snd_vals, send_buffer );
        MPI::COMM_WORLD.Alltoallv(
            (const int*)&send_buffer[0], (const int*)&send_count[0],
            (const int*)&partition.snd_disp[0], MPI::DOUBLE,
            (int*)&input[0], (const int*)&rx_count[0],
            (const int*)&partition.rx_disp[0], MPI::DOUBLE);
    }

}

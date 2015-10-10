#include <pagerankMPI.h>
#include <mpi.h>

using namespace PageRank;

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
    for(N i = 0, max_val = 0; i < snd_vals.size(); ++i)
        max_val = ( max_val < snd_vals[i] ) ? snd_vals[i] : max_val;
    ASSERT_EQUALS( max_vals, num_rows );
}

void PageRankMPI::calculatePageRank(
    const CSRMatrix& matrix,
    const ProcessPartitionInfo& partition,
    RVec& input,
    const ConvergenceCriterion& crit )
{
    // do matrix multiplication
    // send vector values to respective guys
    // reduce norm
    // bcast new norm
    // check for convergence
    // if converged send output to 0
    sanityCheck( partition.snd_vals, matrix->numRows());
    bool converged = false;
    N iterations = 0;
    RVec output( matrix->numRows(), 0 ), send_buffer( parition.snd_vals.size() );
    NVec send_count( calcCountFromDisp( partition.snd_disp ) ),
         rcv_count( calcCountFromDisp( partition.rx_disp ) );
    while( !converged )
    {
        matrix->multiply( input, output );
        copyToSendBuffer( output, partition.snd_vals, send_buffer );
        MPI::COMM_WORLD.Alltoallv(
            &send_buffer[0], &send_count[0],
            &partition.snd_disp[0], MPI::DOUBLE,
            &input[0], &rx_count[0],
            &partition.rx_disp[0], MPI::DOUBLE);

        ++iterations;
    }

}

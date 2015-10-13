#include <pagerankMPI.h>
#include <mpi.h>
#include <cmath>
#include <utils.h>
#include <sys/time.h>

using namespace PageRank;

namespace {

    // utiltiy function to use just for this case and avoid copy
    R normSqDiffTemp ( const RVec& input, N offset, const RVec& output )
    {
        N i = 0, sz = output.size(); R norm_diff = 0;
        for( ; i < sz; ++i )
        {
            R temp = (input[offset+i] - output[i]);
            norm_diff += temp * temp;
        }
        // std::cout << "DEBUG: norm_diff @" << proc_info.rank <<
        //     " = " << norm_diff << "\n";
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
        const RVec& input, N offset, const RVec& output, N root = 0 )
    {
        R norm_diff_local = normSqDiffTemp( input, offset, output );
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
    RVec& output,
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
    timeval start_time, end_time;
    // std::ofstream vec_dump( "vec1.out");
    // Utils::showVector( vec_dump, eigen_vect, "\n" );
    // std::cout << "DEBUG: " << start_time.tv_sec << ' ' << start_time.tv_usec << "\n";
    // std::cout << "DEBUG: " << end_time.tv_sec << ' ' << end_time.tv_usec << "\n";
    R time_taken[] = {0, 0, 0, 0};
    // this is the offset of the current rank vector
    // the input vector
    N offset = partition.rx_disp[proc_info.rank];
    N num_partitions = partition.snd_disp.size();
    R toldiff= 1e5;
    RVec send_buffer( partition.snd_vals.size() );
    NVec send_count( num_partitions ), rx_count( num_partitions );
    Utils::calcCountFromDisp( partition.snd_disp, partition.snd_vals.size() , send_count );
    // std::cout << "DEBUG: Send Counts " << proc_info.rank;
    // Utils::showVector( std::cout, send_count );
    Utils::calcCountFromDisp( partition.rx_disp, matrix.numColumns(), rx_count ) ;
    // std::cout << "DEBUG: Rx Counts " << proc_info.rank;
    // Utils::showVector( std::cout, rx_count );

    while( ++iterations < criterion.max_iterations )
    {
        gettimeofday( &start_time, NULL);
        matrix.multiply( input, output );
        gettimeofday( &end_time, NULL);
        time_taken[0] += (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;

        gettimeofday( &start_time, NULL);
        normalize( output );
        // calculate toldiff
        toldiff = calcTolDiff( input, offset, output );
        gettimeofday( &end_time, NULL);
        time_taken[1] += (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;

        DO_ONLY_AT_RANK0
        std::cout << "DEBUG: iterations = " << iterations
            <<  " toldiff = " << toldiff << std::endl;
        if( toldiff <= criterion.tolerance ) break;
        gettimeofday( &start_time, NULL);
        copyToSendBuffer( output, partition.snd_vals, send_buffer );
        gettimeofday( &end_time, NULL);
        time_taken[2] += (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;

        gettimeofday( &start_time, NULL);
        MPI::COMM_WORLD.Alltoallv(
            (const int*)&send_buffer[0], (const int*)&send_count[0],
            (const int*)&partition.snd_disp[0], MPI::DOUBLE,
            (int*)&input[0], (const int*)&rx_count[0],
            (const int*)&partition.rx_disp[0], MPI::DOUBLE);
        gettimeofday( &end_time, NULL);
        time_taken[3] += (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;
    }

    DO_ONLY_AT_RANK0 {
    if( iterations == criterion.max_iterations )
        std::cout << "DEBUG: Terminated because of maxiterations with " <<
            " tolerancediff = " << toldiff << " and max_iterations = "
            << criterion.max_iterations << "\n";
    else
        std::cout << "DEBUG: Finished and converged on pagerank vector"
            " in " << iterations << " iterations with ||Ax - x||_2 = " << toldiff << "\n";
    }
    std::cout << "DEBUG: Rank = " << proc_info.rank <<
        " MMult = " << time_taken[0] << "s, ";
    std::cout << "Norma = " << time_taken[1] << "s, ";
    std::cout << "BfCpy = " << time_taken[2] << "s, ";
    std::cout << "Commn = " << time_taken[3] << "s\n";
}

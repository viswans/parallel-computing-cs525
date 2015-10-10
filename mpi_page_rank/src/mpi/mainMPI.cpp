#include <pagerankMPI.h>
#include <partition.h>
#include <main.h>
#include <mpi.h>
#include <fstream>
#include <cmath>
#include <sys/time.h>

// Global variable
// Just initialized and then used read only
ProcessInfo proc_info;

int finalize() {
    std::cout << "DEBUG: Finalizing\n";
    MPI::Finalize();
    return 0;
}

// Just for debugging
void checkDataStructures( const PageRank::PreProcOutput& pre )
{
    assert( pre.partition_info->snd_disp.size() == pre.partition_info->rx_disp.size() );
    std::cout << "DEBUG: Orig columns = "
        << pre.partition_info->num_columns_original_matrix
        << ", Num partitions = "
        <<  2*pre.partition_info->snd_disp.size() -
        pre.partition_info->rx_disp.size() << "\n";
}

void writeToResult(
    PageRank::N rank,
    timeval start_time,
    timeval end_time,
    const PageRank::RVec& eigen_vect)
{
    using namespace PageRank;
    if( rank ) return;
    // do only if rank is 0
    R time_taken = (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;
    std::fstream resultFile( "pagerank.result", std::ios::out );
    resultFile << "time: " <<  time_taken << "s\n";
    Utils::writePageRank( resultFile, eigen_vect );
}

int mainMPI( int argc, char* argv[] )
{
    MPI::Init(argc, argv);
    proc_info.numprocs = MPI::COMM_WORLD.Get_size();
    proc_info.rank = MPI::COMM_WORLD.Get_rank();
    MPI::Get_processor_name( proc_info.processor_name, proc_info.namelen);
    std::cout << "DEBUG: Hello!!\n";
    std::cout << "DEBUG: Process " << proc_info.rank << " on " <<
        proc_info.processor_name <<  " out of " << proc_info.numprocs << "\n";

    // ========= end of diagnostic output ========================

    using namespace PageRank;
    // if not enough arguments fail
    if( argc != 3 ) { help(eFormat); finalize(); }
    CSRMatrix::CPtr matrix;
    Partition::CPtr partition;
    std::vector< NodePartitionInfo > partition_map;
    DO_ONLY_AT_RANK0
    {
        std::fstream graphFile( argv[1] ), partitionFile( argv[2] );
        if( !graphFile || !partitionFile ) { help(eFileNotExist); return finalize(); }
        matrix =  CSRMatrix::readFromStream( graphFile );
        partition = Partition::createFromStream( partitionFile ) ;
        partition_map.resize( partition->getNumNodes() );
    }

    PreProcOutput pre = PageRankMPI::preprocess( matrix, partition, partition_map );
    checkDataStructures( pre );
    std::cout << "DEBUG: Preprocess done\n";
    CSRMatrix::CPtr localmatrix = pre.matrix;
    ProcessPartitionInfo::CPtr part_info = pre.partition_info;
    N num_nodes = localmatrix->numColumns();
    N num_orig_columns = part_info->num_columns_original_matrix;
    RVec initial_vect( num_nodes, 1.0/sqrt( num_orig_columns ) );
    timeval start_time, end_time;
    gettimeofday( &start_time, NULL);
    PageRankMPI::calculatePageRank( *localmatrix, *part_info, initial_vect );
    gettimeofday( &end_time, NULL);

    // if( proc_info.rank == 0 ) // gahter and store in eigen vector
    RVec eigen_vect;
    writeToResult( proc_info.rank, start_time, end_time, eigen_vect);
    return finalize();
}


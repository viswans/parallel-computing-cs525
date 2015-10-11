#include <pagerankMPI.h>
#include <partition.h>
#include <main.h>
#include <mpi.h>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <utils.h>

// Global variable
// Just initialized and then used read only
ProcessInfo proc_info;

int finalize() {
    std::cout << "DEBUG: Finalizing\n";
    MPI::Finalize();
    return 0;
}

void assembleEigenVec(
    const PageRank::RVec& output,
    PageRank::N num_orig_columns,
    const std::vector<PageRank::NodePartitionInfo>& partition_map,
    const PageRank::NVec& gather_disp,
    PageRank::RVec& eigen_vec)
{
    using namespace PageRank;
    RVec rx_vec;
    NVec gather_count;
    DO_ONLY_AT_RANK0 {
        gather_count.resize( gather_disp.size() );
        Utils::calcCountFromDisp( gather_disp, num_orig_columns, gather_count );
        Utils::showVector( std::cout, gather_disp );
        eigen_vec.resize( num_orig_columns );
        rx_vec.resize( num_orig_columns );
    }
    MPI::COMM_WORLD.Gatherv( &output[0], output.size(), MPI::DOUBLE,
            &rx_vec[0], (const int*)&gather_count[0],
            (const int*)&gather_disp[0], MPI::DOUBLE, 0);

    DO_ONLY_AT_RANK0 {
        N i, sz = rx_vec.size();
        for( i = 0; i < sz; ++i )
        {
             N part_id = partition_map[i].partition_id;
             N row_id = partition_map[i].row_id;
             N access_id = row_id + gather_disp[part_id];
             eigen_vec[i] = rx_vec[ access_id ];
        }
        std::ofstream vec_dump("vec.out");
        Utils::showVector( vec_dump, eigen_vec, "\n" );
    }
    std::cout << "DEBUG: Gathering data\n";

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
    timeval start_time,
    timeval end_time,
    const PageRank::RVec& eigen_vect)
{
    using namespace PageRank;
    DO_IF_NOT_RANK0 return;
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
    // std::cout << "DEBUG: Hello!!\n";
    // std::cout << "DEBUG: Process " << proc_info.rank << " on " <<
    //     proc_info.processor_name <<  " out of " << proc_info.numprocs << "\n";

    // ========= end of diagnostic output ========================

    using namespace PageRank;
    // if not enough arguments fail
    if( argc != 3 ) { help(eFormat); finalize(); }
    CSRMatrix::CPtr matrix;
    Partition::CPtr partition;
    std::vector< NodePartitionInfo > partition_map;
    NVec gather_disp;
    DO_ONLY_AT_RANK0
    {
        std::fstream graphFile( argv[1] ), partitionFile( argv[2] );
        if( !graphFile || !partitionFile ) { help(eFileNotExist); return finalize(); }
        matrix =  CSRMatrix::readFromStream( graphFile );
        partition = Partition::createFromStream( partitionFile ) ;
        partition_map.resize( partition->getNumNodes() );
        gather_disp.resize( partition->getNumPartitions() );
    }

    PreProcOutput pre = PageRankMPI::preprocess( matrix, partition, partition_map, gather_disp );
    checkDataStructures( pre );
    std::cout << "DEBUG: Preprocess done\n";
    CSRMatrix::CPtr localmatrix = pre.matrix;
    ProcessPartitionInfo::CPtr part_info = pre.partition_info;
    N num_nodes = localmatrix->numColumns();
    N num_orig_columns = part_info->num_columns_original_matrix;
    RVec initial_vec( num_nodes, 1.0/sqrt( num_orig_columns ) );
    RVec output_vec( localmatrix->numRows() );
    timeval start_time, end_time;
    gettimeofday( &start_time, NULL);
    PageRankMPI::calculatePageRank( *localmatrix, *part_info, initial_vec, output_vec );
    gettimeofday( &end_time, NULL);

    RVec eigen_vec;
    assembleEigenVec( output_vec, num_orig_columns, partition_map, gather_disp, eigen_vec );
    // writeToResult( start_time, end_time, eigen_vec);
    return finalize();
}


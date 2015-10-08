#include <pagerank.h>
#include <partition.h>
#include <main.h>
#include <mpi.h>

enum HelpTypes { eFormat, eFileNotExist };
void help( HelpTypes e )
{
    if( e == eFormat )
        std::cout << "Format: ./pagerank <graph.txt> <graph-partition.txt>\n";
    else if( e == eFileNotExist )
        std::cout << "Error: Either graph file or partition file could not be opened\n";
}

int finalize() {
    MPI_finalize();
    return 0;
}

void writeToResult(
    N rank,
    timeval start_time,
    timeval end_time,
    const RVec& eigen_vect)
{
    if( rank ) return;
    // do only if rank is 0
    R time_taken = (  end_time.tv_sec - start_time.tv_sec ) +
        + ( end_time.tv_usec - start_time.tv_usec )/1e6;
    std::fstream resultFile( "pagerank.result", std::ios::out );
    resultFile << "time: " <<  time_taken << "s\n";
    writePageRank( resultFile, eigen_vect );
}

int mainMPI( int argv, const char* argc[] )
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    std::cout << "DEBUG: Process " << rank << " on " <<
        processor_name <<  " out of " << numprocs << "\n";

    // ========= end of diagnostic output ========================

    if( argv != 3 ) { help(eFormat); return finalize(); }
    if( !graphFile || !partitionFile ) { help(eFileNotExist); return finalize(); }

    using namespace PageRank;
    CSRMatrix::CPtr matrix;
    Partition::CPtr partition;
    if( rank == 0 )
    {
        std::fstream graphFile(argc[1]), partitionFile(argc[2]);
        matrix =  CSRMatrix::readFromStream( graphFile );
        partition = Partition::createFromStream( partitionFile ) ;
    }
    CSRMatrix::CPtr localmatrix;
    ProcessPartitionInfo partition_information;
    PreprocessMPI::preprocess(
            matrix, partition, localmatrix, partition_information );

    N num_nodes = localmatrix->numColumns();
    N num_orig_columns = partition_information.num_columns_original_matrix;
    RVec initial_vect( num_nodes, 1.0/sqrt( num_orig_columns ) );
    timeval start_time, end_time;
    gettimeofday( &start_time, NULL);
    PageRankMPI::calculatePageRank( partition_information, *initial_vect );
    gettimeofday( &end_time, NULL);
    if( rank == 0 ) // gahter and store in eigen vector
    writeToResult( rank, start_time, end_time, eigen_vect)
    return finalize();
}


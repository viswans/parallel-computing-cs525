#include <pagerankMPI.h>
#include <set>
#include <mpi.h>


using namespace PageRank;

namespace {
    struct TempCSR {
        RVec values; NVec col_idx; NVec row_ptrs;
    };

    struct DependencySet {
        std::vector< std::set< N > > dep_set;
        DependencySet( N num_partitions )
            : dep_set( num_partitions ) {}

        // findOffset finds the number of entries
        // before elements of partition starts
        N findOffset( N partition ) const
        {
            N ret = 0, i= 0;
            for( ; i != partition; ++i ) ret += dep_set[i].size();
            return ret;
        }
    };
}

PreProcOutput PageRankMPI::preprocess(
    const CSRMatrix::CPtr matrix,
    const Partition::CPtr partition,
    std::vector< NodePartitionInfo >& partition_map,
    NVec& gather_disp)
{
    // matrix and partition information => construct
    // the necessary data

    // preprocess here
    // find new CSRMatrix for all the different processes
    // find dependencies between processes
    PreProcOutput p;
    std::shared_ptr< ProcessPartitionInfo > partition_info(0);
    const int INITIAL_INFO_SIZE = 6;

    DO_ONLY_AT_RANK0
    {
        typedef std::set< N >::iterator It;
        N num_partitions = partition->getNumPartitions();
        std::vector < TempCSR > matrices( num_partitions );
        // initialize dependency set to contain num_partitions elements
        // and each guy in turn initializes its own vector
        std::vector < DependencySet > dependencies(
                num_partitions, DependencySet( num_partitions ) );

        const NVec& row_partition = *(partition->getPartitionMap());
        const std::vector< CSRMatrixEntry >& entries = *(matrix->getMatrixEntries());
        const NVec& row_ptrs = *(matrix->getRowPtr());
        ASSERT_EQUALS( row_partition.size(), matrix->numRows() );

        // initialize all row_vecs to have 0
        for( N i = 0; i < num_partitions; ++i )
            matrices[i].row_ptrs.push_back(0);

        // create all the CSRs albeit with not the process specific
        // information
        for( N i = 0; i < matrix->numRows(); ++i )
        {
            N partition_of_i = row_partition[i];
            NVec& row_vec = matrices[partition_of_i].row_ptrs;
            NVec& col_vec = matrices[partition_of_i].col_idx;
            RVec& val_vec = matrices[partition_of_i].values;

            for( N j = row_ptrs[i]; j < row_ptrs[i+1]; ++j  )
            {
                // for each element in the matrix
                // add it to the tempCSR structure
                N column = entries[j].column_idx;
                col_vec.push_back( column );
                val_vec.push_back( entries[j].value );
                // add column to the set of dependent nodes from process containing row i
                // to process row j
                dependencies[partition_of_i].dep_set[row_partition[column]].insert( column );
            }
            // looks like within a process not neccessarily use all the other row info
            dependencies[partition_of_i].dep_set[partition_of_i].insert( i );
            row_vec.push_back( col_vec.size() );
        }

        // for( N i = 0; i < num_partitions; ++i  )
        //     for( N j = 0; j < num_partitions; ++j )
        //         printf("DEBUG: Partition %u depends on %lu "
        //                 "elements from Partition %u\n",
        //                 i , dependencies[i].dep_set[j].size() , j);

        // fill up map from monolithic vector to
        // partition row based output
        for( N i = 0; i < num_partitions; ++i )
        {
            std::set< N >& set_i = dependencies[i].dep_set[i];
            It it = set_i.begin(), end = set_i.end();
            N count = 0;
            for( ; it != end; ++it, ++count ) {
                N old_row = *it;
                partition_map[old_row].partition_id = row_partition[old_row];
                partition_map[old_row].row_id = count;
            }
        }

        // Replace all CSR column entries with the corresponding
        // vector row values as calculated by the dependency displacement
        // num_columns is the number of columns in the reduced matrix
        NVec num_columns( num_partitions );
        for( N i = 0; i < num_partitions; ++i )
        {
            N count = 0;
            std::map< N, N> big_to_small_map;
            // big_to_small_map converts all columns in CSR
            // from the original column values to the row values
            // of the input vector
            for( N j = 0; j < num_partitions; ++j )
            {
                std::set< N >& set_ij = dependencies[i].dep_set[j];
                It it = set_ij.begin(), end = set_ij.end();
                for( ; it != end; ++it, ++count )
                    big_to_small_map.insert( std::make_pair( *it, count ) );
            }
            num_columns[i] = count;
            NVec& cols = matrices[i].col_idx;
            for( N j = 0; j < cols.size(); ++j )
                cols[j] = big_to_small_map[cols[j]];
        }

        // add row size to gather displacement vector
        gather_disp[0] = 0;
        for( N i = 0; i < num_partitions; ++i )
            gather_disp[i+1] = gather_disp[i] + ( matrices[i].row_ptrs.size() - 1);


        // Send information
        // Find send_vector i.e once rows are calculated
        // computation of which rows should go where
        // Find send_displacements - used for MPI_Alltoallv
        // Find rx_displacements - used for MPI_Alltoallv
        // Send matrix information and the displacement information
        // to particular process / or if master proc, store it in local variable
        for( N i = 0; i < num_partitions; ++i )
        {
            NVec send_info, send_disp, rx_disp;
            send_disp.push_back( 0 );
            for( N j = 0 ; j < num_partitions; ++j )
            {
                // gives dependency of jth set on ith process
                std::set< N >& set_ij = dependencies[j].dep_set[i];
                It it = set_ij.begin(), end = set_ij.end();
                for( ; it != end; ++it )
                    send_info.push_back( partition_map[*it].row_id );
                send_disp.push_back( send_info.size() );
            }

            rx_disp.push_back( 0 );
            for( N j = 0, count = 0 ; j < num_partitions; ++j )
            {
                std::set< N >& set_ij = dependencies[i].dep_set[j];
                count += set_ij.size();
                rx_disp.push_back( count );
            }

            ASSERT_EQUALS( send_disp.size(), num_partitions + 1 );
            ASSERT_EQUALS( rx_disp.size(), num_partitions + 1 );
            send_disp.pop_back(); rx_disp.pop_back();

            // send basic matrix information
            // then send CSR
            N buffer[INITIAL_INFO_SIZE];
            N num_vals = matrices[i].col_idx.size();
            buffer[0] = num_vals;
            buffer[1] = matrices[i].row_ptrs.size() -1;
            buffer[2] = num_columns[i];
            buffer[3] = num_partitions;
            buffer[4] = send_info.size();
            buffer[5] = matrix->numColumns();
            std::cout << "DEBUG: Rank = " << i << std::endl;
            std::cout << "DEBUG: num_vals = " << num_vals <<
                " num_rows = " << matrices[i].row_ptrs.size() - 1 <<
                " num_columns = " << num_columns[i] <<
                " num_partitions = " << num_partitions <<
                " length of send vector = " << send_info.size() << "\n";

            if( i != 0 ) { // only send information if it is not available with me
            MPI::COMM_WORLD.Send( buffer, INITIAL_INFO_SIZE, MPI::UNSIGNED, i, 0 );
            MPI::COMM_WORLD.Send( &matrices[i].values[0],
                    num_vals, MPI::DOUBLE, i, 1);
            MPI::COMM_WORLD.Send( &matrices[i].col_idx[0],
                    num_vals, MPI::UNSIGNED, i, 2);
            MPI::COMM_WORLD.Send( &matrices[i].row_ptrs[0],
                    buffer[1] + 1, MPI::UNSIGNED, i, 3);
            MPI::COMM_WORLD.Send( &send_info[0], buffer[4], MPI::UNSIGNED, i, 4);
            MPI::COMM_WORLD.Send( &send_disp[0], num_partitions, MPI::UNSIGNED, i, 5);
            MPI::COMM_WORLD.Send( &rx_disp[0], num_partitions, MPI::UNSIGNED, i, 6);
            }
            else
            {
                partition_info.reset(
                        new ProcessPartitionInfo( matrix->numColumns(),
                            num_partitions, send_info.size() ) );
                partition_info->snd_vals = send_info;
                partition_info->snd_disp = send_disp;
                partition_info->rx_disp = rx_disp;
                p.matrix = CSRMatrix::create( num_columns[0],
                        matrices[i].values,
                        matrices[i].col_idx,
                        matrices[i].row_ptrs);
            }
        }

    }

    // need number of vals, number of rows, number of columns
    // number of partitions?
    DO_IF_NOT_RANK0 {

    N buffer[INITIAL_INFO_SIZE];
    MPI::COMM_WORLD.Recv( buffer, INITIAL_INFO_SIZE, MPI::UNSIGNED, 0, 0 ); // tagging as 0th comm
    // I need the matrix
    N num_vals = buffer[0], num_rows = buffer[1],
      num_columns = buffer[2], num_partitions = buffer[3],
      snd_vals_size = buffer[4], orig_columns = buffer[5];
    // std::cout << "DEBUG: num_vals = " << num_vals << " num_rows = " << num_rows <<
    //     " num_columns = " << num_columns << " num_partitions = " << num_partitions <<
    //     " rank = " << proc_info.rank << "\n";

    RVec vals( num_vals );
    NVec col_idxs( num_vals ), rows( num_rows + 1 );
    MPI::COMM_WORLD.Recv( &vals[0], num_vals, MPI::DOUBLE, 0, 1 );
    MPI::COMM_WORLD.Recv( &col_idxs[0], num_vals, MPI::UNSIGNED, 0, 2);
    MPI::COMM_WORLD.Recv( &rows[0], num_rows + 1, MPI::UNSIGNED, 0, 3);

    partition_info.reset( new ProcessPartitionInfo( orig_columns, num_partitions, snd_vals_size  ) );
    MPI::COMM_WORLD.Recv( &partition_info->snd_vals[0],
            snd_vals_size, MPI::UNSIGNED, 0, 4);
    MPI::COMM_WORLD.Recv( &partition_info->snd_disp[0],
            num_partitions, MPI::UNSIGNED, 0, 5);
    MPI::COMM_WORLD.Recv( &partition_info->rx_disp[0],
            num_partitions, MPI::UNSIGNED, 0, 6);
    p.matrix = CSRMatrix::create( num_columns, vals, col_idxs, rows );
    }
    p.partition_info = partition_info;

    return p;
}


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
    std::vector< NodePartitionInfo >& partition_map )
{
    // matrix and partition information => construct
    // the necessary data

    // preprocess here
    // find new CSRMatrix for all the different processes
    // find dependencies between processes

    DO_ONLY_AT_RANK0
    {
        N num_partitions = partition->getNumPartitions();
        std::vector < TempCSR > matrices( num_partitions );
        std::vector < ProcessPartitionInfo > proc_partitions( num_partitions );
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
            row_vec.push_back( col_vec.size() );
        }

        for( N i = 0; i < num_partitions; ++i  )
            for( N j = 0; j < num_partitions; ++j )
                printf("DEBUG: Partition %u depends on %lu "
                        "elements from Partition %u\n",
                        i , dependencies[i].dep_set[j].size() , j);

        for( N i = 0; i < num_partitions; ++i )
        {
            typedef std::set< N >::iterator It;
            std::set< N >& set_i = dependencies[i].dep_set[i];
            It it = set_i.begin(), end = set_i.end();
            N count = 0;
            for( ; it != end; ++it ) {
                N old_row = *it;
                partition_map[old_row].partition_id = row_partition[old_row];
                partition_map[old_row].row_id = count;
                ++count;
            }
            count = 0;
            std::map< N, N> big_to_small_map;
            for( N j = 0; j < num_partitions; ++j )
            {
                std::set< N >& set_ij = dependencies[i].dep_set[j];
                It it = set_ij.begin(), end = set_ij.end();
                for( ; it != end; ++it, ++count )
                    big_to_small_map.insert( std::make_pair( *it, count ) );
            }

            NVec& cols = matrices[i].col_idx;
            for( N j = 0; j < cols.size(); ++j )
                cols[j] = big_to_small_map[cols[j]];
        }

    }

    // need number of vals, number of rows, number of columns
    // number of partitions?
    N buffer[4];
    MPI::COMM_WORLD.Recv( buffer, 4, MPI::INT, 0, 0 ); // tagging as 0th comm
    // I need the matrix
    N num_vals = buffer[0], num_rows = buffer[1],
      num_columns = buffer[2], num_partitions = buffer[3];

    RVec vals( num_vals );
    NVec col_idxs( num_vals ), rows( num_rows + 1 );
    MPI::COMM_WORLD.Recv( &vals[0], num_vals, MPI::DOUBLE, 0, 1 );
    MPI::COMM_WORLD.Recv( &col_idxs[0], num_vals, MPI::UNSIGNED, 0, 2);
    MPI::COMM_WORLD.Recv( &rows[0], num_rows + 1, MPI::UNSIGNED, 0, 3);
    PreProcOutput p;
    p.matrix = CSRMatrix::create( num_columns, vals, col_idxs, rows );


    return p;


}

void PageRankMPI::calculatePageRank(
    const CSRMatrix& matrix,
    const ProcessPartitionInfo& partition,
    RVec& input,
    const ConvergenceCriterion& crit )
{

}

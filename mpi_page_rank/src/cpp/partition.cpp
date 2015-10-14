#include <partition.h>
#include <cassert>
#include <iostream>

using namespace PageRank;

ProcessPartitionInfo::ProcessPartitionInfo(
    N orig_columns,
    N orig_nodes,
    N num_procs,
    N snd_vals_size ): num_columns_original_matrix( orig_columns ),
    num_nodes_original_matrix( orig_nodes ),
    snd_vals( snd_vals_size ), snd_disp( num_procs ), rx_disp( num_procs ){};

Partition::CPtr Partition::createFromPartnVector(
    const NVecPtr& partition_map, N num_partitions )
{
    // store a pointer allocating a vector of
    // num_partitions, with each element set to a pointer containing
    // empty NVec Phew!!
    N num_nodes = partition_map->size();
    std::cout << "DEBUG: Number of nodes = " << num_nodes << "\n";
    return CPtr( new Partition(
                partition_map, num_partitions, num_nodes )  );
}

Partition::Partition(
    const NVecPtr& partition_map_,
    N num_partitions_,
    N num_nodes_)
    : partition_map( partition_map_ ),
    num_partitions( num_partitions_ ),
    num_nodes( num_nodes_ )
{
    // sanity checks
    assert( num_nodes == partition_map->size());
}

Partition::CPtr Partition::createFromStream(
    std::istream& iss )
{
    NVecPtr vec_ptr( new NVec() );
    N temp, num_parts = 0;
    while( iss >> temp ) {
        num_parts = ( num_parts < temp )? temp : num_parts;
        vec_ptr->push_back( temp );
    }
    std::cout << "DEBUG: Number of partitions = " << num_parts << "\n";
    // account for 0 indexing of partitions
    num_parts ++;
    return createFromPartnVector( vec_ptr, num_parts );

}

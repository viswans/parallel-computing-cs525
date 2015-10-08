#include <partition.h>
#include <cassert>
#include <iostream>

using namespace PageRank;

Partition::CPtr Partition::createFromPartnVector(
    const NVecPtr& partition_map, N num_partitions )
{
    // store a pointer allocating a vector of
    // num_partitions, with each element set to a pointer containing
    // empty NVec Phew!!
    N num_nodes = partition_map->size();
    VecNVecPtr partitions( new std::vector< NVecPtr >(
                num_partitions, NVecPtr( new NVec() ) ) );
    NodePartitionVecPtr node_part_info(
            new std::vector< NodePartitionInfo >( num_nodes ) );
    for( N i = 0 ; i < num_nodes; ++i  ) {
        N pid = partition_map->at(i);
        partitions->at( pid )->push_back( i );
        node_part_info->at( i ).partition_id = pid;
        node_part_info->at( i ).row_id = partitions->at(pid)->size();
    }
    return CPtr( new Partition(
                partition_map, node_part_info, partitions,
                num_partitions, num_nodes )  );
}

Partition::Partition(
    const NVecPtr& partition_map_,
    const NodePartitionVecPtr& node_partition_vec_,
    const VecNVecPtr& nodes_per_partition_,
    N num_partitions_,
    N num_nodes_)
    : partition_map( partition_map_ ),
    node_partition_vec( node_partition_vec_ ),
    nodes_per_partition( nodes_per_partition_ ),
    num_partitions( num_partitions_ ),
    num_nodes( num_nodes_ )
{
    // sanity checks
    assert( num_nodes == partition_map->size());
    assert( num_nodes == node_partition_vec->size() );
    assert( num_partitions == nodes_per_partition->size() );
}



NVecPtr  Partition::getNodesOfPartition( N i  ) const
{
     assert( i < num_partitions );
     return nodes_per_partition->at( i );
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
    return createFromPartnVector( vec_ptr, num_parts );

}

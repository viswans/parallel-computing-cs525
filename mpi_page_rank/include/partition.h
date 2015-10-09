#ifndef __PAGERANK_PARTITION__
#define __PAGERANK_PARTITION__

#include <types.h>

namespace PageRank {

    struct ProcessPartitionInfo
    {
        // snd_info[i] - all elements to send to proc i
        // rx_info[i] - all elements to receive from proc i
        typedef std::shared_ptr< const ProcessPartitionInfo > CPtr;
        N num_columns_original_matrix;
        std::vector< NVec > snd_info;
        std::vector< NVec > rx_info;
    };

    struct NodePartitionInfo
    {
        N partition_id, row_id;
    };

    class Partition
    {
     public:
        typedef std::shared_ptr< const Partition > CPtr;
        typedef std::shared_ptr< NVec > NVecPtr;
        typedef std::shared_ptr< std::vector< NVecPtr > > VecNVecPtr;
        typedef std::shared_ptr< std::vector< NodePartitionInfo > >
            NodePartitionVecPtr;

        static CPtr createFromStream( std::istream& iss );

        NVecPtr getPartitionMap() const { return partition_map; }
        NVecPtr getNodesOfPartition( N i ) const ;
        N       getNumNodes()      const { return num_nodes; }
        N       getNumPartitions() const { return num_partitions; }

        NodePartitionVecPtr getPartition() const { return node_partition_vec; }

     private:
        static CPtr createFromPartnVector(
            const NVecPtr& partition_map, N num_partitions );

        Partition(
            const NVecPtr& partition_map,
            const NodePartitionVecPtr& node_partition_vec,
            const VecNVecPtr& nodes_per_partition,
            N num_partitions_,
            N num_nodes);

        NVecPtr partition_map;
        NodePartitionVecPtr node_partition_vec;
        VecNVecPtr nodes_per_partition;
        N num_partitions;
        N num_nodes;
    };

}

#endif

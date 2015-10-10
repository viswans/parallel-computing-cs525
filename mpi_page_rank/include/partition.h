#ifndef __PAGERANK_PARTITION__
#define __PAGERANK_PARTITION__

#include <types.h>

namespace PageRank {

    struct NodePartitionInfo
    {
        N partition_id, row_id;
    };

    struct ProcessPartitionInfo
    {
        // snd_vals - all elements to send serially
        typedef std::shared_ptr< const ProcessPartitionInfo > CPtr;
        ProcessPartitionInfo(
            N orig_columns,
            N num_procs,
            N snd_vals_size);
        N num_columns_original_matrix;
        NVec snd_vals;
        NVec snd_disp;
        NVec rx_disp;
    };

    class Partition
    {
     public:
        typedef std::shared_ptr< const Partition > CPtr;

        static CPtr createFromStream( std::istream& iss );

        NVecPtr getPartitionMap() const { return partition_map; }
        N       getNumNodes()      const { return num_nodes; }
        N       getNumPartitions() const { return num_partitions; }


     private:
        static CPtr createFromPartnVector(
            const NVecPtr& partition_map, N num_partitions );

        Partition(
            const NVecPtr& partition_map,
            N num_partitions_,
            N num_nodes);

        NVecPtr partition_map;
        N num_partitions;
        N num_nodes;
    };

}

#endif

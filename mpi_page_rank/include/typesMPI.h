#ifndef __PAGERANK_TYPES_MPI__
#define __PAGERANK_TYPES_MPI__
#include <mpi.h>

// use this macro for doing stateful operations
// which return void
#define DO_ONLY_AT_RANK0 if(proc_info.rank == 0)
#define DO_IF_NOT_RANK0  if(proc_info.rank != 0)

// open namespace
struct ProcessInfo
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
};

extern ProcessInfo proc_info;

enum HelpTypes { eFormat, eFileNotExist };
void help( HelpTypes );

#endif

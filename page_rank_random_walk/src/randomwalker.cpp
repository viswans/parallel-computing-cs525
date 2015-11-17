#include <randomwalker.h>
#include <random>
#include <pthread.h>

using namespace PageRank;

namespace {

typedef N NodeId;

NodeId walkNext(
    NodeId start,
    const CSRMatrix::CPtr& matrix,
    N& seed)
{
    // std::uniform_int_distribution<N> dist( 0, matrix->numNeighbours(start)-1 );
    N rand_num = rand_r( &seed ) % matrix->numNeighbours( start );
    return matrix->getNthNeighbour( start, rand_num );
}

void walkBetween(
        N start_node, N end_node,
        const CSRMatrix::CPtr& matrix,
        N num_iterations,
        NVec& counter)
{
    if (counter.size() != matrix->getNumNodes())
        counter.resize( matrix->getNumNodes() );

    // std::random_device rd;
    // std::mt19937 gen(rd());
    N seed = rand() % 128;

    for( NodeId n = start_node; n < end_node; ++n  )
    {
        NodeId next = n;
        for( N iters = 0; iters < num_iterations; ++iters )
        {
            // std::cout << "DEBUG: Iteration = " << iters << "\n";
            next = walkNext( next, matrix, seed);
            counter[next]++;
        }
    }
}

struct ThreadStruct {
    NodeId start, end;
    CSRMatrix::CPtr matrix;
    N num_iterations;
    NVec* counter;
    ThreadStruct( NodeId s_, NodeId e_, const CSRMatrix::CPtr& m_,
            N niters_, NVec& c_):
        start(s_), end(e_), matrix(m_), num_iterations( niters_ ), counter( &c_ ) {}
    void walk() {
        walkBetween( start, end, matrix, num_iterations, *counter);
    }
};

void* walkThreads( void* input )
{
    ThreadStruct* in = (ThreadStruct*) input;
    in->walk();
    return NULL;
}

} // end of anon namespace

void RandomWalker::walk(
    const CSRMatrix::CPtr& matrix,
    N num_iterations,
    N num_threads,
    NVec& counter )
{
    N num_nodes = matrix->getNumNodes();
    srand (time(NULL));
    if( num_threads == 0 )
    {
        walkBetween( 0, num_nodes, matrix, num_iterations, counter );
        return;
    }

    std::vector<ThreadStruct> ts;
    pthread_t threads[num_threads];
    N chunk_size = num_nodes/num_threads + 1;
    N start = 0;
    for( N i = 0; i < num_threads; ++i )
    {
        N end = std::min( start + chunk_size, num_nodes );
        ts.push_back(ThreadStruct( start, end, matrix, num_iterations, counter ));
        start = end;
    }
    for( N i = 0; i < num_threads; ++i )
    {
        pthread_create( &threads[i], NULL, &walkThreads, (void*)(&ts[i]) );
    }
    for( N i = 0; i < num_threads; ++i )
    {
        pthread_join( threads[i], NULL );
    }

}


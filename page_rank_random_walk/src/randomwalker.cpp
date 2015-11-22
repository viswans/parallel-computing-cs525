#include <randomwalker.h>
#include <random>
#include <pthread.h>

using namespace PageRank;

namespace {

typedef N NodeId;

NodeId walkNext(
    NodeId start,
    const AdjacencyList::CPtr& list,
    std::mt19937& gen)
{
    std::uniform_int_distribution<N> dist( 0, list->numNeigbours(start)-1 );
    N rand_num = dist(gen);
    return list->getNthNeighbour( start, rand_num );
}

void walkBetween(
        N start_node, N end_node,
        const AdjacencyList::CPtr& adj_list,
        N num_iterations,
        NVec& counter)
{
    if (counter.size() != adj_list->getNumNodes())
        counter.resize( adj_list->getNumNodes() );

    std::random_device rd;
    std::mt19937 gen(rd());

    for( NodeId n = start_node; n < end_node; ++n  )
    {
        NodeId next = n;
        for( N iters = 0; iters < num_iterations; ++iters )
        {
            // std::cout << "DEBUG: Iteration = " << iters << "\n";
            next = walkNext( next, adj_list, gen);
            counter[next]++;
        }
    }
}

void addBetween(
        N start_node, N end_node, N num_threads,
        NVec* counter_threads[],
        NVec& counter)
{
    for( NodeId n = start_node; n < end_node; ++n  )
        for( N iters = 0; iters < num_threads; ++iters )
        {
            counter[n] += counter_threads[iters]->at(n);
        }
}

struct ThreadStruct {
    N thread_id, num_threads;
    NodeId start, end;
    AdjacencyList::CPtr list;
    N num_iterations;
    NVec* global_counter;
    NVec** local_counters;
    ThreadStruct( N tid_, N nts_, NodeId s_, NodeId e_, const AdjacencyList::CPtr& l_,
            N niters_, NVec& c_, NVec** lcounter_):
        thread_id( tid_ ), num_threads(nts_), start(s_), end(e_), list(l_), num_iterations( niters_ ), global_counter( &c_ ),
        local_counters( lcounter_ ) {}
    void walk() {
        walkBetween( start, end, list, num_iterations, *local_counters[thread_id]);
    }
    void add() {
        addBetween(start, end, num_threads, local_counters, *global_counter);
    }
};

void* walkThreads( void* input )
{
    ThreadStruct* in = (ThreadStruct*) input;
    in->walk();
    in->add();
    return NULL;
}

} // end of anon namespace

void RandomWalker::walk(
    const AdjacencyList::CPtr& adj_list,
    N num_iterations,
    N num_threads,
    NVec& counter )
{
    N num_nodes = adj_list->getNumNodes();
    if( num_threads == 0 )
    {
        walkBetween( 0, num_nodes, adj_list, num_iterations, counter );
        return;
    }

    std::vector<ThreadStruct> ts;
    pthread_t threads[num_threads];
    NVec* counter_threads[num_threads];
    N chunk_size = num_nodes/num_threads + 1;
    N start = 0;
    for( N i = 0; i < num_threads; ++i )
    {
        N end = std::min( start + chunk_size, num_nodes );
        counter_threads[i] = new NVec( num_nodes );
        ts.push_back(ThreadStruct( i, num_threads, start, end, adj_list,
                    num_iterations, counter, counter_threads ));
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


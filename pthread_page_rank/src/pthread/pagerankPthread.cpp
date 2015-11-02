#include <pagerankPth.h>
#include <cmath>
#include <cassert>
#include <utils.h>
#include <csrPthread.h>
#include <pthread.h>

#include <iostream>

using namespace PageRank;

namespace {
    pthread_barrier_t barr;

    struct ThreadStruct
    {
        N tid, start, end;
        CSRMatrixPthread matrix;
        ConvergenceCriterion criterion;
        RVec* input_buf;
        RVec* output_buf;
        ThreadStruct( N tid_, N start_, N end_,
                CSRMatrixPthread matrix_,
                ConvergenceCriterion c_, RVec* ip,
                RVec* op ): tid( tid_ ), start( start_ ), end( end_ ),
                matrix( matrix_ ), criterion( c_ ),
                input_buf( ip ), output_buf( op ) {}
        void doIteration();
    };

    R toldiff, iters, num_threads;
    RVec* output;
    RVec tol_threads;

    R sumOfSquares( const RVec& input, N start, N end )
    {
        R sum = 0;
        for( N i = start; i < end; ++i )
            sum += (input[i]* input[i]);
        return sum;
    }

    void ThreadStruct::doIteration()
    {
        N i = 0;
        R old_norm = 1e5, new_norm = 1e5, toldiff_local;
        while( ++i < criterion.max_iterations ) {
            matrix.multiply( *(input_buf), *(output_buf) ) ;
            // Utils::normalize( *(output_buf) );
            tol_threads[tid] = sumOfSquares( *output_buf, start, end );
            pthread_barrier_wait( &barr );
            new_norm = 0;
            for( N j = 0; j < num_threads; ++j )
                new_norm += tol_threads[j];
            new_norm = sqrt( new_norm );
            toldiff_local = std::abs( new_norm - old_norm );
            old_norm = new_norm;
            pthread_barrier_wait( &barr );
            if( toldiff_local <= criterion.tolerance ) {
                if( tid == 0 ) {
                    iters = i;
                    output = output_buf;
                    toldiff = toldiff_local;
                }
                break;
            }
            std::swap( input_buf, output_buf );
        };
    }

    void* pageRankIteration( void* pIter )
    {
        ThreadStruct* s = (ThreadStruct*)(pIter);
        s->doIteration();
        return NULL;
    }

}

void PageRankPthread::calculatePageRank (
    const CSRMatrix::CPtr& matrix,
    RVec& input,
    N num_threads,
    const ConvergenceCriterion& criterion)
{
    assert( matrix->numColumns() == input.size() );
    toldiff = 1e5;
    RVec *input_buf =  new RVec( input );
    RVec *output_buf =  new RVec( input.size() );
    const CSRMatrix* matrix_ptr = matrix.get();

    pthread_t threads[num_threads];
    ::num_threads = num_threads;
    tol_threads.resize( num_threads );
    std::vector< ThreadStruct > tstruct;
    pthread_barrier_init( &barr, NULL, num_threads );
    N start = 0, rows = matrix->numRows();
    N chunk = rows/num_threads + 1;
    for( N i = 0; i < num_threads; ++i )
    {
        N end = std::min( start + chunk, rows );
        tstruct.push_back( ThreadStruct( i, start, end,
                    CSRMatrixPthread( matrix_ptr, start, end ),
                    criterion, input_buf, output_buf) );
        start = end ;
    }

    for( N i = 0; i < num_threads; ++i )
        pthread_create( &threads[i], NULL, pageRankIteration, &tstruct[i] );

    for( N i = 0; i < num_threads; ++i )
        pthread_join( threads[i], NULL);
    input = *(output);

    if( iters == criterion.max_iterations )
        std::cout << "DEBUG: Terminated because of maxiterations with " <<
            " tolerancediff = " << toldiff << " and max_iterations = "
            << criterion.max_iterations << "\n";
    else
        std::cout << "DEBUG: Finished and converged on pagerank vector"
            " in " << iters << " iterations with ||Ax - x||_2 = " << toldiff << "\n";


}

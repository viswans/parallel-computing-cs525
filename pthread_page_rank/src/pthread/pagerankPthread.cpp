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
        N tid;
        CSRMatrixPthread matrix;
        ConvergenceCriterion criterion;
        RVec* input_buf;
        RVec* output_buf;
        ThreadStruct( N tid_, CSRMatrixPthread matrix_,
                ConvergenceCriterion c_, RVec* ip,
                RVec* op ): tid( tid_ ), matrix( matrix_ ), criterion( c_ ),
                input_buf( ip ), output_buf( op ) {}
        void doIteration();
    };

    R toldiff, iters;
    RVec* output;
    RVec tol_threads;

    void ThreadStruct::doIteration()
    {
        N i = 0;
        R old_norm = 1e5, new_norm = 1e5;
        while( ++i < criterion.max_iterations ) {
            matrix.multiply( *(input_buf), *(output_buf) ) ;
            // Utils::normalize( *(output_buf) );
            pthread_barrier_wait( &barr );
            if( tid == 0 ) {
                new_norm = sqrt( Utils::sumOfSquares( *(output_buf) ));
                toldiff = std::abs( new_norm - old_norm );
                std::cout << "DEBUG: iterations = " << i <<
                    " toldiff = " << toldiff << "\n";
                iters = i;
                output = output_buf;
                old_norm = new_norm;
            }
            pthread_barrier_wait( &barr );
            std::swap( input_buf, output_buf );
            if( toldiff <= criterion.tolerance ) break;
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
    std::vector< ThreadStruct > tstruct;
    pthread_barrier_init( &barr, NULL, num_threads );
    N start = 0, rows = matrix->numRows();
    N chunk = rows/num_threads + 1;
    for( N i = 0; i < num_threads; ++i )
    {
        N end = std::min( start + chunk, rows );
        tstruct.push_back( ThreadStruct( i, CSRMatrixPthread( matrix_ptr, start, end ),
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

#include <csr.h>
#include <pthread.h>

using namespace PageRank;

namespace {
    // anon namespace to put all pthread based code
    struct MultiplyPthread{
        const CSRMatrix* mat;
        const RVec* input_vector;
        RVec* output_vector;
        N start, end;
        N tid;
    };

    void* multiplyChunk( void* input )
    {
        // casting C style
        MultiplyPthread* in_ptr = (MultiplyPthread*)(input);
        MultiplyPthread& in = *in_ptr;
        // std::cout << "DEBUG: Thread id = " << in.tid << "\n";
        for( N i = in.start; i < in.end; ++i )
        {
            in.output_vector->at(i) = 0;
            for(N j = in.mat->getRowPtr()->at(i);
                  j < in.mat->getRowPtr()->at(i+1); ++j )
            {
                std::shared_ptr< std::vector< CSRMatrixEntry > > entries =
                    in.mat->getMatrixEntries();
                in.output_vector->at(i) +=  entries->at(j).value *
                    in.input_vector->at(entries->at(j).column_idx);
            }
        }
        return NULL;
    }

}



void CSRMatrix::multiplyPthread(
    const RVec& input_vector,
    N num_threads,
    RVec& output_vector ) const
{
    pthread_t threads[num_threads];
    N start = 0, chunk_size = numRows()/num_threads + 1,
      end, rows = numRows();
    MultiplyPthread thread_objs[num_threads];
    for( N t = 0; t < num_threads ; ++t )
    {
        end = std::min( start + chunk_size, rows );
        thread_objs[t].mat = this;
        thread_objs[t].input_vector = &input_vector;
        thread_objs[t].output_vector = &output_vector;
        thread_objs[t].start = start;
        thread_objs[t].end = end;
        thread_objs[t].tid = t;
        start = end;
    }
    for( N t = 0; t < num_threads ; ++t ){
        N rc = pthread_create(&threads[t], NULL, multiplyChunk, (void *)(&thread_objs[t]));
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // barrier
    for( N t = 0; t < num_threads; ++t )
        pthread_join( threads[t], NULL );
}

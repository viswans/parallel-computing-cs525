#ifndef __PAGERANK_UTILS__
#define __PAGERANK_UTILS__

#include <types.h>

namespace PageRank {
    const int defaultMaxIters = 1000;
    const double defaultTolerance = 1e-10;

    struct ConvergenceCriterion
    {
        double tolerance;
        unsigned int maxIterations;
        ConvergenceCriterion(
            int maxiters = defaultMaxIters,
            double tol = defaultTolerance )
            : tolerance( tol ), maxIterations( maxiters ) {}
    };

    class Utils
    {
     public:
        template< typename T>
        static void showVector(
            std::ostream& oss, const std::vector<T>& vec, std::string delim = ", ")
        {
            oss << "[" << vec.size() << "]: "; N i = 0;
            for ( ; i < vec.size()-1 ; ++i )
                oss << vec[i] << delim;
            oss << vec[i] << "\n";
        }
        static void writePageRank(
                std::ostream& oss,
                const RVec& page_rank_weight );

        static R sumOfSquares( const RVec& input );
        // normOfDiff only for vectors of same size
        static R normOfDiff( const RVec& v1, const RVec& v2 );
        static void normalize( RVec& input );
        static void calcCountFromDisp( const NVec& disp, N total, NVec& counts );
    };
}



#endif   // end of utils

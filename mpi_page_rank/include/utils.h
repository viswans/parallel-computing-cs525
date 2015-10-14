#ifndef __PAGERANK_UTILS__
#define __PAGERANK_UTILS__

#include <types.h>
#include <sys/time.h>

namespace PageRank {
    const int defaultMaxIters = 1000;
    const double defaultTolerance = 1e-10;

    // Stop watch
    class Timer
    {
     public:
        Timer(): time_spent(0) {}
        void start();
        void stop();
        double getTimeSpent() const { return time_spent; }
     private:
        timeval start_time, end_time;
        double time_spent;
    };

    struct ConvergenceCriterion
    {
        double tolerance;
        unsigned int max_iterations;
        ConvergenceCriterion(
            int maxiters = defaultMaxIters,
            double tol = defaultTolerance )
            : tolerance( tol ), max_iterations( maxiters ) {}
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

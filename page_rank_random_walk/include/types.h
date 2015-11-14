#ifndef __PAGERANK_TYPES__
#define __PAGERANK_TYPES__

#include <vector>
#include <iostream>
#include <memory>
#include <cassert>

#define ASSERT_EQUALS_MSG( arg1, arg2, msg )                            \
    if( (arg1) != (arg2) )  {                                           \
        std::cout << msg ;                                              \
        std::cout << #arg1"=" << (arg1) << #arg2"=" << (arg2) << "\n";  \
        assert( (arg1) == (arg2) );                                     \
    } \

#define ASSERT_EQUALS(arg1, arg2) ASSERT_EQUALS_MSG(arg1, arg2, "")

namespace PageRank {
    typedef unsigned int N;
    typedef double R;

    typedef std::vector<N> NVec;
    typedef std::vector<R> RVec;

    typedef std::shared_ptr<RVec> RVecPtr;
    typedef std::shared_ptr<NVec> NVecPtr;

    template< typename T >
    std::ostream& operator << ( std::ostream& s, const std::vector<T>& vec )
    {
        N i = 0, sz = vec.size();
        s << "Size = " << sz << " => ";
        for( ; i < sz; ++i ) s << vec[i] << ", ";
        return s;
    }
}

#endif

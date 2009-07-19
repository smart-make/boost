//  Copyright (c) 2005-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define FUSION_MAX_TUPLE_SIZE         10

#include <climits>

#include <iostream> 
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/inc.hpp>

#include <boost/spirit/include/karma.hpp>

#include "../high_resolution_timer.hpp"

///////////////////////////////////////////////////////////////////////////////
#define MAX_ITERATION         10000000
#define MAX_SEQUENCE_LENGTH   9
#define RCHAR(z, n, _)        char_((char)std::rand()) <<

#define SEQUENCE_TEST(z, N, _)                                                \
    {                                                                         \
        util::high_resolution_timer t;                                        \
                                                                              \
        std::srand(42);                                                       \
        for (int i = 0; i < MAX_ITERATION; ++i)                               \
        {                                                                     \
            char *ptr = buffer;                                               \
            generate(ptr, BOOST_PP_REPEAT(N, RCHAR, _) char_('\0'));          \
        }                                                                     \
                                                                              \
        std::cout << "karma::sequence(" << BOOST_PP_INC(N) << "):\t"          \
            << t.elapsed() << " [s]"                                          \
            << std::flush << std::endl;                                       \
    }                                                                         \
    /**/

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost::spirit::karma;
    using namespace boost::spirit::ascii;
    char buffer[512]; // we don't expect more than 512 bytes to be generated here

    std::cout << "Benchmarking sequence of different length: " << std::endl;
    BOOST_PP_REPEAT_FROM_TO(1, MAX_SEQUENCE_LENGTH, SEQUENCE_TEST, _);

    return 0;
}

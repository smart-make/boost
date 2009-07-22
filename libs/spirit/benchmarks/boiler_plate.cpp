/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include "measure.hpp"

namespace
{
    struct f
    {
        void benchmark(int x)
        {
            this->val += x; // Here is where you put code that you want
                            // to benchmark. Make sure it returns something.
                            // Anything.
        }

        int val;    // This is needed to avoid dead-code elimination
    };
}

int main()
{
    BOOST_SPIRIT_TEST_BENCHMARK(
        100000,     // This is the maximum repetitions to execute
        (f)         // Place your tests here. For now, we have only one test: (f)
                    // If you have 3 tests a, b and c, this line will contain (a)(b)(c)
    )
    
    // This is ultimately responsible for preventing all the test code
    // from being optimized away.  Change this to return 0 and you
    // unplug the whole test's life support system.
    return test::live_code != 0;
}


/*=============================================================================
    Phoenix V1.2.1
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/spirit/include/operators.hpp>
#include <boost/spirit/include/primitives.hpp>

using namespace std;
using namespace phoenix;

//////////////////////////////////
template <int N>
struct static_int {

    template <typename TupleT>
    struct result { typedef int type; };

    template <typename TupleT>
    int eval(TupleT const&) const { return N; }
};

//////////////////////////////////
template <int N>
phoenix::actor<static_int<N> >
int_const()
{
    return static_int<N>();
}

//////////////////////////////////
int
main()
{
    cout << (int_const<5>() + int_const<6>())() << endl;
    return 0;
}

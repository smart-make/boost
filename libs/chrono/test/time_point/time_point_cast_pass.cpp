//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//  Adaptation to Boost of the libcxx
//  Copyright 2010 Vicente J. Botet Escriba
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/chrono.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/lightweight_test.hpp>

#if !defined(BOOST_NO_STATIC_ASSERT)
#define NOTHING ""
#endif

template <class FromDuration, class ToDuration>
void
test(const FromDuration& df, const ToDuration& d)
{
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::time_point<Clock, FromDuration> FromTimePoint;
    typedef boost::chrono::time_point<Clock, ToDuration> ToTimePoint;
    FromTimePoint f(df);
    ToTimePoint t(d);
//~ #if defined(BOOST_NO_DECLTYPE)
    //~ typedef BOOST_TYPEOF_TPL(boost::chrono::time_point_cast<ToDuration>(f)) R;
//~ #else
    //~ typedef decltype(boost::chrono::time_point_cast<ToDuration>(f)) R;
//~ #endif        
    //~ BOOST_CHRONO_STATIC_ASSERT((boost::is_same<R, ToTimePoint>::value), NOTHING, ());
    BOOST_TEST(boost::chrono::time_point_cast<ToDuration>(f) == t);
}

int main()
{
    test(boost::chrono::milliseconds(7265000), boost::chrono::hours(2));
    test(boost::chrono::milliseconds(7265000), boost::chrono::minutes(121));
    test(boost::chrono::milliseconds(7265000), boost::chrono::seconds(7265));
    test(boost::chrono::milliseconds(7265000), boost::chrono::milliseconds(7265000));
    test(boost::chrono::milliseconds(7265000), boost::chrono::microseconds(7265000000LL));
    test(boost::chrono::milliseconds(7265000), boost::chrono::nanoseconds(7265000000000LL));
    test(boost::chrono::milliseconds(7265000),
         boost::chrono::duration<double, boost::ratio<3600> >(7265./3600));
    test(boost::chrono::duration<int, boost::ratio<2, 3> >(9),
         boost::chrono::duration<int, boost::ratio<3, 5> >(10));

    return boost::report_errors();
}

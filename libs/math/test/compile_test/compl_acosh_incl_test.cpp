//  Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Basic sanity check that header <boost/math/complex/acosh.hpp>
// #includes all the files that it needs to.
//
#include <boost/math/complex/acosh.hpp>

inline void check_result_imp(std::complex<float>, std::complex<float>){}
inline void check_result_imp(std::complex<double>, std::complex<double>){}
inline void check_result_imp(std::complex<long double>, std::complex<long double>){}

#include "test_compile_result.hpp"



void check()
{
   check_result<std::complex<float> >(boost::math::acosh(std::complex<float>()));
   check_result<std::complex<double> >(boost::math::acosh(std::complex<double>()));
   check_result<std::complex<long double> >(boost::math::acosh(std::complex<long double>()));
}



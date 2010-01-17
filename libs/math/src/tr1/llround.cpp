//  Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0.  (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#  include <pch.hpp>
#ifndef BOOST_MATH_TR1_SOURCE
#  define BOOST_MATH_TR1_SOURCE
#endif
#include <boost/math/tr1.hpp>
#include <boost/math/special_functions/round.hpp>
#include "c_policy.hpp"

namespace boost{ namespace math{ namespace tr1{

extern "C" boost::long_long_type BOOST_MATH_TR1_DECL llround BOOST_PREVENT_MACRO_SUBSTITUTION(double x)
{
   return c_policies::llround BOOST_PREVENT_MACRO_SUBSTITUTION(x);
}

}}}



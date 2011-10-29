//  is_evenly_divisible_by.hpp  --------------------------------------------------------------//

//  Copyright 2009-2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_CHRONO_DETAIL_IS_EVENLY_DIVISIBLE_BY_HPP
#define BOOST_CHRONO_DETAIL_IS_EVENLY_DIVISIBLE_BY_HPP

#include <boost/chrono/config.hpp>

#include <boost/mpl/logical.hpp>
#include <boost/ratio/ratio.hpp>


namespace boost {
namespace chrono {
namespace chrono_detail {

#if 1
  template <class R1, class R2>
  struct is_evenly_divisible_by : public boost::mpl::bool_ < ratio_divide<R1, R2>::type::den == 1 >
  {};
#else
  template <class R1, class R2, bool C1>
  struct is_evenly_divisible_by2 : public boost::mpl::bool_ < ratio_divide<R1, R2>::type::den == 1 >
  {};
  template <class R1, class R2>
  struct is_evenly_divisible_by2<R1,R2,true> : public boost::mpl::bool_ < false >
  {};
  template <class R1, class R2>
  struct is_evenly_divisible_by : public is_evenly_divisible_by2<R1, R2, (R1::num < R1::den && R2::den < R2::num) || (R1::den < R1::num && R2::num < R2::den)>
  {};
#endif
} // namespace chrono_detail
} // namespace detail
} // namespace chrono

#endif // BOOST_CHRONO_DETAIL_IS_EVENLY_DIVISIBLE_BY_HPP

//  boost/chrono/stopwatches/stopwatch_reporter.hpp
//  Copyright 2011 Vicente J. Botet Escriba
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or
//   copy at http://www.boost.org/LICENSE_1_0.txt)
//  See http://www.boost.org/libs/chrono/stopwatches for documentation.

#ifndef BOOST_CHRONO_STOPWATCHES_REPORTERS_THREAD_DEFAULT_FORMATTER_HPP
#define BOOST_CHRONO_STOPWATCHES_REPORTERS_THREAD_DEFAULT_FORMATTER_HPP

#if defined(BOOST_CHRONO_HAS_THREAD_CLOCK)

#include <boost/chrono/stopwatches/reporters/stopwatch_reporter_default_formatter.hpp>
#include <boost/chrono/stopwatches/formatters/elapsed_formatter.hpp>
#include <boost/chrono/stopwatches/simple_stopwatch.hpp>
#include <boost/chrono/thread_clock.hpp>

namespace boost
{
  namespace chrono
  {

    template<>
    struct stopwatch_reporter_default_formatter<simple_stopwatch<thread_clock> >
    {
      typedef elapsed_formatter type;
    };

    template<>
    struct wstopwatch_reporter_default_formatter<simple_stopwatch<thread_clock> >
    {
      typedef welapsed_formatter type;
    };

  } // namespace chrono
} // namespace boost


#endif

#endif



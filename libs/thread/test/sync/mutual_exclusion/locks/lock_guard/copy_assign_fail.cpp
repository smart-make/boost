//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/locks.hpp>

// template <class Mutex> class lock_guard;

// lock_guard& operator=(lock_guard const&) = delete;

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

boost::mutex m0;
boost::mutex m1;

int main()
{
  boost::lock_guard<boost::mutex> lk0(m0);
  boost::lock_guard<boost::mutex> lk1(m1);
  lk1 = lk0;

}


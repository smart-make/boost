//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/shared_mutex.hpp>

// class shared_mutex;

// shared_mutex(const shared_mutex&) = delete;

#include <boost/thread/shared_mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

int main()
{
  boost::shared_mutex m0;
  boost::shared_mutex m1(m0);
}

#include "libs/thread/test/remove_error_code_unused_warning.hpp"



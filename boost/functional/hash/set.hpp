
// Copyright 2005-2007 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18. 

#if !defined(BOOST_FUNCTIONAL_HASH_SET_HPP)
#define BOOST_FUNCTIONAL_HASH_SET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma message("Warning: boost/functional/hash/set.hpp is deprecated, use boost/functional/hash.hpp instead.")
#else
#warning "boost/functional/hash/set.hpp is deprecated, use boost/functional/hash.hpp instead."
#endif

#include <boost/functional/hash.hpp>

#endif

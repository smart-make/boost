//  boost/filesystem/config.hpp  ---------------------------------------------//

//  Copyright Beman Dawes 2003

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONFIG_HPP
#define BOOST_FILESYSTEM_CONFIG_HPP

#define BOOST_FILESYSTEM_I18N  // aid users wishing to compile several versions

//  ability to change namespace aids path_table.cpp  ------------------------// 
#ifndef BOOST_FILESYSTEM_NAMESPACE
# define BOOST_FILESYSTEM_NAMESPACE filesystem
#endif

#include <boost/config.hpp>
#include <boost/system/api_config.hpp>  // for BOOST_POSIX_API or BOOST_WINDOWS_API
#include <boost/detail/workaround.hpp> 

//  BOOST_POSIX_PATH or BOOST_WINDOWS_PATH specify which path syntax to recognise

# if defined(BOOST_WINDOWS_API) && defined(BOOST_POSIX_PATH)
#   error builds with Windows API do not support BOOST_POSIX_PATH
# endif

# if !defined(_WIN32) && !defined(__CYGWIN__) && defined(BOOST_WINDOWS_PATH)
#   error builds on non-Windows platforms do not support BOOST_WINDOWS_PATH
# endif

# if defined(BOOST_WINDOWS_PATH) && defined(BOOST_POSIX_PATH)
#   error both BOOST_WINDOWS_PATH and BOOST_POSIX_PATH are defined
# elif !defined(BOOST_WINDOWS_PATH) && !defined(BOOST_POSIX_PATH)
#   if !defined(BOOST_POSIX_PATH) && (defined(_WIN32) || defined(__CYGWIN__))
#     define BOOST_WINDOWS_PATH
#   else
#     define BOOST_POSIX_PATH
#   endif
# endif

//  narrow support only for badly broken compilers or libraries  -------------//

# if defined(BOOST_NO_STD_WSTRING) || defined(BOOST_NO_SFINAE) || defined(BOOST_NO_STD_LOCALE) || BOOST_WORKAROUND(__BORLANDC__, <0x610)
#   define BOOST_FILESYSTEM_NARROW_ONLY
# endif

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

//  enable dynamic linking ---------------------------------------------------//

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_FILESYSTEM_DYN_LINK)
# if defined(BOOST_FILESYSTEM_SOURCE)
#   define BOOST_FILESYSTEM_DECL BOOST_SYMBOL_EXPORT
# else 
#   define BOOST_FILESYSTEM_DECL BOOST_SYMBOL_IMPORT
# endif
#else
# define BOOST_FILESYSTEM_DECL
#endif

//  enable automatic library variant selection  ------------------------------// 

#if !defined(BOOST_FILESYSTEM_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_FILESYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define BOOST_LIB_NAME boost_filesystem
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_FILESYSTEM_DYN_LINK)
#  define BOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <boost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif // BOOST_FILESYSTEM_CONFIG_HPP

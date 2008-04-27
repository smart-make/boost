//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_ANONYMOUS_SHARED_MEMORY_HPP
#define BOOST_INTERPROCESS_ANONYMOUS_SHARED_MEMORY_HPP

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/move.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstddef>

#if (!defined(BOOST_WINDOWS)) || defined(BOOST_DISABLE_WIN32)
#  include <fcntl.h>        //open, O_CREAT, O_*... 
#  include <sys/mman.h>     //mmap
#  include <sys/stat.h>     //mode_t, S_IRWXG, S_IRWXO, S_IRWXU,
#else
#include <boost/interprocess/windows_shared_memory.hpp>
#endif


//!\file
//!Describes a function that creates anonymous shared memory that can be
//!shared between forked processes

namespace boost {
namespace interprocess {

/// @cond

namespace detail{

   class raw_mapped_region_creator
   {
      public:
      static move_return<mapped_region> create_posix_mapped_region(void *address, offset_t offset, std::size_t size)
      {
         mapped_region region;
         region.m_base = address;
         region.m_offset = offset;
         region.m_extra_offset = 0;
         region.m_size = size;
         return region;
      }
   };
}

/// @endcond

//!A function that creates an anonymous shared memory segment of size "size".
//!If "address" is passed the function will try to map the segment in that address.
//!Otherwise the operating system will choose the mapping address.
//!The function returns a mapped_region holding that segment or throws
//!interprocess_exception if the function fails.
static detail::move_return<mapped_region> anonymous_shared_memory(std::size_t size, void *address = 0)
#if (!defined(BOOST_WINDOWS)) || defined(BOOST_DISABLE_WIN32)
{
   int flags;
   int fd = -1;

   #if defined(MAP_ANONYMOUS) //Use MAP_ANONYMOUS
   flags = MAP_ANONYMOUS | MAP_SHARED;
   #elif !defined(MAP_ANONYMOUS) && defined(MAP_ANON) //use MAP_ANON
   flags = MAP_ANON | MAP_SHARED;
   #else // Use "/dev/zero"
   fd = open("/dev/zero", O_RDWR);
   flags = MAP_SHARED;
   if(fd == -1){
      error_info err = system_error_code();
      throw interprocess_exception(err);
   }
   #endif


   address = mmap( (void*)address
                    , size
                    , PROT_READ|PROT_WRITE
                    , flags
                    , fd
                    , 0);

   if(address == MAP_FAILED){
      if(fd != -1)   
         close(fd);
      error_info err = system_error_code();
      throw interprocess_exception(err);
   }

   if(fd != -1)   
      close(fd);

   return detail::raw_mapped_region_creator::create_posix_mapped_region(address, 0, size);
}
#else
{
   windows_shared_memory anonymous_mapping(create_only, 0, read_write, size);
   mapped_region region(anonymous_mapping, read_write, 0, size, address);
   return detail::move_return<mapped_region>(region);
}

#endif

}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_ANONYMOUS_SHARED_MEMORY_HPP

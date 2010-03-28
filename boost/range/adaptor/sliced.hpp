// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_SLICED_HPP
#define BOOST_RANGE_ADAPTOR_SLICED_HPP

#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/size_type.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost
{
    namespace adaptors
    {
		template< class RandomAccessRange >
		inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type > 
		slice( RandomAccessRange& rng, std::size_t t, std::size_t u )
		{
			BOOST_ASSERT( t <= u && "error in slice indices" );
            BOOST_ASSERT( static_cast<std::size_t>(boost::size(rng)) >= u && 
						  "second slice index out of bounds" );

			return make_iterator_range( rng, t, u - boost::size(rng) ); 
		}
		
		template< class RandomAccessRange >
		inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<const RandomAccessRange>::type >
		slice( const RandomAccessRange& rng, std::size_t t, std::size_t u )
		{
		    BOOST_ASSERT( t <= u && "error in slice indices" );
		    BOOST_ASSERT( static_cast<std::size_t>(boost::size(rng)) >= u &&
		                  "second slice index out of bounds" );
		                  
            return make_iterator_range( rng, t, u - boost::size(rng) );
		}
    } // 'adaptors'
    
	namespace range_detail
	{
		template< class T >
		struct slice_holder 
			: holder2<std::size_t> 
		{
			slice_holder( std::size_t t, std::size_t u ) 
			    : holder2<std::size_t>(t,u)
			{ }
		};
		
		template<class R, class H>
		inline iterator_range< BOOST_DEDUCED_TYPENAME range_iterator<R>::type >
		sliced_impl( R& r, const H& f)
		{
		    return adaptors::slice(r, f.val1, f.val2);
		}
		
		template< class RandomAccessRange, class Int >
		inline iterator_range< 
			     BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type > 
		operator|( RandomAccessRange& r, const slice_holder<Int>& f )
		{
			return sliced_impl( r, f );
		}

		template< class RandomAccessRange, class Int >
		inline iterator_range< 
				 BOOST_DEDUCED_TYPENAME range_iterator<const RandomAccessRange>::type > 
		operator|( const RandomAccessRange& r, const slice_holder<Int>& f )
		{
			return sliced_impl( r, f );
		}

	} // 'range_detail'

	namespace adaptors
	{ 
		namespace
		{
			const range_detail::forwarder2<range_detail::slice_holder> 
  	           sliced = range_detail::forwarder2<range_detail::slice_holder>();
		}
	}
	
}

#endif

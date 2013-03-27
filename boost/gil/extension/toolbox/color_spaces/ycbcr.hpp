﻿/*
    Copyright 2013 Juan V. Puertos G-Cluster, Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_COLOR_SPACES_YCBCR_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_COLOR_SPACES_YCBCR_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file ycbcr.hpp
/// \brief Support for ycbcr ITU.BT-601 color space
/// \author Juan V. Puertos G-Cluster 2013 \n
///
/// \date 2013 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/algorithm/clamp.hpp>
#include <boost/cast.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/toolbox/metafunctions/get_num_bits.hpp>

namespace boost{ namespace gil {

/// \addtogroup ColorNameModel
/// \{
namespace ycbcr_color_space
{
/// \brief Luminance
struct y_t {};    
/// \brief Blue chrominance component
struct cb_t {};
/// \brief Red chrominance component
struct cr_t {}; 
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3<ycbcr_color_space::y_t, ycbcr_color_space::cb_t, ycbcr_color_space::cr_t> ycbcr_601__t;

/// \ingroup LayoutModel
typedef boost::gil::layout<ycbcr_601__t> ycbcr_601__layout_t;

//The channel depth is ALWAYS 8bits ofr YCbCr!
GIL_DEFINE_ALL_TYPEDEFS(8,  ycbcr_601_)

/*
 * Source: http://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.601_conversion
 * (using values coming directly from ITU-R BT.601 recommendation)
 */

/**
* @brief Convert YCbCr ITU.BT-601 to RGB.
*/
template<>
struct default_color_converter_impl<ycbcr_601__t, rgb_t>
{
	// Note: the RGB_t channels range can be set later on by the users. We dont want to cast to bits8 or anything here.
	template < typename SRCP, typename DSTP >
	void operator()( const SRCP& src, DSTP& dst ) const
	{
        typedef channel_type< DSTP >::type dst_channel_t;
        convert( src, dst
               , boost::is_same< mpl::int_<8>::type, mpl::int_<8>::type >::type()
               );
	}

private:

    // optimization for bit8 channels
    template< typename Src_Pixel
            , typename Dst_Pixel
            >
    void convert( const Src_Pixel& src
                ,       Dst_Pixel& dst
                , mpl::true_ // is 8 bit channel
                ) const
    {
		using namespace boost::algorithm;
        using namespace boost::gil::ycbcr_color_space;

        typedef channel_type< Src_Pixel >::type src_channel_t;
        typedef channel_type< Dst_Pixel >::type dst_channel_t;

		src_channel_t y  = channel_convert<src_channel_t>( get_color(src,  y_t()));
		src_channel_t cb = channel_convert<src_channel_t>( get_color(src, cb_t()));
		src_channel_t cr = channel_convert<src_channel_t>( get_color(src, cr_t()));

		// The intermediate results of the formulas require at least 16bits of precission.
		boost::int_fast16_t c = y  - 16;
		boost::int_fast16_t d = cb - 128;
		boost::int_fast16_t e = cr - 128;
		boost::int_fast16_t red   = clamp((( 298 * c + 409 * e + 128) >> 8), 0, 255);
		boost::int_fast16_t green = clamp((( 298 * c - 100 * d - 208 * e + 128) >> 8), 0, 255);
		boost::int_fast16_t blue  = clamp((( 298 * c + 516 * d + 128) >> 8), 0, 255);

		get_color( dst,  red_t() )  = (dst_channel_t) red;
		get_color( dst, green_t() ) = (dst_channel_t) green;
		get_color( dst,  blue_t() ) = (dst_channel_t) blue;
    }


    template< typename Src_Pixel
            , typename Dst_Pixel
            >
    void convert( const Src_Pixel& s
                ,       Dst_Pixel& d
                , mpl::false_ // is 8 bit channel
                ) const
    {
        using namespace boost::algorithm;
        using namespace boost::gil::ycbcr_color_space;

        typedef channel_type< DSTP >::type dst_channel_t;

        double  y = get_color( src,  y_t() );
        double cb = get_color( src, cb_t() );
        double cr = get_color( src, cr_t() );

		get_color(dst, red_t())   = (dst_channel_t) clamp( 1.6438 * ( y - 16.0 ) + 1.5960 * ( cr -128.0 )
                                                         , 0.0
                                                         , 255.0
                                                         );

		get_color(dst, green_t()) = (dst_channel_t) clamp( 1.6438 * ( y - 16.0 ) - 0.3917 * ( cb - 128.0 ) + 0.8129 * ( cr -128.0 )
                                                         , 0.0
                                                         , 255.0
                                                         );

		get_color(dst, blue_t())  = (dst_channel_t) clamp( 1.6438 * ( y - 16.0 ) - 2.0172 * ( cb -128.0 )
                                                         , 0.0
                                                         , 255.0
                                                         );
    }
};

/*
 * Source: http://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.601_conversion
 * digital Y′CbCr derived from digital R'dG'dB'd 8 bits per sample, each using the full range.
 * with NO footroom wither headroom.
 */
/**
* @brief Convert RGB to YCbCr ITU.BT-601.
*/
template<>
struct default_color_converter_impl<rgb_t, ycbcr_601__t>
{
	template < typename SRCP, typename DSTP >
	void operator()( const SRCP& src, DSTP& dst ) const
	{
        using namespace boost::gil::ycbcr_color_space;

        typedef channel_type< SRCP >::type src_channel_t;
        typedef channel_type< DSTP >::type dst_channel_t;

		src_channel_t red   = channel_convert<src_channel_t>( get_color(src,   red_t()));
		src_channel_t green = channel_convert<src_channel_t>( get_color(src, green_t()));
		src_channel_t blue  = channel_convert<src_channel_t>( get_color(src,  blue_t()));

		double  y =  16.0 + 0.2567 * red  + 0.5041 * green + 0.0979 * blue;
		double cb = 128.0 - 0.1482 * red  - 0.2909 * green + 0.4392 * blue;
		double cr = 128.0 + 0.4392 * red  - 0.3677 * green - 0.0714 * blue;


		get_color( dst,  y_t() ) = (dst_channel_t)  y;
		get_color( dst, cb_t() ) = (dst_channel_t) cb;
		get_color( dst, cr_t() ) = (dst_channel_t) cr;  
	}
};

} // namespace gil
} // namespace boost

#endif

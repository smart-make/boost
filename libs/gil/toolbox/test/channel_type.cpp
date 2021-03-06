#include <boost/test/unit_test.hpp>

#include <boost/type_traits/is_same.hpp>

#include <boost/gil/extension/toolbox/metafunctions/channel_type.hpp>

using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( channel_type_test )

BOOST_AUTO_TEST_CASE( channel_type_test )
{
    BOOST_STATIC_ASSERT(( is_same< unsigned char, channel_type< rgb8_pixel_t >::type >::value ));

    // bits32f is a scoped_channel_value object
    BOOST_STATIC_ASSERT(( is_same< bits32f, channel_type< rgba32f_pixel_t >::type >::value ));

    // channel_type for bit_aligned images doesn't work with standard gil.
    typedef bit_aligned_image4_type<4, 4, 4, 4, rgb_layout_t>::type image_t;
    typedef channel_type< image_t::view_t::reference >::type channel_t;
}

BOOST_AUTO_TEST_SUITE_END()

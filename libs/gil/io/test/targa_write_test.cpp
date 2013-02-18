//#define BOOST_TEST_MODULE targa_write_test_module
#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/io/detail/typedefs.hpp>
#include <boost/gil/extension/io/targa_all.hpp>

#include "color_space_write_test.hpp"
#include "mandel_view.hpp"
#include "paths.hpp"

using namespace std;
using namespace boost::gil;

typedef targa_tag tag_t;

BOOST_AUTO_TEST_SUITE( targa_test )

#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
BOOST_AUTO_TEST_CASE( write_test )
{
    // test writing all supported image types
    {
        write_view( targa_out + "rgb8_test.tga"
                  , create_mandel_view( 200, 200
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  , tag_t()
                  );
    }

    {
        write_view( targa_out + "rgba8_test.tga"
                  , create_mandel_view( 200, 200
                                      , rgba8_pixel_t( 0,   0, 255, 0 )
                                      , rgba8_pixel_t( 0, 255,   0, 0 )
                                      )
                  , tag_t()
                  );
    }
}
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES

BOOST_AUTO_TEST_CASE( rgb_color_space_write_test )
{
    color_space_write_test< tag_t >( targa_out + "rgb_color_space_test.tga"
                                   , targa_out + "bgr_color_space_test.tga"
                                   );
}

BOOST_AUTO_TEST_SUITE_END()

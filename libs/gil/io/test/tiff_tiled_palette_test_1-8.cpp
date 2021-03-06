/********************************************************
 *
 * This test file will test palette tiled tiff reading
 *
 *******************************************************/
//#define BOOST_TEST_MODULE tiff_tiled_palette_test_1_8_module
#include <boost/test/unit_test.hpp>

#include "tiff_tiled_read_macros.hpp"

BOOST_AUTO_TEST_SUITE( tiff_test )

#ifdef BOOST_GIL_IO_USE_TIFF_GRAPHICSMAGICK_TEST_SUITE_IMAGES

BOOST_PP_REPEAT_FROM_TO(1, 9, GENERATE_TILE_STRIP_COMPARISON_PALETTE, palette )

#endif // BOOST_GIL_IO_USE_TIFF_GRAPHICSMAGICK_TEST_SUITE_IMAGES

BOOST_AUTO_TEST_SUITE_END()

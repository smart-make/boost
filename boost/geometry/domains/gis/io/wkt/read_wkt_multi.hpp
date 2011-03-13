// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2008-2009, Geodan, Amsterdam, the Netherlands.
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_DOMAINS_GIS_IO_WKT_READ_WKT_MULTI_HPP
#define BOOST_GEOMETRY_DOMAINS_GIS_IO_WKT_READ_WKT_MULTI_HPP

#include <string>

#include <boost/geometry/core/mutable_range.hpp>
#include <boost/geometry/multi/core/tags.hpp>
#include <boost/geometry/multi/core/point_type.hpp>

#include <boost/geometry/domains/gis/io/wkt/read_wkt.hpp>
#include <boost/geometry/domains/gis/io/wkt/detail/wkt_multi.hpp>


namespace boost { namespace geometry
{

namespace detail { namespace wkt {

template <typename MultiGeometry, template<typename> class Parser, typename PrefixPolicy>
struct multi_parser
{
    static inline void apply(std::string const& wkt, MultiGeometry& geometry)
    {
        traits::clear<MultiGeometry>::apply(geometry);

        tokenizer tokens(wkt, boost::char_separator<char>(" ", ",()"));
        tokenizer::iterator it;
        if (initialize<MultiGeometry>(tokens, PrefixPolicy::apply(), wkt, it))
        {
            handle_open_parenthesis(it, tokens.end(), wkt);

            // Parse sub-geometries
            while(it != tokens.end() && *it != ")")
            {
                traits::resize<MultiGeometry>::apply(geometry, boost::size(geometry) + 1);
                Parser
                    <
                        typename boost::range_value<MultiGeometry>::type
                    >::apply(it, tokens.end(), wkt, geometry.back());
                if (it != tokens.end() && *it == ",")
                {
                    // Skip "," after multi-element is parsed
                    ++it;
                }
            }

            handle_close_parenthesis(it, tokens.end(), wkt);
        }
    }
};




}} // namespace detail::wkt

#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{

template <typename MultiGeometry>
struct read_wkt<multi_point_tag, MultiGeometry>
    : detail::wkt::multi_parser
            <
                MultiGeometry,
                detail::wkt::point_parser,
                detail::wkt::prefix_multipoint
            >
{};


template <typename MultiGeometry>
struct read_wkt<multi_linestring_tag, MultiGeometry>
    : detail::wkt::multi_parser
            <
                MultiGeometry,
                detail::wkt::linestring_parser,
                detail::wkt::prefix_multilinestring
            >
{};


template <typename MultiGeometry>
struct read_wkt<multi_polygon_tag, MultiGeometry>
    : detail::wkt::multi_parser
            <
                MultiGeometry,
                detail::wkt::polygon_parser,
                detail::wkt::prefix_multipolygon
            >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_DOMAINS_GIS_IO_WKT_READ_WKT_MULTI_HPP

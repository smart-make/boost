#ifndef BOOST_GEOMETRY_PROJECTIONS_HAMMER_HPP
#define BOOST_GEOMETRY_PROJECTIONS_HAMMER_HPP

// Boost.Geometry - extensions-gis-projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright (c) 2008-2012 Barend Gehrels, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Boost.Geometry by Barend Gehrels

// Original copyright notice:
 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <boost/math/special_functions/hypot.hpp>

#include <boost/geometry/extensions/gis/projections/impl/base_static.hpp>
#include <boost/geometry/extensions/gis/projections/impl/base_dynamic.hpp>
#include <boost/geometry/extensions/gis/projections/impl/projects.hpp>
#include <boost/geometry/extensions/gis/projections/impl/factory_entry.hpp>

namespace boost { namespace geometry { namespace projections
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace hammer{ 

            struct par_hammer
            {
                double w;
                double m, rm;
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_hammer_spheroid : public base_t_f<base_hammer_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_hammer m_proj_parm;

                inline base_hammer_spheroid(const Parameters& par)
                    : base_t_f<base_hammer_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    double cosphi, d;
                
                    d = sqrt(2./(1. + (cosphi = cos(lp_lat)) * cos(lp_lon *= this->m_proj_parm.w)));
                    xy_x = this->m_proj_parm.m * d * cosphi * sin(lp_lon);
                    xy_y = this->m_proj_parm.rm * d * sin(lp_lat);
                }
            };

            // Hammer & Eckert-Greifendorff
            template <typename Parameters>
            void setup_hammer(Parameters& par, par_hammer& proj_parm)
            {
                if (pj_param(par.params, "tW").i) {
                    if ((proj_parm.w = fabs(pj_param(par.params, "dW").f)) <= 0.) throw proj_exception(-27);
                } else
                    proj_parm.w = .5;
                if (pj_param(par.params, "tM").i) {
                    if ((proj_parm.m = fabs(pj_param(par.params, "dM").f)) <= 0.) throw proj_exception(-27);
                } else
                    proj_parm.m = 1.;
                proj_parm.rm = 1. / proj_parm.m;
                proj_parm.m /= proj_parm.w;
                par.es = 0.;
                // par.fwd = s_forward;
            }

        }} // namespace detail::hammer
    #endif // doxygen 

    /*!
        \brief Hammer & Eckert-Greifendorff projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Miscellaneous
         - Spheroid
         - no inverse
         - W= M=
        \par Example
        \image html ex_hammer.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct hammer_spheroid : public detail::hammer::base_hammer_spheroid<Geographic, Cartesian, Parameters>
    {
        inline hammer_spheroid(const Parameters& par) : detail::hammer::base_hammer_spheroid<Geographic, Cartesian, Parameters>(par)
        {
            detail::hammer::setup_hammer(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class hammer_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    return new base_v_f<hammer_spheroid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void hammer_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("hammer", new hammer_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail 
    #endif // doxygen

}}} // namespace boost::geometry::projections

#endif // BOOST_GEOMETRY_PROJECTIONS_HAMMER_HPP


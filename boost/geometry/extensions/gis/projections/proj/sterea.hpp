#ifndef BOOST_GEOMETRY_PROJECTIONS_STEREA_HPP
#define BOOST_GEOMETRY_PROJECTIONS_STEREA_HPP

// Boost.Geometry - extensions-gis-projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright (c) 2008-2011 Barend Gehrels, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Boost.Geometry by Barend Gehrels (Geodan, Amsterdam)

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
#include <boost/geometry/extensions/gis/projections/impl/pj_gauss.hpp>

#include <boost/geometry/extensions/gis/projections/epsg_traits.hpp>

namespace boost { namespace geometry { namespace projection
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace sterea{ 
            static const double DEL_TOL = 1.e-14;
            static const int MAX_ITER = 10;

            struct par_sterea
            {
                double phic0;
                double cosc0, sinc0;
                double R2;
                gauss::GAUSS en;
            };
            
            
            

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_sterea_ellipsoid : public base_t_fi<base_sterea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_sterea m_proj_parm;

                inline base_sterea_ellipsoid(const Parameters& par)
                    : base_t_fi<base_sterea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    double cosc, sinc, cosl_, k;
                
                    detail::gauss::gauss(m_proj_parm.en, lp_lon, lp_lat);
                    sinc = sin(lp_lat);
                    cosc = cos(lp_lat);
                    cosl_ = cos(lp_lon);
                    k = this->m_par.k0 * this->m_proj_parm.R2 / (1. + this->m_proj_parm.sinc0 * sinc + this->m_proj_parm.cosc0 * cosc * cosl_);
                    xy_x = k * cosc * sin(lp_lon);
                    xy_y = k * (this->m_proj_parm.cosc0 * sinc - this->m_proj_parm.sinc0 * cosc * cosl_);
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    double rho, c, sinc, cosc;
                
                    xy_x /= this->m_par.k0;
                    xy_y /= this->m_par.k0;
                    if((rho = boost::math::hypot(xy_x, xy_y))) {
                        c = 2. * atan2(rho, this->m_proj_parm.R2);
                        sinc = sin(c);
                        cosc = cos(c);
                        lp_lat = asin(cosc * this->m_proj_parm.sinc0 + xy_y * sinc * this->m_proj_parm.cosc0 / rho);
                        lp_lon = atan2(xy_x * sinc, rho * this->m_proj_parm.cosc0 * cosc -
                            xy_y * this->m_proj_parm.sinc0 * sinc);
                    } else {
                        lp_lat = this->m_proj_parm.phic0;
                        lp_lon = 0.;
                    }
                    detail::gauss::inv_gauss(m_proj_parm.en, lp_lon, lp_lat);
                }
            };

            // Oblique Stereographic Alternative
            template <typename Parameters>
            void setup_sterea(Parameters& par, par_sterea& proj_parm)
            {
                    
             
            
                double R;
                proj_parm.en = detail::gauss::gauss_ini(par.e, par.phi0, proj_parm.phic0, R);
                proj_parm.sinc0 = sin(proj_parm.phic0);
                proj_parm.cosc0 = cos(proj_parm.phic0);
                proj_parm.R2 = 2. * R;
                // par.inv = e_inverse;
                // par.fwd = e_forward;
            }

        }} // namespace detail::sterea
    #endif // doxygen 

    /*!
        \brief Oblique Stereographic Alternative projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Azimuthal
         - Spheroid
         - Ellipsoid
        \par Example
        \image html ex_sterea.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct sterea_ellipsoid : public detail::sterea::base_sterea_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline sterea_ellipsoid(const Parameters& par) : detail::sterea::base_sterea_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::sterea::setup_sterea(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class sterea_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    return new base_v_fi<sterea_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void sterea_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("sterea", new sterea_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail 
    // Create EPSG specializations
    // (Proof of Concept, only for some)

    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2036, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=46.5 +lon_0=-66.5 +k=0.999912 +x_0=2500000 +y_0=7500000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2171, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=50.625 +lon_0=21.08333333333333 +k=0.9998 +x_0=4637000 +y_0=5647000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2172, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=53.00194444444445 +lon_0=21.50277777777778 +k=0.9998 +x_0=4603000 +y_0=5806000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2173, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=53.58333333333334 +lon_0=17.00833333333333 +k=0.9998 +x_0=3501000 +y_0=5999000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2174, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=51.67083333333333 +lon_0=16.67222222222222 +k=0.9998 +x_0=3703000 +y_0=5627000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2200, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=46.5 +lon_0=-66.5 +k=0.999912 +x_0=300000 +y_0=800000 +a=6378135 +b=6356750.304921594 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2290, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=47.25 +lon_0=-63 +k=0.999912 +x_0=700000 +y_0=400000 +a=6378135 +b=6356750.304921594 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2291, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=47.25 +lon_0=-63 +k=0.999912 +x_0=400000 +y_0=800000 +a=6378135 +b=6356750.304921594 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2292, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=47.25 +lon_0=-63 +k=0.999912 +x_0=400000 +y_0=800000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2953, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=46.5 +lon_0=-66.5 +k=0.999912 +x_0=2500000 +y_0=7500000 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2954, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=47.25 +lon_0=-63 +k=0.999912 +x_0=400000 +y_0=800000 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3120, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=50.625 +lon_0=21.08333333333333 +k=0.9998 +x_0=4637000 +y_0=5467000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3328, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=52.16666666666666 +lon_0=19.16666666666667 +k=0.999714 +x_0=500000 +y_0=500000 +ellps=krass +towgs84=33.4,-146.6,-76.3,-0.359,-0.053,0.844,-0.84 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<22780, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=34.2 +lon_0=39.15 +k=0.9995341 +x_0=0 +y_0=0 +a=6378249.2 +b=6356515 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<28991, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=52.15616055555555 +lon_0=5.38763888888889 +k=0.9999079 +x_0=0 +y_0=0 +ellps=bessel +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<28992, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=52.15616055555555 +lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<31600, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=45.9 +lon_0=25.39246588888889 +k=0.9996667 +x_0=500000 +y_0=500000 +ellps=intl +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<31700, LatLongRadian, Cartesian, Parameters>
    {
        typedef sterea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=sterea +lat_0=46 +lon_0=25 +k=0.99975 +x_0=500000 +y_0=500000 +ellps=krass +units=m";
        }
    };


    #endif // doxygen

}}} // namespace boost::geometry::projection

#endif // BOOST_GEOMETRY_PROJECTIONS_STEREA_HPP


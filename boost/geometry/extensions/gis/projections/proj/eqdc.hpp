#ifndef BOOST_GEOMETRY_PROJECTIONS_EQDC_HPP
#define BOOST_GEOMETRY_PROJECTIONS_EQDC_HPP

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
#include <boost/geometry/extensions/gis/projections/impl/pj_msfn.hpp>
#include <boost/geometry/extensions/gis/projections/impl/pj_mlfn.hpp>

namespace boost { namespace geometry { namespace projection
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace eqdc{
            static const double EPS10 = 1.e-10;

            struct par_eqdc
            {
                double phi1;
                double phi2;
                double n;
                double rho;
                double rho0;
                double c;
                double en[EN_SIZE];
                int        ellips;
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_eqdc_ellipsoid : public base_t_fi<base_eqdc_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                mutable par_eqdc m_proj_parm;

                inline base_eqdc_ellipsoid(const Parameters& par)
                    : base_t_fi<base_eqdc_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    this->m_proj_parm.rho = this->m_proj_parm.c - (this->m_proj_parm.ellips ? pj_mlfn(lp_lat, sin(lp_lat),
                        cos(lp_lat), this->m_proj_parm.en) : lp_lat);
                    xy_x = this->m_proj_parm.rho * sin( lp_lon *= this->m_proj_parm.n );
                    xy_y = this->m_proj_parm.rho0 - this->m_proj_parm.rho * cos(lp_lon);
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    if ((this->m_proj_parm.rho = boost::math::hypot(xy_x, xy_y = this->m_proj_parm.rho0 - xy_y)) != 0.0 ) {
                        if (this->m_proj_parm.n < 0.) {
                            this->m_proj_parm.rho = -this->m_proj_parm.rho;
                            xy_x = -xy_x;
                            xy_y = -xy_y;
                        }
                        lp_lat = this->m_proj_parm.c - this->m_proj_parm.rho;
                        if (this->m_proj_parm.ellips)
                            lp_lat = pj_inv_mlfn(lp_lat, this->m_par.es, this->m_proj_parm.en);
                        lp_lon = atan2(xy_x, xy_y) / this->m_proj_parm.n;
                    } else {
                        lp_lon = 0.;
                        lp_lat = this->m_proj_parm.n > 0. ? HALFPI : - HALFPI;
                    }
                }

                #ifdef SPECIAL_FACTORS_NOT_CONVERTED
                inline void fac(Geographic lp, Factors &fac) const
                {
                    double sinphi, cosphi;

                    sinphi = sin(lp_lat);
                    cosphi = cos(lp_lat);
                    this->m_fac.code |= IS_ANAL_HK;
                    this->m_fac.h = 1.;
                    this->m_fac.k = this->m_proj_parm.n * (this->m_proj_parm.c - (this->m_proj_parm.ellips ? pj_mlfn(lp_lat, sinphi,
                        cosphi, this->m_proj_parm.en) : lp_lat)) / pj_msfn(sinphi, cosphi, this->m_par.es);
                }
                #endif
            };

            // Equidistant Conic
            template <typename Parameters>
            void setup_eqdc(Parameters& par, par_eqdc& proj_parm)
            {
                double cosphi, sinphi;
                int secant;
                proj_parm.phi1 = pj_param(par.params, "rlat_1").f;
                proj_parm.phi2 = pj_param(par.params, "rlat_2").f;
                if (fabs(proj_parm.phi1 + proj_parm.phi2) < EPS10) throw proj_exception(-21);
                    pj_enfn(par.es, proj_parm.en);

                proj_parm.n = sinphi = sin(proj_parm.phi1);
                cosphi = cos(proj_parm.phi1);
                secant = fabs(proj_parm.phi1 - proj_parm.phi2) >= EPS10;
                if( (proj_parm.ellips = (par.es > 0.)) ) {
                    double ml1, m1;
                    m1 = pj_msfn(sinphi, cosphi, par.es);
                    ml1 = pj_mlfn(proj_parm.phi1, sinphi, cosphi, proj_parm.en);
                    if (secant) { /* secant cone */
                        sinphi = sin(proj_parm.phi2);
                        cosphi = cos(proj_parm.phi2);
                        proj_parm.n = (m1 - pj_msfn(sinphi, cosphi, par.es)) /
                            (pj_mlfn(proj_parm.phi2, sinphi, cosphi, proj_parm.en) - ml1);
                    }
                    proj_parm.c = ml1 + m1 / proj_parm.n;
                    proj_parm.rho0 = proj_parm.c - pj_mlfn(par.phi0, sin(par.phi0),
                        cos(par.phi0), proj_parm.en);
                } else {
                    if (secant)
                        proj_parm.n = (cosphi - cos(proj_parm.phi2)) / (proj_parm.phi2 - proj_parm.phi1);
                    proj_parm.c = proj_parm.phi1 + cos(proj_parm.phi1) / proj_parm.n;
                    proj_parm.rho0 = proj_parm.c - par.phi0;
                }
                // par.inv = e_inverse;
                // par.fwd = e_forward;
                // par.spc = fac;
            }

        }} // namespace detail::eqdc
    #endif // doxygen

    /*!
        \brief Equidistant Conic projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Conic
         - Spheroid
         - Ellipsoid
         - lat_1= lat_2=
        \par Example
        \image html ex_eqdc.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct eqdc_ellipsoid : public detail::eqdc::base_eqdc_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline eqdc_ellipsoid(const Parameters& par) : detail::eqdc::base_eqdc_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::eqdc::setup_eqdc(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class eqdc_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    return new base_v_fi<eqdc_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void eqdc_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("eqdc", new eqdc_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail
    #endif // doxygen

}}} // namespace boost::geometry::projection

#endif // BOOST_GEOMETRY_PROJECTIONS_EQDC_HPP


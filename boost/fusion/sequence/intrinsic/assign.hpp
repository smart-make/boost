/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ASSIGN_02162011_2308)
#define FUSION_ASSIGN_02162011_2308

#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/sequence/comparison/detail/equal_to.hpp>
#include <boost/config.hpp>

#if defined (BOOST_MSVC)
#  pragma warning(push)
#  pragma warning (disable: 4100) // unreferenced formal parameter
#endif

namespace boost { namespace fusion
{
    namespace detail
    {
        template <typename Seq1, typename Seq2>
        struct sequence_assign
        {
            typedef typename result_of::end<Seq1>::type end1_type;
            typedef typename result_of::end<Seq2>::type end2_type;

            template <typename I1, typename I2>
            static void
            call(I1 const&, I2 const&, mpl::true_)
            {
            }

            template <typename I1, typename I2>
            static void
            call(I1 const& src, I2 const& dest, mpl::false_)
            {
                *src = *dest;
                call(fusion::next(src), fusion::next(dest));
            }

            template <typename I1, typename I2>
            static bool
            call(I1 const& src, I2 const& dest)
            {
                typename result_of::equal_to<I1, end1_type>::type eq;
                return call(src, dest, eq);
            }
        };
    }

    template <typename Seq1, typename Seq2>
    inline bool
    assign(Seq1 const& src, Seq2& dest)
    {
        BOOST_STATIC_ASSERT(
            result_of::size<Seq1>::value == result_of::size<Seq2>::value);

        detail::sequence_assign<
            Seq1 const, Seq2>::
            call(fusion::begin(src), fusion::begin(dest));
    }
}}

#if defined (BOOST_MSVC)
#  pragma warning(pop)
#endif

#endif

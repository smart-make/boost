/*=============================================================================
    Copyright (c) 2009 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_PP_IS_ITERATING

#if !defined(BOOST_FUSION_NVIEW_IMPL_SEP_23_2009_1017PM)
#define BOOST_FUSION_NVIEW_IMPL_SEP_23_2009_1017PM

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

#define BOOST_PP_ITERATION_PARAMS_1                                           \
    (3, (1, FUSION_MAX_VECTOR_SIZE,                                           \
    "boost/fusion/view/nview/detail/nview_impl.hpp"))                         \
    /**/

#include BOOST_PP_ITERATE()

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace fusion { namespace result_of
{
    template <typename Sequence
      , BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(FUSION_MAX_VECTOR_SIZE, int I, -1)>
    struct as_nview
    {
        typedef mpl::vector_c<
            int, BOOST_PP_ENUM_PARAMS(FUSION_MAX_VECTOR_SIZE, I)
        > index_type;

        typedef nview<Sequence, index_type> type;
    };

}}}

#endif

///////////////////////////////////////////////////////////////////////////////
//  Preprocessor vertical repetition code
///////////////////////////////////////////////////////////////////////////////
#else // defined(BOOST_PP_IS_ITERATING)

#define N BOOST_PP_ITERATION()

namespace boost { namespace fusion
{
    template<BOOST_PP_ENUM_PARAMS(N, int I), typename Sequence>
    inline nview<Sequence, mpl::vector_c<int, BOOST_PP_ENUM_PARAMS(N, I)> >
    as_nview(Sequence& s)
    {
        typedef mpl::vector_c<int, BOOST_PP_ENUM_PARAMS(N, I)> index_type;
        return nview<Sequence, index_type>(s);
    }

}}

#undef N

#endif

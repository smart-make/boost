///////////////////////////////////////////////////////////////////////////////
// width_of.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_WIDTH_OF_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_WIDTH_OF_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/ref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/times.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>
#include <boost/xpressive/proto/traits.hpp>

///////////////////////////////////////////////////////////////////////////////
// add widths
#define BOOST_XPR_ADD_WIDTH_(X, Y)                                                                  \
    mpl::if_<                                                                                       \
        mpl::or_<mpl::equal_to<X, unknown_width>, mpl::equal_to<Y, unknown_width> >                 \
      , unknown_width                                                                               \
      , mpl::plus<X, Y >                                                                            \
    >::type

///////////////////////////////////////////////////////////////////////////////
// multiply widths
#define BOOST_XPR_MULT_WIDTH_(X, Y)                                                                 \
    mpl::if_<                                                                                       \
        mpl::or_<mpl::equal_to<X, unknown_width>, mpl::equal_to<Y, unknown_width> >                 \
      , unknown_width                                                                               \
      , mpl::times<X, Y >                                                                           \
    >::type

///////////////////////////////////////////////////////////////////////////////
// check widths for equality
#define BOOST_XPR_EQUAL_WIDTH_(X, Y)                                                                \
    mpl::if_<                                                                                       \
        mpl::equal_to<X, Y >                                                                        \
      , X                                                                                           \
      , unknown_width                                                                               \
    >::type

namespace boost { namespace xpressive { namespace detail
{

    ///////////////////////////////////////////////////////////////////////////////
    // width_of_terminal
    //
    template<typename Expr, bool IsXpr = is_xpr<Expr>::value>
    struct width_of_terminal
      : mpl::size_t<Expr::width>      // char literals
    {};

    template<typename Expr>
    struct width_of_terminal<Expr, false>
      : mpl::size_t<1>      // char literals
    {};

    template<typename Expr>
    struct width_of_terminal<Expr *, false>
      : unknown_width       // string literals
    {};

    template<typename Char, std::size_t N>
    struct width_of_terminal<Char (&) [N], false>
      : mpl::size_t<N-1>    // string literals
    {};

    template<typename Char, std::size_t N>
    struct width_of_terminal<Char const (&) [N], false>
      : mpl::size_t<N-1>    // string literals
    {};

    template<typename BidiIter>
    struct width_of_terminal<tracking_ptr<regex_impl<BidiIter> >, false>
      : unknown_width       // basic_regex
    {};

    template<typename BidiIter>
    struct width_of_terminal<reference_wrapper<basic_regex<BidiIter> >, false>
      : unknown_width       // basic_regex
    {};

    template<typename BidiIter>
    struct width_of_terminal<reference_wrapper<basic_regex<BidiIter> const>, false>
      : unknown_width       // basic_regex
    {};

    ///////////////////////////////////////////////////////////////////////////////
    // width_of
    //
    template<typename Expr, typename Tag = typename Expr::tag_type>
    struct width_of;

    template<typename Expr>
    struct width_of<Expr, proto::tag::terminal>
      : width_of_terminal<typename proto::result_of::arg<Expr>::type>
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::shift_right>
      : BOOST_XPR_ADD_WIDTH_(
            width_of<typename proto::result_of::left<Expr>::type>
          , width_of<typename proto::result_of::right<Expr>::type>
        )
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::bitwise_or>
      : BOOST_XPR_EQUAL_WIDTH_(
            width_of<typename proto::result_of::left<Expr>::type>
          , width_of<typename proto::result_of::right<Expr>::type>
        )
    {};

    template<typename Expr, typename Left>
    struct width_of_assign;

    template<typename Expr>
    struct width_of_assign<Expr, basic_mark_tag>
      : width_of<typename proto::result_of::right<Expr>::type>
    {};

    template<typename Expr>
    struct width_of_assign<Expr, set_initializer_type>
      : mpl::size_t<1>
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::assign>
      : width_of_assign<Expr, typename proto::result_of::left<Expr>::type>
    {};

    template<typename Expr>
    struct width_of<Expr, modifier_tag>
      : width_of<typename proto::result_of::right<Expr>::type>
    {};

    template<typename Expr>
    struct width_of<Expr, lookahead_tag>
      : mpl::size_t<0>
    {};

    template<typename Expr>
    struct width_of<Expr, lookbehind_tag>
      : mpl::size_t<0>
    {};

    // keep() is used to turn off backtracking, so they should only be used
    // for things that are variable-width (eg. quantified)
    template<typename Expr>
    struct width_of<Expr, keeper_tag>
      : unknown_width
    {
        // If this assert fires, you put something that doesn't require backtracking
        // in a keep(). In that case, the keep() is not necessary and you should just
        // remove it.
        BOOST_MPL_ASSERT_RELATION(width_of<typename proto::result_of::arg<Expr>::type>::value, ==, unknown_width::value);
    };

    template<typename Expr>
    struct width_of<Expr, proto::tag::posit>
      : unknown_width
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::dereference>
      : unknown_width
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::logical_not>
      : unknown_width
    {};

    template<typename Expr, uint_t Min, uint_t Max>
    struct width_of<Expr, generic_quant_tag<Min, Max> >
      : unknown_width
    {};

    template<typename Expr, uint_t Count>
    struct width_of<Expr, generic_quant_tag<Count, Count> >
      : BOOST_XPR_MULT_WIDTH_(width_of<typename proto::result_of::arg<Expr>::type>, mpl::size_t<Count>)
    {};

    template<typename Expr>
    struct width_of<Expr, proto::tag::negate>
      : width_of<typename proto::result_of::arg<Expr>::type>
    {};

    // when complementing a set or an assertion, the width is that of the set (1) or the assertion (0)
    template<typename Expr>
    struct width_of<Expr, proto::tag::complement>
      : width_of<typename proto::result_of::arg<Expr>::type>
    {};

    // The comma is used in list-initialized sets, and the width of sets are 1
    template<typename Expr>
    struct width_of<Expr, proto::tag::comma>
      : mpl::size_t<1>
    {};

    // The subscript operator[] is used for sets, as in set['a' | range('b','h')],
    // or for actions as in (any >> expr)[ action ]
    template<typename Expr, typename Left>
    struct width_of_subscript
      : width_of<Left>
    {};

    template<typename Expr>
    struct width_of_subscript<Expr, set_initializer_type>
      : mpl::size_t<1>
    {
        // If Left is "set" then make sure that Right has a width_of 1
        BOOST_MPL_ASSERT_RELATION(1, ==, width_of<typename proto::result_of::right<Expr>::type>::value);
    };

    template<typename Expr>
    struct width_of<Expr, proto::tag::subscript>
      : width_of_subscript<Expr, typename proto::result_of::left<Expr>::type>
    {};

}}} // namespace boost::xpressive::detail

#endif

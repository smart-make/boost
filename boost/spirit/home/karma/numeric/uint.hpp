//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_UINT_FEB_23_2007_0840PM)
#define BOOST_SPIRIT_KARMA_UINT_FEB_23_2007_0840PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <limits>

#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/string_traits.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/detail/get_encoding.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/auxiliary/lazy.hpp>
#include <boost/spirit/home/karma/detail/get_casetag.hpp>
#include <boost/spirit/home/karma/detail/extract_from.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/numeric/detail/numeric_utils.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/include/vector.hpp>

namespace boost { namespace spirit 
{
    namespace tag
    {
        template <typename T, unsigned Radix>
        struct uint_tag {};
    }

    namespace karma
    {
        ///////////////////////////////////////////////////////////////////////
        // This one is the class that the user can instantiate directly in 
        // order to create a customized int generator
        template <typename T = unsigned int, unsigned Radix = 10>
        struct uint_generator
          : spirit::terminal<tag::uint_tag<T, Radix> > 
        {};
    }

    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<karma::domain, tag::ushort_>   // enables ushort_
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::uint_>     // enables uint_
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::ulong_>    // enables ulong_
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::bin>       // enables bin
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::oct>       // enables oct
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, tag::hex>       // enables hex
      : mpl::true_ {};

#ifdef BOOST_HAS_LONG_LONG
    template <>
    struct use_terminal<karma::domain, tag::ulong_long> // enables ulong_long
      : mpl::true_ {};
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <>           // enables lit(unsigned short(0))
    struct use_terminal<karma::domain, unsigned short>
      : mpl::true_ {};

    template <>           // enables lit(0U)
    struct use_terminal<karma::domain, unsigned int>
      : mpl::true_ {};

    template <>           // enables lit(0UL)
    struct use_terminal<karma::domain, unsigned long>
      : mpl::true_ {};

#ifdef BOOST_HAS_LONG_LONG
    template <>           // enables lit(0ULL)
    struct use_terminal<karma::domain, boost::ulong_long_type> 
      : mpl::true_ {};
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <typename A0>
    struct use_terminal<karma::domain         // enables ushort_(...)
      , terminal_ex<tag::ushort_, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain         // enables uint_(...)
      , terminal_ex<tag::uint_, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain         // enables ulong_(...)
      , terminal_ex<tag::ulong_, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain         // enables bin(...)
      , terminal_ex<tag::bin, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain         // enables oct(...)
      , terminal_ex<tag::oct, fusion::vector1<A0> >
    > : mpl::true_ {};

    template <typename A0>
    struct use_terminal<karma::domain         // enables hex(...)
      , terminal_ex<tag::hex, fusion::vector1<A0> >
    > : mpl::true_ {};

#ifdef BOOST_HAS_LONG_LONG
    template <typename A0>
    struct use_terminal<karma::domain         // enables ulong_long(...)
      , terminal_ex<tag::ulong_long, fusion::vector1<A0> >
    > : mpl::true_ {};
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <>                               // enables *lazy* ushort_(...)
    struct use_lazy_terminal<karma::domain, tag::ushort_, 1> 
      : mpl::true_ {};

    template <>                               // enables *lazy* uint_(...)
    struct use_lazy_terminal<karma::domain, tag::uint_, 1> 
      : mpl::true_ {};

    template <>                               // enables *lazy* ulong_(...)
    struct use_lazy_terminal<karma::domain, tag::ulong_, 1> 
      : mpl::true_ {};

    template <>                               // enables *lazy* bin(...)
    struct use_lazy_terminal<karma::domain, tag::bin, 1> 
      : mpl::true_ {};

    template <>                               // enables *lazy* oct(...)
    struct use_lazy_terminal<karma::domain, tag::oct, 1> 
      : mpl::true_ {};

    template <>                               // enables *lazy* hex(...)
    struct use_lazy_terminal<karma::domain, tag::hex, 1> 
      : mpl::true_ {};

#ifdef BOOST_HAS_LONG_LONG
    template <>                               // enables *lazy* ulong_long(...)
    struct use_lazy_terminal<karma::domain, tag::ulong_long, 1> 
      : mpl::true_ {};
#endif

    ///////////////////////////////////////////////////////////////////////////
    // enables any custom uint_generator
    template <typename T, unsigned Radix>
    struct use_terminal<karma::domain, tag::uint_tag<T, Radix> >
      : mpl::true_ {};

    // enables any custom uint_generator(...)
    template <typename T, unsigned Radix, typename A0>
    struct use_terminal<karma::domain
      , terminal_ex<tag::uint_tag<T, Radix>, fusion::vector1<A0> >
    > : mpl::true_ {};

    // enables *lazy* custom uint_generator
    template <typename T, unsigned Radix>
    struct use_lazy_terminal<
        karma::domain
      , tag::uint_tag<T, Radix>
      , 1 // arity
    > : mpl::true_ {};

}} 

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    using spirit::ushort_;
    using spirit::ushort__type;
    using spirit::uint_;
    using spirit::uint__type;
    using spirit::ulong_;
    using spirit::ulong__type;
#ifdef BOOST_HAS_LONG_LONG
    using spirit::ulong_long;
    using spirit::ulong_long_type;
#endif
    using spirit::bin;
    using spirit::bin_type;
    using spirit::oct;
    using spirit::oct_type;
    using spirit::hex;
    using spirit::hex_type;

    using spirit::lit;    // lit(1U) is equivalent to 1U

    ///////////////////////////////////////////////////////////////////////////
    //  This specialization is used for unsigned int generators not having a 
    //  direct initializer: uint_, ulong_ etc. These generators must be used in
    //  conjunction with an Attribute.
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename CharEncoding, typename Tag, unsigned Radix>
    struct any_uint_generator
      : primitive_generator<any_uint_generator<T, CharEncoding, Tag, Radix> >
    {
        template <typename Context, typename Unused>
        struct attribute
        {
            typedef T type;
        };

        // check template Attribute 'Radix' for validity
        BOOST_SPIRIT_ASSERT_MSG(
            Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16,
            not_supported_radix, ());

        BOOST_SPIRIT_ASSERT_MSG(!std::numeric_limits<T>::is_signed,
            signed_unsigned_mismatch, ());

        // int has a Attribute attached
        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool
        generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr)
        {
            if (!traits::has_optional_value(attr))
                return false;       // fail if it's an uninitialized optional

            return int_inserter<Radix, CharEncoding, Tag>::
                        call(sink, traits::extract_from(attr)) &&
                   delimit_out(sink, d);      // always do post-delimiting
        }

        // this int has no Attribute attached, it needs to have been
        // initialized from a direct literal
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        generate(OutputIterator&, Context&, Delimiter const&, unused_type)
        {
            BOOST_SPIRIT_ASSERT_MSG(false, uint_not_usable_without_attribute, ());
            return false;
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("unsigned-integer");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  This specialization is used for unsigned int generators having a direct
    //  initializer: uint_(10), ulong_(20) etc.
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, typename CharEncoding, typename Tag, unsigned Radix
      , bool no_attribute>
    struct literal_uint_generator
      : primitive_generator<literal_uint_generator<T, CharEncoding, Tag, Radix
          , no_attribute> >
    {
        template <typename Context, typename Unused>
        struct attribute
          : mpl::if_c<no_attribute, unused_type, T>
        {};

        literal_uint_generator(typename add_const<T>::type n)
          : n_(n) {}

        // check template Attribute 'Radix' for validity
        BOOST_SPIRIT_ASSERT_MSG(
            Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16,
            not_supported_radix, ());

        BOOST_SPIRIT_ASSERT_MSG(!std::numeric_limits<T>::is_signed,
            signed_unsigned_mismatch, ());

        // A uint(1U) which additionally has an associated attribute emits
        // its immediate literal only if it matches the attribute, otherwise
        // it fails.
        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr) const
        {
            if (!traits::has_optional_value(attr) || 
                n_ != traits::extract_from(attr))
            {
                return false;
            }
            return int_inserter<Radix, CharEncoding, Tag>::call(sink, n_) &&
                   delimit_out(sink, d);      // always do post-delimiting
        }

        // A uint(1U) without any associated attribute just emits its 
        // immediate literal
        template <typename OutputIterator, typename Context, typename Delimiter>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , unused_type) const
        {
            return int_inserter<Radix, CharEncoding, Tag>::call(sink, n_) &&
                   delimit_out(sink, d);      // always do post-delimiting
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("unsigned-integer");
        }

        T n_;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T, typename Modifiers, unsigned Radix = 10>
        struct make_uint
        {
            static bool const lower = 
                has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
            static bool const upper = 
                has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

            typedef any_uint_generator<
                T
              , typename spirit::detail::get_encoding<
                    Modifiers, unused_type, lower || upper>::type
              , typename detail::get_casetag<Modifiers, lower || upper>::type
              , Radix
            > result_type;

            result_type operator()(unused_type, unused_type) const
            {
                return result_type();
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Modifiers>
    struct make_primitive<tag::ushort_, Modifiers> 
      : detail::make_uint<unsigned short, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<tag::uint_, Modifiers> 
      : detail::make_uint<unsigned int, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<tag::ulong_, Modifiers> 
      : detail::make_uint<unsigned long, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<tag::bin, Modifiers> 
      : detail::make_uint<unsigned, Modifiers, 2> {};

    template <typename Modifiers>
    struct make_primitive<tag::oct, Modifiers> 
      : detail::make_uint<unsigned, Modifiers, 8> {};

    template <typename Modifiers>
    struct make_primitive<tag::hex, Modifiers> 
      : detail::make_uint<unsigned, Modifiers, 16> {};

#ifdef BOOST_HAS_LONG_LONG
    template <typename Modifiers>
    struct make_primitive<tag::ulong_long, Modifiers> 
      : detail::make_uint<boost::ulong_long_type, Modifiers> {};
#endif

    template <typename T, unsigned Radix, typename Modifiers>
    struct make_primitive<tag::uint_tag<T, Radix>, Modifiers>
      : detail::make_uint<T, Modifiers, Radix> {};

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T, typename Modifiers, unsigned Radix = 10>
        struct make_uint_direct
        {
            static bool const lower = 
                has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
            static bool const upper = 
                has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

            typedef literal_uint_generator<
                T
              , typename spirit::detail::get_encoding<
                    Modifiers, unused_type, lower || upper>::type
              , typename detail::get_casetag<Modifiers, lower || upper>::type
              , Radix, false
            > result_type;

            template <typename Terminal>
            result_type operator()(Terminal const& term, unused_type) const
            {
                return result_type(fusion::at_c<0>(term.args));
            }
        };
    }

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::ushort_, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned short, Modifiers> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::uint_, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned int, Modifiers> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::ulong_, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned long, Modifiers> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::bin, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned, Modifiers, 2> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::oct, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned, Modifiers, 8> {};

    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::hex, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<unsigned, Modifiers, 16> {};

#ifdef BOOST_HAS_LONG_LONG
    template <typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<tag::ulong_long, fusion::vector1<A0> >, Modifiers>
      : detail::make_uint_direct<boost::ulong_long_type, Modifiers> {};
#endif

    template <typename T, unsigned Radix, typename A0, typename Modifiers>
    struct make_primitive<
        terminal_ex<tag::uint_tag<T, Radix>, fusion::vector1<A0> >
          , Modifiers>
      : detail::make_uint_direct<T, Modifiers, Radix> {};

    namespace detail
    {
        template <typename T, typename Modifiers>
        struct basic_uint_literal
        {
            static bool const lower =
                has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
            static bool const upper =
                has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

            typedef literal_uint_generator<
                T
              , typename spirit::detail::get_encoding<
                    Modifiers, unused_type, lower || upper>::type
              , typename detail::get_casetag<Modifiers, lower || upper>::type
              , 10, true
            > result_type;

            template <typename T_>
            result_type operator()(T_ i, unused_type) const
            {
                return result_type(i);
            }
        };
    }

    template <typename Modifiers>
    struct make_primitive<unsigned short, Modifiers> 
      : detail::basic_uint_literal<unsigned short, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<unsigned int, Modifiers> 
      : detail::basic_uint_literal<unsigned int, Modifiers> {};

    template <typename Modifiers>
    struct make_primitive<unsigned long, Modifiers> 
      : detail::basic_uint_literal<unsigned long, Modifiers> {};

#ifdef BOOST_HAS_LONG_LONG
    template <typename Modifiers>
    struct make_primitive<boost::ulong_long_type, Modifiers> 
      : detail::basic_uint_literal<boost::ulong_long_type, Modifiers> {};
#endif

}}}

#endif

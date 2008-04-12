/*=============================================================================
    Copyright (c) 1998-2003 Joel de Guzman
    Copyright (c) 2001 Daniel Nuffer
    Copyright (c) 2002 Hartmut Kaiser
    http://spirit.sourceforge.net/

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SEQUENCE_HPP)
#define BOOST_SPIRIT_SEQUENCE_HPP

#include <boost/spirit/home/classic/core/parser.hpp>
#include <boost/spirit/home/classic/core/primitives/primitives.hpp>
#include <boost/spirit/home/classic/core/composite/composite.hpp>
#include <boost/spirit/home/classic/meta/as_parser.hpp>

namespace boost { namespace spirit {

    ///////////////////////////////////////////////////////////////////////////
    //
    //  sequence class
    //
    //      Handles expressions of the form:
    //
    //          a >> b
    //
    //      where a and b are parsers. The expression returns a composite
    //      parser that matches a and b in sequence. One (not both) of the
    //      operands may be a literal char, wchar_t or a primitive string
    //      char const*, wchar_t const*.
    //
    //////////////////////////////////////////////////////////////////////////
    struct sequence_parser_gen;
    
    template <typename A, typename B>
    struct sequence : public binary<A, B, parser<sequence<A, B> > >
    {
        typedef sequence<A, B>                  self_t;
        typedef binary_parser_category          parser_category_t;
        typedef sequence_parser_gen             parser_generator_t;
        typedef binary<A, B, parser<self_t> >   base_t;
    
        sequence(A const& a, B const& b)
        : base_t(a, b) {}
    
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            if (result_t ma = this->left().parse(scan))
                if (result_t mb = this->right().parse(scan))
                {
                    scan.concat_match(ma, mb);
                    return ma;
                }
            return scan.no_match();
        }
    };
    
    struct sequence_parser_gen
    {
        template <typename A, typename B>
        struct result 
        {
            typedef
                sequence<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
    
        template <typename A, typename B>
        static sequence<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return sequence<BOOST_DEDUCED_TYPENAME as_parser<A>::type,
                BOOST_DEDUCED_TYPENAME as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    
    template <typename A, typename B>
    sequence<A, B>
    operator>>(parser<A> const& a, parser<B> const& b);
    
    template <typename A>
    sequence<A, chlit<char> >
    operator>>(parser<A> const& a, char b);
    
    template <typename B>
    sequence<chlit<char>, B>
    operator>>(char a, parser<B> const& b);
    
    template <typename A>
    sequence<A, strlit<char const*> >
    operator>>(parser<A> const& a, char const* b);
    
    template <typename B>
    sequence<strlit<char const*>, B>
    operator>>(char const* a, parser<B> const& b);
    
    template <typename A>
    sequence<A, chlit<wchar_t> >
    operator>>(parser<A> const& a, wchar_t b);
    
    template <typename B>
    sequence<chlit<wchar_t>, B>
    operator>>(wchar_t a, parser<B> const& b);
    
    template <typename A>
    sequence<A, strlit<wchar_t const*> >
    operator>>(parser<A> const& a, wchar_t const* b);
    
    template <typename B>
    sequence<strlit<wchar_t const*>, B>
    operator>>(wchar_t const* a, parser<B> const& b);
    
}} // namespace boost::spirit

#endif

#include <boost/spirit/home/classic/core/composite/impl/sequence.ipp>

/*=============================================================================
    Copyright (c) 2009 Francois Barel
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_REPOSITORY_QI_SUBRULE_AUGUST_06_2009_0239AM)
#define SPIRIT_REPOSITORY_QI_SUBRULE_AUGUST_06_2009_0239AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/qi/reference.hpp>
#include <boost/spirit/home/qi/nonterminal/detail/parser_binder.hpp>
#include <boost/spirit/home/support/argument.hpp>
#include <boost/spirit/home/support/assert_msg.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/nonterminal/extract_param.hpp>
#include <boost/spirit/home/support/nonterminal/locals.hpp>

#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/find_if.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_reference.hpp>

#if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable: 4355) // 'this' : used in base member initializer list warning
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<qi::domain, proto::tag::comma>  // enables ,
      : mpl::true_ {};

    template <>
    struct flatten_tree<qi::domain, proto::tag::comma>  // flattens ,
      : mpl::true_ {};
}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace repository { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    // subrule_context: special context used when parsing subrules, to pass
    // around the current set of subrule definitions (subrule_group)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Group, typename Attributes, typename Locals>
    struct subrule_context
      : context<Attributes, Locals>
    {
        typedef context<Attributes, Locals> base_type;
        typedef Group group_type;

        subrule_context(
            Group const& group
          , typename Attributes::car_type attribute
        ) : base_type(attribute), group(group)
        {
        }

        template <typename Args, typename Context>
        subrule_context(
            Group const& group
          , typename Attributes::car_type attribute
          , Args const& args
          , Context& caller_context
        ) : base_type(attribute, args, caller_context), group(group)
        {
        }

        subrule_context(Group const& group, Attributes const& attributes)
          : base_type(attributes), group(group)
        {
        }

        Group const& group;
    };

    ///////////////////////////////////////////////////////////////////////////
    // subrule_group:
    // - parser representing a group of subrule definitions (one or more),
    //   invokes first subrule on entry,
    // - also a Proto terminal, so that a group behaves like any Spirit
    //   expression.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Defs>
    struct subrule_group
      : proto::extends<
            typename proto::terminal<
                spirit::qi::reference<subrule_group<Defs> const>
            >::type
          , subrule_group<Defs>
        >
      , spirit::qi::parser<subrule_group<Defs> >
    {
        // Fusion associative sequence, associating each subrule ID in this
        // group (as an MPL integral constant) with its definition
        typedef Defs defs_type;

        typedef subrule_group<Defs> this_type;
        typedef spirit::qi::reference<this_type const> reference_;
        typedef typename proto::terminal<reference_>::type terminal;
        typedef proto::extends<terminal, this_type> base_type;

        static size_t const params_size =
            // Forward to first subrule.
            remove_reference<
                typename fusion::result_of::front<Defs>::type
            >::type::second_type::params_size;

        subrule_group(subrule_group const& rhs)
          : base_type(terminal::make(reference_(*this)))
          , defs(rhs.defs)
        {
        }

        subrule_group(Defs const& defs)
          : base_type(terminal::make(reference_(*this)))
          , defs(defs)
        {
        }

        // from a subrule ID, get the type of a reference to its definition
        template <int ID>
        struct def_type
        {
            typedef mpl::int_<ID> id_type;

            // If you are seeing a compilation error here, you are trying
            // to use a subrule which was not defined in this group.
            BOOST_SPIRIT_ASSERT_MSG(
                (fusion::result_of::has_key<
                    defs_type const, id_type>::type::value)
              , subrule_used_without_being_defined, (mpl::int_<ID>));

            typedef typename
                fusion::result_of::at_key<defs_type const, id_type>::type
            type;
        };

        // from a subrule ID, get a reference to its definition
        template <int ID>
        typename def_type<ID>::type def() const
        {
            return fusion::at_key<mpl::int_<ID> >(defs);
        }

        template <typename Context, typename Iterator>
        struct attribute
            // Forward to first subrule.
          : mpl::identity<
                typename remove_reference<
                    typename fusion::result_of::front<Defs>::type
                >::type::second_type::attr_type> {};

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            // Forward to first subrule.
            return parse_subrule(fusion::front(defs).second
              , first, last, context, skipper, attr);
        }

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute, typename Params>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr, Params const& params) const
        {
            // Forward to first subrule.
            return parse_subrule(fusion::front(defs).second
              , first, last, context, skipper, attr, params);
        }

        template <int ID, typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse_subrule_id(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            return parse_subrule(def<ID>()
              , first, last, context, skipper, attr);
        }

        template <int ID, typename Iterator, typename Context
          , typename Skipper, typename Attribute, typename Params>
        bool parse_subrule_id(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr, Params const& params) const
        {
            return parse_subrule(def<ID>()
              , first, last, context, skipper, attr, params);
        }

        template <typename Def
          , typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse_subrule(Def const& def
          , Iterator& first, Iterator const& last
          , Context& /*caller_context*/, Skipper const& skipper
          , Attribute& attr) const
        {
            // compute context type for this subrule
            typedef typename Def::locals_type subrule_locals_type;
            typedef typename Def::skipper_type subrule_skipper_type;
            typedef typename Def::attr_type subrule_attr_type;
            typedef typename Def::attr_reference_type subrule_attr_reference_type;
            typedef typename Def::parameter_types subrule_parameter_types;

            typedef
                subrule_context<
                    this_type
                  , fusion::cons<
                        subrule_attr_reference_type, subrule_parameter_types>
                  , subrule_locals_type
                >
            context_type;

            // prepare attribute
            typedef
                traits::make_attribute<subrule_attr_type, Attribute>
            make_attribute;
            typename make_attribute::type attr_ = make_attribute::call(attr);

            // If you are seeing a compilation error here, you are probably
            // trying to use a subrule which has inherited attributes,
            // without passing values for them.
            context_type context(*this, attr_);

            // If you are seeing a compilation error here stating that the
            // forth parameter can't be converted to a qi::reference
            // then you are probably trying to use a subrule with an
            // incompatible skipper type.
            return call_binder<subrule_skipper_type>(
                first, last, context, skipper, def.binder);
        }

        template <typename Def
          , typename Iterator, typename Context
          , typename Skipper, typename Attribute, typename Params>
        bool parse_subrule(Def const& def
          , Iterator& first, Iterator const& last
          , Context& caller_context, Skipper const& skipper
          , Attribute& attr, Params const& params) const
        {
            // compute context type for this subrule
            typedef typename Def::locals_type subrule_locals_type;
            typedef typename Def::skipper_type subrule_skipper_type;
            typedef typename Def::attr_type subrule_attr_type;
            typedef typename Def::attr_reference_type subrule_attr_reference_type;
            typedef typename Def::parameter_types subrule_parameter_types;

            typedef
                subrule_context<
                    this_type
                  , fusion::cons<
                        subrule_attr_reference_type, subrule_parameter_types>
                  , subrule_locals_type
                >
            context_type;

            // prepare attribute
            typedef
                traits::make_attribute<subrule_attr_type, Attribute>
            make_attribute;
            typename make_attribute::type attr_ = make_attribute::call(attr);

            // If you are seeing a compilation error here, you are probably
            // trying to use a subrule which has inherited attributes,
            // passing values of incompatible types for them.
            context_type context(*this, attr_, params, caller_context);

            // If you are seeing a compilation error here stating that the
            // forth parameter can't be converted to a qi::reference
            // then you are probably trying to use a subrule with an
            // incompatible skipper type.
            return call_binder<subrule_skipper_type>(
                first, last, context, skipper, def.binder);
        }

        // wrapper to let the incoming skipper be converted to the
        // skipper type expected by the subrule being invoked
        template <typename Skipper, typename Iterator
          , typename Context, typename Binder>
        bool call_binder(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Binder const& binder) const
        {
            return binder(first, last, context, skipper);
        }

        template <typename Context>
        info what(Context& context) const
        {
            // Forward to first subrule.
            return fusion::front(defs).second.binder.p.what(context);
        }

        // bring in the operator() overloads
        this_type const& get_parameterized_subject() const { return *this; }
        typedef this_type parameterized_subject_type;
        #include <boost/spirit/home/qi/nonterminal/detail/fcall.hpp>

        Defs defs;
    };

    ///////////////////////////////////////////////////////////////////////////
    // subrule_definition: holds one definition of a subrule
    ///////////////////////////////////////////////////////////////////////////
    template <
        int ID_
      , typename Locals
      , typename Skipper
      , typename Attr
      , typename AttrRef
      , typename Parameters
      , size_t ParamsSize
      , typename Subject
      , bool Auto_
    >
    struct subrule_definition
    {
        typedef mpl::int_<ID_> id_type;
        BOOST_STATIC_CONSTANT(int, ID = ID_);

        typedef Locals locals_type;
        typedef Skipper skipper_type;
        typedef Attr attr_type;
        typedef AttrRef attr_reference_type;
        typedef Parameters parameter_types;
        static size_t const params_size = ParamsSize;

        typedef Subject subject_type;
        typedef mpl::bool_<Auto_> auto_type;
        BOOST_STATIC_CONSTANT(bool, Auto = Auto_);

        typedef spirit::qi::detail::parser_binder<
            Subject, auto_type> binder_type;

        subrule_definition(Subject const& subject, std::string const& name)
          : binder(subject), name(name)
        {
        }

        binder_type const binder;
        std::string const name;
    };

    ///////////////////////////////////////////////////////////////////////////
    // subrule placeholder:
    // - on subrule definition: helper for creation of subrule_group,
    // - on subrule invocation: Proto terminal and parser.
    ///////////////////////////////////////////////////////////////////////////
    template <
        int ID_
      , typename T1 = unused_type
      , typename T2 = unused_type
      , typename T3 = unused_type
    >
    struct subrule
      : proto::extends<
            typename proto::terminal<
                spirit::qi::reference<subrule<ID_, T1, T2, T3> const>
            >::type
          , subrule<ID_, T1, T2, T3>
        >
      , spirit::qi::parser<subrule<ID_, T1, T2, T3> >
    {
        typedef mpl::int_<ID_> id_type;
        BOOST_STATIC_CONSTANT(int, ID = ID_);

        typedef subrule<ID_, T1, T2, T3> this_type;
        typedef spirit::qi::reference<this_type const> reference_;
        typedef typename proto::terminal<reference_>::type terminal;
        typedef proto::extends<terminal, this_type> base_type;

        typedef mpl::vector<T1, T2, T3> template_params;

        // locals_type is a sequence of types to be used as local variables
        typedef typename
            spirit::detail::extract_locals<template_params>::type
        locals_type;

        // The skip-parser type
        typedef typename
            spirit::detail::extract_component<
                spirit::qi::domain, template_params>::type
        skipper_type;

        typedef typename
            spirit::detail::extract_sig<template_params>::type
        sig_type;

        // This is the subrule's attribute type
        typedef typename
            spirit::detail::attr_from_sig<sig_type>::type
        attr_type;
        typedef typename add_reference<attr_type>::type attr_reference_type;

        // parameter_types is a sequence of types passed as parameters to the subrule
        typedef typename
            spirit::detail::params_from_sig<sig_type>::type
        parameter_types;

        static size_t const params_size =
            fusion::result_of::size<parameter_types>::type::value;

        explicit subrule(std::string const& name_ = "unnamed-subrule")
          : base_type(terminal::make(reference_(*this)))
          , name_(name_)
        {
        }

        // compute type of this subrule's definition for expr type Expr
        template <typename Expr, bool Auto>
        struct def_type_helper
        {
            // Report invalid expression error as early as possible.
            // If you got an error_invalid_expression error message here,
            // then the expression (Expr) is not a valid spirit qi expression.
            BOOST_SPIRIT_ASSERT_MATCH(spirit::qi::domain, Expr);

            typedef typename result_of::compile<
                spirit::qi::domain, Expr>::type subject_type;

            typedef subrule_definition<
                ID_
              , locals_type
              , skipper_type
              , attr_type
              , attr_reference_type
              , parameter_types
              , params_size
              , subject_type
              , Auto
            > const type;
        };

        // compute type of subrule group containing only this
        // subrule's definition for expr type Expr
        template <typename Expr, bool Auto>
        struct group_type_helper
        {
            typedef typename def_type_helper<Expr, Auto>::type def_type;

            // create Defs map with only one entry: (ID -> def)
            typedef typename
                fusion::result_of::make_map<id_type, def_type>::type
            defs_type;

            typedef subrule_group<defs_type> type;
        };

        template <typename Expr>
        typename group_type_helper<Expr, false>::type
        operator=(Expr const& expr) const
        {
            typedef group_type_helper<Expr, false> helper;
            typedef typename helper::def_type def_type;
            typedef typename helper::type result_type;
            return result_type(fusion::make_map<id_type>(
                def_type(compile<spirit::qi::domain>(expr), name_)));
        }

        template <typename Expr>
        friend typename group_type_helper<Expr, true>::type
        operator%=(subrule const& sr, Expr const& expr)
        {
            typedef group_type_helper<Expr, true> helper;
            typedef typename helper::def_type def_type;
            typedef typename helper::type result_type;
            return result_type(fusion::make_map<id_type>(
                def_type(compile<spirit::qi::domain>(expr), sr.name_)));
        }

        // non-const versions needed to suppress proto's %= kicking in
        template <typename Expr>
        friend typename group_type_helper<Expr, true>::type
        operator%=(subrule const& sr, Expr& expr)
        {
            return operator%=(
                sr
              , static_cast<Expr const&>(expr));
        }
        template <typename Expr>
        friend typename group_type_helper<Expr, true>::type
        operator%=(subrule& sr, Expr const& expr)
        {
            return operator%=(
                static_cast<subrule const&>(sr)
              , expr);
        }
        template <typename Expr>
        friend typename group_type_helper<Expr, true>::type
        operator%=(subrule& sr, Expr& expr)
        {
            return operator%=(
                static_cast<subrule const&>(sr)
              , static_cast<Expr const&>(expr));
        }

        std::string const& name() const
        {
            return name_;
        }

        void name(std::string const& str)
        {
            name_ = str;
        }

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef attr_type type;
        };

        template <typename Iterator, typename Group
          , typename Attributes, typename Locals
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , subrule_context<Group, Attributes, Locals>& context
          , Skipper const& skipper, Attribute& attr) const
        {
            return context.group.template parse_subrule_id<ID_>(
                first, last, context, skipper, attr);
        }

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& /*first*/, Iterator const& /*last*/
          , Context& /*context*/
          , Skipper const& /*skipper*/, Attribute& /*attr*/) const
        {
            // If you are seeing a compilation error here, you are trying
            // to use a subrule as a parser outside of a subrule group.
            BOOST_SPIRIT_ASSERT_MSG(false
              , subrule_used_outside_subrule_group, (id_type));

            return false;
        }

        template <typename Iterator, typename Group
          , typename Attributes, typename Locals
          , typename Skipper, typename Attribute
          , typename Params>
        bool parse(Iterator& first, Iterator const& last
          , subrule_context<Group, Attributes, Locals>& context
          , Skipper const& skipper, Attribute& attr
          , Params const& params) const
        {
            return context.group.template parse_subrule_id<ID_>(
                first, last, context, skipper, attr, params);
        }

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute
          , typename Params>
        bool parse(Iterator& /*first*/, Iterator const& /*last*/
          , Context& /*context*/
          , Skipper const& /*skipper*/, Attribute& /*attr*/
          , Params const& /*params*/) const
        {
            // If you are seeing a compilation error here, you are trying
            // to use a subrule as a parser outside of a subrule group.
            BOOST_SPIRIT_ASSERT_MSG(false
              , subrule_used_outside_subrule_group, (id_type));

            return false;
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info(name_);
        }

        // bring in the operator() overloads
        this_type const& get_parameterized_subject() const { return *this; }
        typedef this_type parameterized_subject_type;
        #include <boost/spirit/home/qi/nonterminal/detail/fcall.hpp>

        std::string name_;
    };
}}}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::comma, Elements, Modifiers>
    {
        // Elements is a Fusion sequence of reference<subrule_group<...> const>


        ///////////////////////////////////////////////////////////////////////
        // 1. confirm that, to avoid further confusion if it is not the case

        // this check is done with a metafunction class instead of an MPL
        // lambda expression with placeholders for the sake of gcc-3.x
        struct is_not_subrule_group
        {
            template <typename T>
            struct apply
            {
                typedef mpl::true_ type;
            };
            template <typename Defs>
            struct apply<reference<
                spirit::repository::qi::subrule_group<Defs> const> >
            {
                typedef mpl::false_ type;
            };
        };

        // If you are seeing a compilation error here, you are using a comma
        // (,) for something other than separating definitions of subrules.
        BOOST_SPIRIT_ASSERT_MSG(
            (is_same<
                typename fusion::result_of::find_if<Elements
                  , is_not_subrule_group>::type,
                typename fusion::result_of::end<Elements>::type>::value)
          , comma_not_separating_subrule_definitions, (Elements));


        ///////////////////////////////////////////////////////////////////////
        // 2. merge subrule groups together

        // function object applied on each element (reference to subrule_group)
        // with fusion::fold to compute the map of definitions for the
        // merged subrule_group
        struct merge_defs
        {
            template <typename Element, typename State>
            struct result_
            {
                // Note: it is not checked that any subrule is defined at most
                // once within a group (i.e. that keys are unique when joining
                // the two maps). If needed, this check could be added here.

                typedef
                    typename fusion::result_of::join<
                        State const
                      , typename Element::subject_type::defs_type const
                    >::type
                type;
            };

            template <typename Signature>
            struct result;
            template <typename Self, typename Element, typename State>
            struct result<Self(Element, State)>
              : result_<
                    typename remove_reference<Element>::type
                  , typename remove_reference<State>::type> {};

            template <typename Element, typename State>
            typename result_<Element, State>::type
            operator()(Element const& element, State const& state)
            {
                typedef typename
                    result_<Element, State>::type result_type;

                return fusion::join(
                    state
                  , element.ref.get().defs);
            }
        };

        typedef
            typename fusion::result_of::fold<
                Elements
              , typename fusion::result_of::make_map<>::type
              , merge_defs>::type
        merged_defs_type;

        typedef typename
            fusion::result_of::as_map<merged_defs_type>::type defs_type;


        typedef spirit::repository::qi::subrule_group<
            defs_type> result_type;

        result_type operator()(Elements const& elements, unused_type) const
        {
            return result_type(
                fusion::as_map(
                    fusion::fold(
                        elements
                      , fusion::make_map()
                      , merge_defs())));
        }
    };
}}}

#if defined(BOOST_MSVC)
# pragma warning(pop)
#endif

#endif

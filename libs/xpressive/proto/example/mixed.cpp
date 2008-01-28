//[ Mixed
///////////////////////////////////////////////////////////////////////////////
//  Copyright 2007 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This is an example of using BOOST_PROTO_DEFINE_OPERATORS to proto-ify
// expressions using std::vector<> and std::list, non-proto types. It is a port
// of the Mixed example from PETE.
// (http://www.codesourcery.com/pooma/download.html).

#include <list>
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <stdexcept>
#include <boost/xpressive/proto/proto.hpp>
#include <boost/xpressive/proto/debug.hpp>
#include <boost/xpressive/proto/context.hpp>
#include <boost/xpressive/proto/transform.hpp>
#include <boost/typeof/std/list.hpp>
#include <boost/typeof/std/vector.hpp>
#include <boost/typeof/std/complex.hpp>

namespace proto = boost::proto;
namespace mpl = boost::mpl;
using namespace proto;

template<typename Expr>
struct MixedExpr;

template<typename Iter>
struct iterator_wrapper
{
    explicit iterator_wrapper(Iter iter)
      : it(iter)
    {}

    Iter it;
};

struct begin : proto::callable
{
    template<class Sig>
    struct result;

    template<class This, class Cont>
    struct result<This(Cont)>
      : proto::result_of::as_expr<
            iterator_wrapper<typename boost::remove_reference<Cont>::type::const_iterator>
        >
    {};

    template<typename Cont>
    typename result<begin(Cont const &)>::type
    operator ()(Cont const &cont) const
    {
        iterator_wrapper<typename Cont::const_iterator> it(cont.begin());
        return proto::as_expr(it);
    }
};

// Here is a grammar that replaces vector and list terminals with their
// begin iterators
struct Begin
  : proto::or_<
        when< proto::terminal< std::vector<_, _> >, begin(_arg) >
      , when< proto::terminal< std::list<_, _> >, begin(_arg) >
      , when< proto::terminal<_> >
      , when< proto::nary_expr<_, proto::vararg<Begin> > >
    >
{};

// Here is an evaluation context that dereferences iterator
// terminals.
struct DereferenceCtx
{
    // Unless this is a vector terminal, use the
    // default evaluation context
    template<typename Expr, typename Arg = typename proto::result_of::arg<Expr>::type>
    struct eval
      : proto::default_eval<Expr, DereferenceCtx const>
    {};

    // Index vector terminals with our subscript.
    template<typename Expr, typename Iter>
    struct eval<Expr, iterator_wrapper<Iter> >
    {
        typedef typename std::iterator_traits<Iter>::reference result_type;

        result_type operator ()(Expr &expr, DereferenceCtx const &) const
        {
            return *proto::arg(expr).it;
        }
    };
};

// Here is an evaluation context that increments iterator
// terminals.
struct IncrementCtx
{
    // Unless this is a vector terminal, use the
    // default evaluation context
    template<typename Expr, typename Arg = typename proto::result_of::arg<Expr>::type>
    struct eval
      : proto::null_eval<Expr, IncrementCtx const>
    {};

    // Index vector terminals with our subscript.
    template<typename Expr, typename Iter>
    struct eval<Expr, iterator_wrapper<Iter> >
    {
        typedef void result_type;

        result_type operator ()(Expr &expr, IncrementCtx const &) const
        {
            ++proto::arg(expr).it;
        }
    };
};

// A grammar which matches all the assignment operators,
// so we can easily disable them.
struct AssignOps
  : proto::switch_<struct AssignOpsCases>
{};

// Here are the cases used by the switch_ above.
struct AssignOpsCases
{
    template<typename Tag, int D = 0> struct case_  : proto::not_<_> {};

    template<int D> struct case_< proto::tag::plus_assign, D >         : _ {};
    template<int D> struct case_< proto::tag::minus_assign, D >        : _ {};
    template<int D> struct case_< proto::tag::multiplies_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::divides_assign, D >      : _ {};
    template<int D> struct case_< proto::tag::modulus_assign, D >      : _ {};
    template<int D> struct case_< proto::tag::shift_left_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::shift_right_assign, D >  : _ {};
    template<int D> struct case_< proto::tag::bitwise_and_assign, D >  : _ {};
    template<int D> struct case_< proto::tag::bitwise_or_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::bitwise_xor_assign, D >  : _ {};
};

// A vector grammar is a terminal or some op that is not an
// assignment op. (Assignment will be handled specially.)
struct MixedGrammar
  : proto::or_<
        proto::terminal<_>
      , proto::and_<
            proto::nary_expr<_, proto::vararg<MixedGrammar> >
          , proto::not_<AssignOps>
        >
    >
{};

// Expressions in the vector domain will be wrapped in VectorExpr<>
// and must conform to the VectorGrammar
struct MixedDomain
  : proto::domain<proto::generator<MixedExpr>, MixedGrammar>
{};

// Here is MixedExpr, a wrapper for expression types in the MixedDomain.
template<typename Expr>
struct MixedExpr
  : proto::extends<Expr, MixedExpr<Expr>, MixedDomain>
{
    explicit MixedExpr(Expr const &expr)
      : proto::extends<Expr, MixedExpr<Expr>, MixedDomain>(expr)
    {}
private:
    // hide this:
    using proto::extends<Expr, MixedExpr<Expr>, MixedDomain>::operator [];
};

// Define a trait type for detecting vector and list terminals, to
// be used by the BOOST_PROTO_DEFINE_OPERATORS macro below.
template<typename T>
struct IsMixed
  : mpl::false_
{};

template<typename T, typename A>
struct IsMixed<std::list<T, A> >
  : mpl::true_
{};

template<typename T, typename A>
struct IsMixed<std::vector<T, A> >
  : mpl::true_
{};

namespace VectorOps
{
    // This defines all the overloads to make expressions involving
    // std::vector to build expression templates.
    BOOST_PROTO_DEFINE_OPERATORS(IsMixed, MixedDomain)

    struct assign_op
    {
        template<typename T, typename U>
        void operator ()(T &t, U const &u) const
        {
            t = u;
        }
    };

    struct plus_assign_op
    {
        template<typename T, typename U>
        void operator ()(T &t, U const &u) const
        {
            t += u;
        }
    };

    struct minus_assign_op
    {
        template<typename T, typename U>
        void operator ()(T &t, U const &u) const
        {
            t -= u;
        }
    };

    struct sin_
    {
        template<typename Sig>
        struct result;

        template<typename This, typename Arg>
        struct result<This(Arg)>
          : boost::remove_const<typename boost::remove_reference<Arg>::type>
        {};

        template<typename Arg>
        Arg operator ()(Arg const &arg) const
        {
            return std::sin(arg);
        }
    };

    template<typename A>
    typename proto::result_of::make_expr<
        proto::tag::function
      , MixedDomain
      , sin_ const
      , A const
    >::type sin(A const &a)
    {
        return proto::result_of::make_expr<
            proto::tag::function
          , MixedDomain
          , sin_ const
          , A const
        >::call(sin_(), a);
    }

    template<typename FwdIter, typename Expr, typename Op>
    void evaluate(FwdIter begin, FwdIter end, Expr const &expr, Op op)
    {
        int i = 0;
        IncrementCtx const inc = {};
        DereferenceCtx const deref = {};
        typename boost::result_of<Begin(Expr, int, int)>::type expr2 = Begin()(expr, i, i);
        for(; begin != end; ++begin)
        {
            op(*begin, proto::eval(expr2, deref));
            proto::eval(expr2, inc);
        }
    }

    // Add-assign to a vector from some expression.
    template<typename T, typename A, typename Expr>
    std::vector<T, A> &assign(std::vector<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), assign_op());
        return arr;
    }

    // Add-assign to a list from some expression.
    template<typename T, typename A, typename Expr>
    std::list<T, A> &assign(std::list<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), assign_op());
        return arr;
    }

    // Add-assign to a vector from some expression.
    template<typename T, typename A, typename Expr>
    std::vector<T, A> &operator +=(std::vector<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), plus_assign_op());
        return arr;
    }

    // Add-assign to a list from some expression.
    template<typename T, typename A, typename Expr>
    std::list<T, A> &operator +=(std::list<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), plus_assign_op());
        return arr;
    }

    // Minus-assign to a vector from some expression.
    template<typename T, typename A, typename Expr>
    std::vector<T, A> &operator -=(std::vector<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), minus_assign_op());
        return arr;
    }

    // Minus-assign to a list from some expression.
    template<typename T, typename A, typename Expr>
    std::list<T, A> &operator -=(std::list<T, A> &arr, Expr const &expr)
    {
        evaluate(arr.begin(), arr.end(), proto::as_expr<MixedDomain>(expr), minus_assign_op());
        return arr;
    }
}

int main()
{
    using namespace VectorOps;

    int n = 10;
    std::vector<int> a,b,c,d;
    std::list<double> e;
    std::list<std::complex<double> > f;

    int i;
    for(i = 0;i < n; ++i)
    {
        a.push_back(i);
        b.push_back(2*i);
        c.push_back(3*i);
        d.push_back(i);
        e.push_back(0.0);
        f.push_back(std::complex<double>(1.0, 1.0));
    }

    VectorOps::assign(b, 2);
    VectorOps::assign(d, a + b * c);
    a += if_else(d < 30, b, c);

    VectorOps::assign(e, c);
    e += e - 4 / (c + 1);

    f -= sin(0.1 * e * std::complex<double>(0.2, 1.2));

    std::list<double>::const_iterator ei = e.begin();
    std::list<std::complex<double> >::const_iterator fi = f.begin();
    for (i = 0; i < n; ++i)
    {
        std::cout
            << "a(" << i << ") = " << a[i]
            << " b(" << i << ") = " << b[i]
            << " c(" << i << ") = " << c[i]
            << " d(" << i << ") = " << d[i]
            << " e(" << i << ") = " << *ei++
            << " f(" << i << ") = " << *fi++
            << std::endl;
    }
}
//]

// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP
# define BOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP

# include <boost/preprocessor/cat.hpp>

# ifdef BOOST_OLD_CONCEPT_SUPPORT
#  include <boost/concept/detail/has_constraints.hpp>
#  include <boost/mpl/if.hpp>
# endif

// This implementation works on Comeau and GCC, all the way back to
// 2.95
namespace boost { namespace concept {

template <class ModelFn>
struct requirement_;

namespace detail
{
  template <void(*)()> struct instantiate {};
}

template <class Model>
struct requirement
{
    static void failed() { ((Model*)0)->~Model(); }
};

struct failed {};

# ifndef BOOST_NO_PARTIAL_SPECIALIZATION
template <class Model>
struct requirement<failed ************ Model::************>
{
    static void failed() { ((Model*)0)->~Model(); }
};
# endif

# ifdef BOOST_OLD_CONCEPT_SUPPORT

template <class Model>
struct constraint
{
    static void failed() { ((Model*)0)->constraints(); }
};
  
template <class Model>
struct requirement_<void(*)(Model)>
  : mpl::if_<
        concept::not_satisfied<Model>
      , constraint<Model>
# ifndef BOOST_NO_PARTIAL_SPECIALIZATION
      , requirement<failed ************ Model::************>
# else
      , requirement<Model>
# endif 
    >::type
{};
  
# else

// For GCC-2.x, these can't have exactly the same name
# ifndef BOOST_NO_PARTIAL_SPECIALIZATION
template <class Model>
struct requirement_<void(*)(Model)>
    : requirement<failed ************ Model::************>
{};
# else
template <class Model>
struct requirement_<void(*)(Model)>
    : requirement<Model>
{};
# endif 
  
# endif

#  define BOOST_CONCEPT_ASSERT_FN( ModelFnPtr )             \
    typedef ::boost::concept::detail::instantiate<          \
    &::boost::concept::requirement_<ModelFnPtr>::failed>    \
      BOOST_PP_CAT(boost_concept_check,__LINE__)

}}

#endif // BOOST_CONCEPT_DETAIL_GENERAL_DWA2006429_HPP

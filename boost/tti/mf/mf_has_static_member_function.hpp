
//  (C) Copyright Edward Diener 2011
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(TTI_MF_STATIC_MEMBER_FUNCTION_HPP)
#define TTI_MF_STATIC_MEMBER_FUNCTION_HPP

#include <boost/config.hpp>
#include <boost/function_types/property_tags.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/tti/gen/namespace_gen.hpp>
#include <boost/tti/detail/dtclass.hpp>

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

namespace boost
  {
  namespace tti
    {
  
    /// A metafunction which checks whether a static member function exists within an enclosing type.
    /**

        This metafunction takes its specific class types, except for the optional parameters, as nullary metafunctions whose typedef 'type' member is the actual type used.
    
        template<class TTI_HasStaticMemberFunction,class TTI_T,class TTI_R,class TTI_FS = boost::mpl::vector<>,class TTI_TAG = boost::function_types::null_tag>
        struct mf_has_static_member_function
          {
          static const value = unspecified;
          typedef mpl::bool_<true-or-false> type;
          };

        The metafunction types and return:

          TTI_HasStaticMemberFunction = a Boost MPL lambda expression using the metafunction generated from the BOOST_TTI_HAS_STATIC_MEMBER_FUNCTION ( or BOOST_TTI_TRAIT_HAS_STATIC_MEMBER_FUNCTION ) macro.<br />
                                        The easiest way to generate the lambda expression is to use a Boost MPL placeholder expression of the form 'metafunction\<_,_\> ( or optionally 'metafunction\<_,_,_\> or ' 'metafunction\<_,_,_,_\> )'.
                                        You can also use a metafunction class generated by boost::mpl::quote4.
                                    
          TTI_T               = the enclosing type as a nullary metafunction.
          
          TTI_R               = the return type of the static member function as a nullary metafunction.
                                If the type is not a class type it may be passed as is.
          
          TTI_FS              = an optional parameter which is the parameters of the static member function, each class type as a nullary metafunction, as a boost::mpl forward sequence.
                                This parameter defaults to boost::mpl::vector<>. Each parameter is a nullary 
                                metafunction, but if the type is a non-class type it may be passed as is.
          
          TTI_TAG             = an optional parameter which is a boost::function_types tag to apply to the static member function.
                                This parameter defaults to boost::function_types::null_tag.
      
          returns = 'value' is true if the static member function
                    exists within the enclosing type,
                    otherwise 'value' is false.
                          
    */
    template
      <
      class TTI_HasStaticMemberFunction,
      class TTI_T,
      class TTI_R,
      class TTI_FS = boost::mpl::vector<>,
      class TTI_TAG = boost::function_types::null_tag
      >
    struct mf_has_static_member_function :
      boost::mpl::apply
        <
        TTI_HasStaticMemberFunction,
        typename TTI_T::type,
        typename BOOST_TTI_NAMESPACE::detail::tclass<TTI_R>::type,
        typename boost::mpl::transform<TTI_FS,BOOST_TTI_NAMESPACE::detail::tclass<boost::mpl::_1> >::type,
        TTI_TAG
        >::type
      {
      };
    }
  }
  
#endif // TTI_MF_STATIC_MEMBER_FUNCTION_HPP

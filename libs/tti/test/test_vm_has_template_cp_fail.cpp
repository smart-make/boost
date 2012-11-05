
//  (C) Copyright Edward Diener 2011
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "test_vm_has_template_cp.hpp"
#include <boost/mpl/assert.hpp>

int main()
  {
  
#if BOOST_PP_VARIADICS

#if !defined(BOOST_TTI_VERSION_1_5)

  // TemplateNotExist does not exist at all
  
  BOOST_MPL_ASSERT((BOOST_TTI_HAS_TEMPLATE_GEN(TemplateNotExist)<AType>));
  
#else // BOOST_TTI_VERSION_1_5

  // TemplateNotExist does not exist at all
  
  BOOST_MPL_ASSERT((BOOST_TTI_VM_HAS_TEMPLATE_CHECK_PARAMS_GEN(TemplateNotExist)<AType>));
  
#endif // !BOOST_TTI_VERSION_1_5

#else
  
  BOOST_MPL_ASSERT((boost::mpl::false_));
  
#endif // BOOST_PP_VARIADICS

  return 0;

  }

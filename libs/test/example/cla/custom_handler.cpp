//  (C) Copyright Gennadiy Rozental 2001-2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

// Boost.Runtime.Param
#include <boost/test/utils/runtime/cla/named_parameter.hpp>
#include <boost/test/utils/runtime/cla/parser.hpp>

namespace rt  = boost::runtime;
namespace cla = boost::runtime::cla;

// STL
#include <iostream>

void validate_abcd( cla::parameter const&, int& value )
{
    if( value < 0 )
        value += 40;
}

int main() {
    char* argv[] = { "basic", "-abcd", "-25" };
    int argc = sizeof(argv)/sizeof(char*);

    try {
        cla::parser P;

        P << cla::named_parameter<int>( "abcd" ) - (cla::handler = &validate_abcd);

        P.parse( argc, argv );

        std::cout << "abcd = " << P.get<int>( "abcd" ) << std::endl;
    }
    catch( rt::logic_error const& ex ) {
        std::cout << "Logic error: " << ex.msg() << std::endl;
        return -1;
    }

    return 0;
}

// EOF

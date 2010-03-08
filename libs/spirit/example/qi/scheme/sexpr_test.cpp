﻿/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "sexpr.hpp"
#include "simple_print.hpp"
#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string source_code; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(source_code));

    typedef std::string::const_iterator iterator_type;
    iterator_type first = source_code.begin();
    iterator_type last = source_code.end();

    scheme::sexpr<iterator_type> p;
    scheme::white_space<iterator_type> ws;

    scheme::utree result;
    if (phrase_parse(first, last, p, ws, result))
    {
        std::cout << "success: ";
        println(result);
        std::cout << std::endl;
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}



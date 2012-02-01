//  Testing boost::lexical_cast with boost::container::string.
//
//  See http://www.boost.org for most recent version, including documentation.
//
//  Copyright Antony Polukhin, 2011.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/container/string.hpp>

void testing_boost_containers_basic_string();
void testing_boost_containers_string_std_string();

using namespace boost;

boost::unit_test::test_suite *init_unit_test_suite(int, char *[])
{
    unit_test::test_suite *suite =
        BOOST_TEST_SUITE("Testing boost::lexical_cast with boost::container::string");
    suite->add(BOOST_TEST_CASE(testing_boost_containers_basic_string));
    suite->add(BOOST_TEST_CASE(testing_boost_containers_string_std_string));

    return suite;
}

void testing_boost_containers_basic_string()
{       
    BOOST_CHECK("100" == lexical_cast<boost::container::string>("100"));
    BOOST_CHECK(L"100" == lexical_cast<boost::container::wstring>(L"100"));

    BOOST_CHECK("100" == lexical_cast<boost::container::string>(100));
    boost::container::string str("1000");
    BOOST_CHECK(1000 == lexical_cast<int>(str));
}

#if defined(BOOST_NO_STRINGSTREAM) || defined(BOOST_NO_STD_WSTRING)
#define BOOST_LCAST_NO_WCHAR_T
#endif

void testing_boost_containers_string_std_string()
{
    std::string std_str("std_str");
    boost::container::string boost_str("boost_str");
    BOOST_CHECK(boost::lexical_cast<std::string>(boost_str) == "boost_str");
    BOOST_CHECK(boost::lexical_cast<boost::container::string>(std_str) == "std_str");

#ifndef BOOST_LCAST_NO_WCHAR_T
    std::wstring std_wstr(L"std_wstr");
    boost::container::wstring boost_wstr(L"boost_wstr");

    BOOST_CHECK(boost::lexical_cast<std::wstring>(boost_wstr) == L"boost_wstr");
    BOOST_CHECK(boost::lexical_cast<boost::container::wstring>(std_wstr) == L"std_wstr");

#endif

}

/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <cctype>
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/qi/char/detail/range_run.hpp>
#include <boost/random.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/integer_traits.hpp>

#if defined(BOOST_MSVC)
# pragma warning(disable: 4800) // 'int' : forcing value to bool 'true' or 'false' warning
#endif

template <typename Char>
void acid_test()
{
    using boost::spirit::qi::detail::range_run;
    using boost::spirit::qi::detail::range;

    typedef boost::integer_traits<Char> integer_traits;
    Char const const_min = integer_traits::const_min;
    Char const const_max = integer_traits::const_max;
    int const test_size = 1000;

    boost::mt19937 rng;
    int min = const_min;
    int max = const_max;
    boost::uniform_int<> char_(min, max);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
       gen(rng, char_);
    boost::uniform_int<> _1of10(1, 10);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
       on_or_off(rng, _1of10);

    range_run<Char> rr;
    boost::dynamic_bitset<> bset(int(const_max)-int(const_min)+1);

    for (int i = 0; i < test_size; ++i)
    {
        range<Char> r = range<Char>(gen(), gen());
        if (r.first > r.last)
            std::swap(r.first, r.last);

        bool set = on_or_off() != 1;
        if (set)
            rr.set(r);
        else
            rr.clear(r);
        for (int j = r.first; j <= int(r.last); ++j)
            bset[j-const_min] = set;
    }

    for (int i = const_min; i <= int(const_max); ++i)
    {
        BOOST_TEST(rr.test(i) == bset[i-const_min]);
    }
}

int
main()
{
    using boost::spirit::qi::detail::range_run;
    using boost::spirit::qi::detail::range;

    {
        range_run<char> rr;
        rr.set(range<char>('a', 'a'));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(c == 'a' == rr.test(c));
        }
    }
    {
        range_run<char> rr;
        rr.set(range<char>('a', 'z'));
        rr.set(range<char>('A', 'Z'));
        rr.clear(range<char>('A', 'Z'));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(bool(std::islower(c)) == rr.test(c));
        }
    }
    {
        range_run<char> rr;
        rr.set(range<char>(0, 0));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(c == 0 == rr.test(c));
        }
        rr.set(range<char>(0, 50));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(((c >= 0) && (c <= 50)) == rr.test(c));
        }
    }
    {
        range_run<unsigned char> rr;
        rr.set(range<unsigned char>(255, 255));
        for (unsigned char c = 0; c < 255; ++c)
        {
            BOOST_TEST(c == 255 == rr.test(c));
        }
        rr.set(range<unsigned char>(250, 255));
        for (unsigned char c = 0; c < 255; ++c)
        {
            BOOST_TEST((c >= 250) == rr.test(c));
        }
    }
    {
        range_run<char> rr;
        rr.set(range<char>('a', 'z'));
        rr.set(range<char>('A', 'Z'));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(bool(std::isalpha(c)) == rr.test(c));
        }
    }
    {
        range_run<char> rr;
        rr.set(range<char>('a', 'z'));
        rr.set(range<char>('A', 'Z'));
        rr.clear(range<char>('J', 'j'));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST((bool(std::isalpha(c)) && (c < 'J' || c > 'j')) == rr.test(c));
        }
    }
    {
        range_run<char> rr;
        rr.set(range<char>(3, 3));
        rr.set(range<char>(1, 5));
        BOOST_TEST(rr.test(5));
    }
    {
        range_run<char> rr;
        for (char c = 0; c < 127; ++c)
        {
            if (c & 1)
            {
                rr.set(range<char>(c, c));
            }
        }
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST(bool((c & 1)) == rr.test(c));
        }
        rr.clear(range<char>(90, 105));
        for (char c = 0; c < 127; ++c)
        {
            BOOST_TEST((bool((c & 1)) && (c < 90 || c > 105)) == rr.test(c));
        }
    }
    {
        acid_test<char>();
        acid_test<signed char>();
        acid_test<unsigned char>();
        acid_test<wchar_t>();
        acid_test<short>();
        acid_test<signed short>();
        acid_test<unsigned short>();
    }

    return boost::report_errors();
}

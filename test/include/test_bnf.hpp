//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_TEST_BNF_HPP
#define BOOST_URL_TEST_BNF_HPP

#include <boost/url/bnf/char_set.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/string.hpp>
#include <initializer_list>

#include "test_suite.hpp"

namespace boost {
namespace urls {

template<class F>
void
for_each_char(F const& f)
{
    unsigned char u = 0;
    do
    {
        f(static_cast<
            char>(u));
    }
    while(++u != 0);
}

template<class CharSet>
void
test_char_set(
    CharSet const& cs,
    string_view s) noexcept
{
    // each char in s is in the set.
    for(char c : s)
        BOOST_TEST(cs(c));

    // number of chars in
    // set equals s.size()
    int n = 0;
    for_each_char(
    [&cs, &n](char c)
    {
        if(cs(c))
            ++n;
    });
    BOOST_TEST(n == s.size());

    // test find_if and find_if_not
    for_each_char(
    [&cs](char c)
    {
        if(cs(c))
        {
            BOOST_TEST(bnf::find_if(
                &c, &c+1, cs) == &c);
            BOOST_TEST(bnf::find_if_not(
                &c, &c+1, cs) == &c+1);
        }
        else
        {
            BOOST_TEST(bnf::find_if(
                &c, &c+1, cs) == &c+1);
            BOOST_TEST(bnf::find_if_not(
                &c, &c+1, cs) == &c);
        }
    });
}

template<class T>
void
bad(string_view s)
{
    BOOST_TEST_THROWS(
        bnf::validate<T>(s),
        std::exception);
    BOOST_TEST(! bnf::is_valid<T>(s));
}

template<class T>
void
bad(std::initializer_list<
    string_view> init)
{
    for(auto s : init)
        bad<T>(s);
}

template<class T>
void
good(string_view s)
{
    BOOST_TEST_NO_THROW(
        bnf::validate<T>(s));
    BOOST_TEST(bnf::is_valid<T>(s));
}

template<class T>
void
good(std::initializer_list<
    string_view> init)
{
    for(auto s : init)
        good<T>(s);
}

} // urls
} // boost

#endif

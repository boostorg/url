//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_TEST_TEST_BNF_HPP
#define BOOST_URL_TEST_TEST_BNF_HPP

#include <boost/url/bnf/algorithm.hpp>
#include <boost/url/string.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

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
good(string_view s)
{
    BOOST_TEST_NO_THROW(
        bnf::validate<T>(s));
    BOOST_TEST(bnf::is_valid<T>(s));
}

} // urls
} // boost

#endif

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

#include <boost/url/bnf/parse.hpp>
#include <boost/url/string.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

template<class T>
void
bad_(string_view s)
{
    BOOST_TEST_THROWS(
        bnf::validate_<T>(s),
        std::exception);
    BOOST_TEST(! bnf::is_valid_<T>(s));
}

template<class T>
void
good_(string_view s)
{
    BOOST_TEST_NO_THROW(
        bnf::validate_<T>(s));
    BOOST_TEST(bnf::is_valid_<T>(s));
}

} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/hexdig_chars.hpp>

#include <boost/url/grammar/charset.hpp>
#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

BOOST_STATIC_ASSERT(is_charset<decltype(
    hexdig_chars)>::value);

struct hexdig_chars_test
{
    void
    run()
    {
        test_char_set(
            hexdig_chars,
            "0123456789"
            "ABCDEF"
            "abcdef");

        for_each_char(
        [](char c)
        {
            char d;
            if(hexdig_chars(c))
                BOOST_TEST(
                    hexdig_value(c, d));
            else
                BOOST_TEST(
                    !hexdig_value(c, d));
        });
    }
};

TEST_SUITE(
    hexdig_chars_test,
    "boost.url.grammar.hexdig_chars");

} // grammar
} // urls
} // boost

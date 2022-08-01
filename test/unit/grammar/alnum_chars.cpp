//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/alnum_chars.hpp>

#include <boost/url/grammar/charset.hpp>
#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

BOOST_STATIC_ASSERT(is_charset<decltype(
    alnum_chars)>::value);

struct alnum_chars_test
{
    void
    run()
    {
        test_char_set(
            alnum_chars,
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz");
    }
};

TEST_SUITE(
    alnum_chars_test,
    "boost.url.grammar.alnum_chars");

} // grammar
} // urls
} // boost

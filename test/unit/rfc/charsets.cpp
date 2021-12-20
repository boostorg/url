//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/charsets.hpp>
#include <boost/static_assert.hpp>

#include "test_bnf.hpp"
#include "test_suite.hpp"

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(bnf::is_charset<unreserved_chars_t>::value);
BOOST_STATIC_ASSERT(bnf::is_charset<gen_delim_chars_t>::value);
BOOST_STATIC_ASSERT(bnf::is_charset<subdelim_chars_t>::value);
BOOST_STATIC_ASSERT(bnf::is_charset<pchars_t>::value);
BOOST_STATIC_ASSERT(bnf::is_charset<query_chars_t>::value);
BOOST_STATIC_ASSERT(bnf::is_charset<fragment_chars_t>::value);

class charsets_test
{
public:
    void
    testLuts()
    {
        test_char_set(
            unreserved_chars,
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789"
                "-._~");

        test_char_set(
            gen_delim_chars,
                ":/?#[]@");

        test_char_set(
            subdelim_chars,
                "!$&()*+,;=\x27");

        test_char_set(
            pchars,
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            "!$&'()*+,;="
            ":@");

        test_char_set(
            query_chars,
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            "!$&'()*+,;="
            ":@"
            "/?");

        test_char_set(
            fragment_chars,
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            "!$&'()*+,;="
            ":@"
            "/?");
    }

    void
    run()
    {
        testLuts();
    }
};

TEST_SUITE(
    charsets_test,
    "boost.url.charsets");

} // urls
} // boost

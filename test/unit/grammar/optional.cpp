//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/optional.hpp>

#include <boost/url/grammar/char_literal.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/elements.hpp>
#include <boost/url/grammar/token.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct optional_test
{
    void testParse()
    {
        {
            optional<token<alnum_chars_t>> t;
            BOOST_TEST_NO_THROW(
                parse_string("xy", t));
        }
        {
            elements<
                optional<token<alnum_chars_t>>,
                char_literal<' '>> t;
            BOOST_TEST_NO_THROW(
                parse_string("xy ", t));
            BOOST_TEST(get<0>(t).has_value);
            BOOST_TEST(*get<1>(t).p == ' ');
        }
        {
            elements<
                optional<token<alnum_chars_t>>,
                char_literal<' '>> t;
            BOOST_TEST_NO_THROW(
                parse_string(" ", t));
            BOOST_TEST(! get<0>(t).has_value);
            BOOST_TEST(*get<1>(t).p == ' ');
        }
    }

    void
    run()
    {
        testParse();
    }
};

TEST_SUITE(
    optional_test,
    "boost.url.grammar.optional");

} // grammar
} // urls
} // boost

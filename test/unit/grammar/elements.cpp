//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/elements.hpp>

#include "test_suite.hpp"

#include <boost/url/grammar/char_literal.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/token.hpp>

namespace boost {
namespace urls {
namespace grammar {

struct elements_test
{
    void
    testParse()
    {
        {
            elements<
                char_literal<'x'>,
                char_literal<'y'>> t;
            BOOST_TEST_NO_THROW(
                parse_string("xy", t));
            BOOST_TEST(*get<0>(t).p == 'x');
            BOOST_TEST(*get<1>(t).p == 'y');
        }
        {
            elements<token<alnum_chars_t>> t;
            BOOST_TEST_NO_THROW(
                parse_string("token", t));
            BOOST_TEST(get<0>(t).s == "token");
        }
    }

    void
    run()
    {
        testParse();
    }
};

TEST_SUITE(
    elements_test,
    "boost.url.grammar.elements");

}
} // urls
} // boost

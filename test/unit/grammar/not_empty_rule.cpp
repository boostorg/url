//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/not_empty_rule.hpp>

#include <boost/url/grammar/digit_chars.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/unreserved_chars.hpp>

#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct not_empty_rule_test
{
    void
    run()
    {
        // constexpr
        {
            constexpr auto r = not_empty_rule(
                pct_encoded_rule( unreserved_chars ));
            (void)r;
        }

        // javadoc
        {
            result< pct_encoded_view > rv = parse( "Program%20Files",
                not_empty_rule( pct_encoded_rule( unreserved_chars ) ) );

            (void)rv;
        }

        ok( pct_encoded_rule(
                grammar::digit_chars),
            "0");
        ok( pct_encoded_rule(
                grammar::digit_chars),
            "");

        ok( not_empty_rule(
                pct_encoded_rule(
                    grammar::digit_chars)),
            "0");
        bad( not_empty_rule(
                pct_encoded_rule(
                    grammar::digit_chars)),
            "");
        bad( not_empty_rule(
                pct_encoded_rule(
                    grammar::digit_chars)),
            "%");
    }
};

TEST_SUITE(
    not_empty_rule_test,
    "boost.url.grammar.not_empty_rule");

} // grammar
} // urls
} // boost

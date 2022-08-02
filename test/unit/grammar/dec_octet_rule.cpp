//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/dec_octet_rule.hpp>

#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct dec_octet_rule_test
{
    void
    testRule()
    {
    }

    void
    run()
    {
        // test constexpr
        constexpr auto r = dec_octet_rule;
        (void)r;

        // javadoc
        {
            result< unsigned char > rv = parse( "255", dec_octet_rule );
        }
        testRule();
    }
};

TEST_SUITE(
    dec_octet_rule_test,
    "boost.url.grammar.dec_octet_rule");

} // grammar
} // urls
} // boost

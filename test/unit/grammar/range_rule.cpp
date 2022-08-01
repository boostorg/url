//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/range_rule.hpp>

#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct range_rule_test
{
    void
    run()
    {
        // test constexpr
        constexpr auto r =
            range_rule(char_rule('.'));
    }
};

TEST_SUITE(
    range_rule_test,
    "boost.url.grammar.range_rule");

} // grammar
} // urls
} // boost

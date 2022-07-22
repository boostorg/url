//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/optional_rule.hpp>

#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct optional_rule_test
{
    void
    run()
    {
        auto rv = grammar::parse(
            "", optional_rule(char_rule('+')));
    }
};

TEST_SUITE(
    optional_rule_test,
    "boost.url.grammar.optional_rule");

} // grammar
} // urls
} // boost

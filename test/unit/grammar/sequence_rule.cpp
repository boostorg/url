//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/sequence_rule.hpp>

#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct sequence_rule_test
{
    template<class R>
    static
    void
    ok( string_view s,
        R const& r)
    {
        BOOST_TEST(
            grammar::parse(
                s, r).has_value());
    }

    template<class R>
    static
    void
    bad(string_view s,
        R const& r)
    {
        BOOST_TEST(
            grammar::parse(
                s, r).has_error());
    }

    void
    testSequence()
    {
        ok("$", sequence_rule(char_rule('$')));
        ok("$!", sequence_rule(char_rule('$'), char_rule('!')));
        bad("$", sequence_rule(char_rule('!')));
    }

    void
    run()
    {
        // test constexpr
        constexpr auto r =
            sequence_rule(
                char_rule('.'),
                char_rule('.'));
        
        testSequence();
    }
};

TEST_SUITE(
    sequence_rule_test,
    "boost.url.grammar.sequence_rule");

} // grammar
} // urls
} // boost

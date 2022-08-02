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
#include <boost/url/grammar/digit_chars.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/unreserved_chars.hpp>

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
    testSquelch()
    {
        result< std::tuple< pct_encoded_view, string_view > > r1 =
            grammar::parse(
                "www.example.com:443",
                grammar::sequence_rule(
                    pct_encoded_rule(unreserved_chars + '-' + '.'),
                    grammar::squelch( grammar::char_rule( ':' ) ),
                    grammar::token_rule( grammar::digit_chars ) ) );

        result< std::tuple<
                pct_encoded_view, string_view, string_view > > r2 =
            grammar::parse(
                "www.example.com:443",
                grammar::sequence_rule(
                    pct_encoded_rule(unreserved_chars + '-' + '.'),
                    grammar::char_rule( ':' ),
                    grammar::token_rule( grammar::digit_chars ) ) );
    }

    void
    run()
    {
        // test constexpr
        constexpr auto r1 =
            sequence_rule(
                char_rule('.'),
                char_rule('.'));
        constexpr auto r2 =
            sequence_rule(
                squelch( char_rule('.') ),
                char_rule('.'));
        (void)r1;
        (void)r2;
        
        testSequence();
        testSquelch();
    }
};

TEST_SUITE(
    sequence_rule_test,
    "boost.url.grammar.sequence_rule");

} // grammar
} // urls
} // boost

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

#include <boost/url/grammar/dec_octet_rule.hpp>
#include <boost/url/grammar/delim_rule.hpp>
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
            parse(s, r).has_value());
    }

    template<class R>
    static
    void
    bad(string_view s,
        R const& r)
    {
        BOOST_TEST(
            parse(s, r).has_error());
    }

    void
    testSequence()
    {
        ok("$", sequence_rule(delim_rule('$')));
        ok("$!", sequence_rule(delim_rule('$'), delim_rule('!')));
        bad("$", sequence_rule(delim_rule('!')));
    }

    void
    testSquelch()
    {
        result< std::tuple< pct_encoded_view, string_view > > r1 =
            parse(
                "www.example.com:443",
                sequence_rule(
                    pct_encoded_rule(unreserved_chars + '-' + '.'),
                    squelch( delim_rule( ':' ) ),
                    token_rule( digit_chars ) ) );

        result< std::tuple<
                pct_encoded_view, string_view, string_view > > r2 =
            parse(
                "www.example.com:443",
                sequence_rule(
                    pct_encoded_rule(unreserved_chars + '-' + '.'),
                    delim_rule( ':' ),
                    token_rule( digit_chars ) ) );
    }

    void
    run()
    {
        // constexpr
        {
            constexpr auto r1 =
                sequence_rule(
                    delim_rule('.'),
                    delim_rule('.'));
            constexpr auto r2 =
                sequence_rule(
                    squelch( delim_rule('.') ),
                    delim_rule('.'));
            (void)r1;
            (void)r2;
        }

        // javadoc
        {
            result< std::tuple< unsigned char, unsigned char, unsigned char, unsigned char > > rv =
                parse( "192.168.0.1", 
                    sequence_rule(
                        dec_octet_rule,
                        squelch( delim_rule('.') ),
                        dec_octet_rule,
                        squelch( delim_rule('.') ),
                        dec_octet_rule,
                        squelch( delim_rule('.') ),
                        dec_octet_rule ) );

            (void)rv;
        }
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

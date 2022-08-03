//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/delim_rule.hpp>

#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct delim_rule_test
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
    run()
    {
        // constexpr
        {
            constexpr auto r = delim_rule('.');

            (void)r;
        }

        // javadoc
        {
            result< string_view > rv = parse( ".", delim_rule('.') );

            (void)rv;
        }

        ok("$", delim_rule('$'));       
        bad("~", delim_rule('$'));
    }
};

TEST_SUITE(
    delim_rule_test,
    "boost.url.grammar.delim_rule");

} // grammar
} // urls
} // boost

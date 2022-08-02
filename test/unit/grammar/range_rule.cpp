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

#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <boost/url/grammar/token_rule.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <initializer_list>

namespace boost {
namespace urls {
namespace grammar {

struct range_rule_test
{
    template<class R>
    static
    void
    check(
        string_view s,
        std::initializer_list<
            string_view> init,
        R const& r)
    {
        auto rv = parse(s, r);
        if(! BOOST_TEST(rv.has_value()))
            return;
        if(! BOOST_TEST_EQ(
                rv->size(), init.size()))
            return;
        BOOST_TEST(
            std::equal(
                rv->begin(),
                rv->end(),
                init.begin()));
    }

    void
    run()
    {
        // test constexpr
        constexpr auto r = range_rule(
            token_rule(alpha_chars),
            sequence_rule(
                squelch(
                    delim_rule('+')),
                token_rule(alpha_chars)));

        // javadoc
        {
            result< range<string_view> > rv = parse( ";alpha;xray;charlie",
                range_rule(
                    sequence_rule(
                        squelch( delim_rule( ';' ) ),
                        token_rule( alpha_chars ) ),
                    1 ) );
        }

        // javadoc
        {
            result< range< string_view > > rv = parse( "whiskey,tango,foxtrot",
                range_rule(
                    token_rule( alpha_chars ),          // first
                    sequence_rule(                      // next
                        squelch( delim_rule(',') ),
                        token_rule( alpha_chars ) ) ) );
        }

        // default construction
        {
            range<string_view> v;
        }

        check("", {}, r);
        check("x", {"x"}, r);
    }
};

TEST_SUITE(
    range_rule_test,
    "boost.url.grammar.range_rule");

} // grammar
} // urls
} // boost

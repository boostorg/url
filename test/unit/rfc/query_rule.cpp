//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/query_rule.hpp>

#include <boost/url/decode_view.hpp>
#include <boost/url/grammar/range_rule.hpp>

#include "test_rule.hpp"

#include <algorithm>

namespace boost {
namespace urls {

bool
operator==(
    param_pct_view const& t0,
    param_pct_view const& t1) noexcept
{
    return
        *t0.key == *t1.key &&
        t0.has_value == t1.has_value &&
        (! t0.has_value ||
            *t0.value == *t1.value);
}

class query_rule_test
{
public:
    void
    check(
        string_view s,
        std::initializer_list<
            param_pct_view> i)
    {
        auto rv = grammar::parse(
            s, query_rule);
        if(! BOOST_TEST(! rv.has_error()))
            return;
        auto const& t = *rv;
        if(! BOOST_TEST_EQ(
            t.size(), i.size()))
            return;
        BOOST_TEST(std::equal(
            i.begin(),
            i.end(),
            t.begin()));
    }

    void
    testParse()
    {
        check("", {{}});
        check("&", {{},{}});
        check("x", {{"x", no_value}});
        check("x&", {{"x", no_value},{}});
        check("x=", {{"x",""}});
        check("x=y", {{"x","y"}});
        check("a=b&c=d", {{"a","b"},{"c","d"}});
    }

    void
    run()
    {
        // javadoc
        {
            result< grammar::range< param_pct_view > > rv = grammar::parse( "format=web&id=42&compact", query_rule );
            (void)rv;
        }

        testParse();

        auto const& t = query_rule;
        
        bad(t, "%");

        ok(t, "");
        ok(t, "x");
        ok(t, "x=");
        ok(t, "x=y");
        ok(t, "x=y&");
        ok(t, "x=y&a");
        ok(t, "x=y&a=b&");
        ok(t, "keys[]=value1&keys[]=value2");

        // some gen-delims
        bad(t, "#");

        // pchar / "/" / "?"
        ok(t,
            // unreserved
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            // sub-delims
            "!$&'()*+,;="
            // ":" / "@"
            ":@"
            // "/" / "?"
            "/?"
            );
    }
};

TEST_SUITE(
    query_rule_test,
    "boost.url.query_rule");

} // urls
} // boost

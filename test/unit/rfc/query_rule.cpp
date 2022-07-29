//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/query_rule.hpp>

#include <boost/url/grammar/range.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <algorithm>

namespace boost {
namespace urls {

bool
operator==(
    query_param_view const& t0,
    query_param_view const& t1) noexcept
{
    // VFALCO This needs to be a pct-encoded equality
    return
        t0.key == t1.key &&
        t0.has_value == t1.has_value &&
        (! t0.has_value ||
            t0.value == t1.value);
}

class query_rule_test
{
public:
    void
    check(
        string_view s,
        std::initializer_list<
            query_param_encoded_view> i)
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
        check("x", {{"x","",false}});
        check("x&", {{"x","",false},{}});
        check("x=", {{"x","",true}});
        check("x=y", {{"x","y",true}});
        check("a=b&c=d", {{"a","b",true},{"c","d",true}});
    }

    void
    run()
    {
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

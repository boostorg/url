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
            query_param_view> i)
    {
        query_rule t;
        if(! BOOST_TEST_NO_THROW(
            grammar::parse_string(s, t)))
            return;
        if(! BOOST_TEST(t.v.size() == i.size()))
            return;
        BOOST_TEST(std::equal(
            i.begin(),
            i.end(),
            t.v.begin()));
    }

    void
    testParse()
    {
        check("", {{}});
        check("&", {{},{}});
        check("x", {{"x"}});
        check("x&", {{"x"},{}});
        check("x=", {{"x","",true}});
        check("x=y", {{"x","y",true}});
        check("a=b&c=d", {{"a","b",true},{"c","d",true}});
    }

    void
    run()
    {
        testParse();

        using T = query_rule;

        bad <T>("%");

        good<T>("");
        good<T>("x");
        good<T>("x=");
        good<T>("x=y");
        good<T>("x=y&");
        good<T>("x=y&a");
        good<T>("x=y&a=b&");
        good<T>("keys[]=value1&keys[]=value2");

        // some gen-delims
        bad<T>("#");

        // pchar / "/" / "?"
        good<T>(
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

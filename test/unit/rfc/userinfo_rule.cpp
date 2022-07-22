//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/userinfo_rule.hpp>

#include <boost/url/grammar/parse.hpp>
#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

struct userinfo_rule_test
{
    void
    check(
        string_view s,
        string_view s1,
        string_view s2,
        bool check_s2 = true)
    {
        auto rv = grammar::parse(
            s, userinfo_rule);
        if(! BOOST_TEST(rv.has_value()))
            return;
        auto t = *rv;
        BOOST_TEST_EQ(
            t.user.encoded(), s1);
        if(check_s2)
        {
            BOOST_TEST(
                t.has_password &&
                t.password.encoded() == s2);
        }
        else
        {
            BOOST_TEST(! t.has_password);
        }
    }

    void
    run()
    {
        auto const& t =
            userinfo_rule;

        bad(t, "@");

        ok(t, "");
        ok(t, "x");
        ok(t, "xy");
        ok(t, "x:");
        ok(t, "x:y");
        ok(t, "x:y:");
        ok(t, "x:y:z");
        ok(t, "%41");

        check("x",      "x",  "", false);
        check("x:",     "x",  "");
        check(":",      "",   "");
        check("::",     "",   ":");
        check(":x",     "",   "x");
        check("x:y",    "x",  "y");
        check("xy:zz:", "xy", "zz:");
        check(
            "%41%42:%43%44", "%41%42", "%43%44");
    }
};

TEST_SUITE(
    userinfo_rule_test,
    "boost.url.userinfo_rule");

} // urls
} // boost

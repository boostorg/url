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

class userinfo_rule_test
{
public:
    void
    check(
        string_view s,
        string_view s1,
        string_view s2,
        bool check_s2 = true)
    {
        userinfo_rule t;
        error_code ec;
        if(! BOOST_TEST(
            grammar::parse_string(s, ec, t)))
            return;
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(t.user.str == s1);
        if(check_s2)
        {
            BOOST_TEST(
                t.has_password &&
                t.password.str == s2);
        }
        else
        {
            BOOST_TEST(! t.has_password);
        }
    }

    void
    run()
    {
        using T = userinfo_rule;

        bad<T> ("@");

        good<T>("");
        good<T>("x");
        good<T>("xy");
        good<T>("x:");
        good<T>("x:y");
        good<T>("x:y:");
        good<T>("x:y:z");
        good<T>("%41");

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

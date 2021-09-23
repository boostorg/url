//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/userinfo_bnf.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class userinfo_bnf_test
{
public:
    void
    check(
        string_view s,
        string_view s1,
        optional<
            string_view> s2)
    {
        using urls::detail::throw_system_error;
        userinfo_bnf t;
        error_code ec;
        if(! BOOST_TEST(
            bnf::parse_string(s, ec, t)))
            return;
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(t.user.str == s1);
        if(s2.has_value())
        {
            BOOST_TEST(
                t.has_password &&
                t.password.str == *s2);
        }
        else
        {
            BOOST_TEST(! t.has_password);
        }
    }

    void
    run()
    {
        using T = userinfo_bnf;

        bad<T> ("@");

        good<T>("");
        good<T>("x");
        good<T>("xy");
        good<T>("x:");
        good<T>("x:y");
        good<T>("x:y:");
        good<T>("x:y:z");
        good<T>("%41");

        using U =
            optional<string_view>;
        check("x",      "x",  boost::none);
        check("x:",     "x",  U(""));
        check(":",      "",   U(""));
        check("::",     "",   U(":"));
        check(":x",     "",   U("x"));
        check("x:y",    "x",  U("y"));
        check("xy:zz:", "xy", U("zz:"));
        check(
            "%41%42:%43%44", "%41%42", U("%43%44"));
    }
};

TEST_SUITE(
    userinfo_bnf_test,
    "boost.url.userinfo_bnf");

} // urls
} // boost

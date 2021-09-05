//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/userinfo.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class userinfo_test
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
        userinfo p;
        error_code ec;
        using bnf::parse;
        if(! BOOST_TEST(
            parse(s, ec, p)))
            return;
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(p.str == s);
        BOOST_TEST(p.user.str == s1);
        if(s2.has_value())
            BOOST_TEST(
                p.pass.has_value() &&
                p.pass->str == *s2);
        else
            BOOST_TEST(! p.pass.has_value());
    }

    void
    run()
    {
        bad<userinfo>({
            "@"
            });

        good<userinfo>({
            "",
            "x",
            "xy",
            "x:",
            "x:y",
            "x:y:",
            "x:y:z",
            "%41"
            });

        using T =
            optional<string_view>;
        check("x",      "x",  boost::none);
        check("x:",     "x",  T(""));
        check(":",      "",   T(""));
        check("::",     "",   T(":"));
        check(":x",     "",   T("x"));
        check("x:y",    "x",  T("y"));
        check("xy:zz:", "xy", T("zz:"));
        check(
            "%41%42:%43%44", "%41%42", T("%43%44"));
    }
};

TEST_SUITE(
    userinfo_test,
    "boost.url.userinfo");

} // rfc
} // urls
} // boost

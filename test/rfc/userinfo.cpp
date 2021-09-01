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

#include <boost/url/detail/except.hpp>
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
        string_view s2,
        string_view s3)
    {
        userinfo p;
        error_code ec;
        auto const end =
            s.data() + s.size();
        auto it = p.parse(
            s.data(), end, ec);
        if(! ec && it != end)
            ec = error::syntax;
        if(ec)
            detail::throw_system_error(ec,
                BOOST_CURRENT_LOCATION);
        BOOST_TEST(p->encoded_userinfo() == s1);
        BOOST_TEST(p->user() == s2);
        BOOST_TEST(p->password() == s3);
    }

    void
    run()
    {
        bad <userinfo>("@");

        good<userinfo>("");
        good<userinfo>("x");
        good<userinfo>("xy");
        good<userinfo>("x:");
        good<userinfo>("x:y");
        good<userinfo>("x:y:");
        good<userinfo>("x:y:z");
        good<userinfo>("%41");

        check("x", "x", "x", "");
        check("x:", "x:", "x", "");
        check(":", ":", "", "");
        check("::", "::", "", ":");
        check(":x", ":x", "", "x");
        check("x:y", "x:y", "x", "y");
        check("xy:zz:", "xy:zz:", "xy", "zz:");
        check(
            "%41%42:%43%44",
            "%41%42:%43%44",
            "AB", "CD");
    }
};

TEST_SUITE(
    userinfo_test,
    "boost.url.userinfo");

} // rfc
} // urls
} // boost

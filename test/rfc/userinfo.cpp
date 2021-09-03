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
        optional<
            string_view> s3)
    {
        using urls::detail::throw_system_error;
        userinfo p;
        error_code ec;
        auto const end =
            s.data() + s.size();
        auto it = parse(
            s.data(), end, ec, p);
        if(! ec && it != end)
            ec = error::syntax;
        if(ec)
            throw_system_error(ec,
                BOOST_CURRENT_LOCATION);
        BOOST_TEST(p.str() == s1);
        BOOST_TEST(p.user().str() == s2);
        if(s3.has_value())
            BOOST_TEST(
                p.password().has_value() &&
                p.password()->str() == *s3);
        else
            BOOST_TEST(! p.password().has_value());
    }

    void
    run()
    {
        bad_ <userinfo>("@");

        good_<userinfo>("");
        good_<userinfo>("x");
        good_<userinfo>("xy");
        good_<userinfo>("x:");
        good_<userinfo>("x:y");
        good_<userinfo>("x:y:");
        good_<userinfo>("x:y:z");
        good_<userinfo>("%41");

        using T = optional<string_view>;
        check("x",      "x",      "x",  boost::none);
        check("x:",     "x:",     "x",  T(""));
        check(":",      ":",      "",   T(""));
        check("::",     "::",     "",   T(":"));
        check(":x",     ":x",     "",   T("x"));
        check("x:y",    "x:y",    "x",  T("y"));
        check("xy:zz:", "xy:zz:", "xy", T("zz:"));
        check(
            "%41%42:%43%44",
            "%41%42:%43%44",
            "%41%42", T("%43%44"));
    }
};

TEST_SUITE(
    userinfo_test,
    "boost.url.userinfo");

} // rfc
} // urls
} // boost

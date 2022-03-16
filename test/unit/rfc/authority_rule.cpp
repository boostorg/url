//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/authority_rule.hpp>

#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <type_traits>

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        authority_rule>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_assignable<
        authority_rule>::value);

class authority_rule_test
{
public:
    void
    run()
    {
        auto const bad = [](string_view s)
        {
            error_code ec;
            authority_rule t;
            grammar::parse_string(s, ec, t);
            BOOST_TEST(ec.failed());
        };

        auto const good = [](string_view s)
        {
            error_code ec;
            authority_rule t;
            grammar::parse_string(s, ec, t);
            BOOST_TEST(! ec.failed());
        };

        bad("%");
        bad("host:a");

        good("");
        good(":");
        good("me@you.com");
        good("user:pass@");
        good("user:1234");

        {
            authority_rule p;
            error_code ec;
            BOOST_TEST(grammar::parse_string(
                "x:y@e.com:8080", ec, p));
            BOOST_TEST(p.host.host_type ==
                host_type::name);
            BOOST_TEST(p.host.name.str
                == "e.com");
            if(BOOST_TEST(p.port.has_port))
            {
                BOOST_TEST_EQ(p.port.port, "8080");
                BOOST_TEST(p.port.has_number);
                BOOST_TEST_EQ(p.port.port_number, 8080);
            }
            if(BOOST_TEST(p.has_userinfo))
            {
                BOOST_TEST_EQ(p.userinfo.user.str, "x");
                if(BOOST_TEST(p.userinfo.has_password))
                    BOOST_TEST_EQ(p.userinfo.password.str, "y");
            }
        }
    }
};

TEST_SUITE(
    authority_rule_test,
    "boost.url.authority_rule");

} // urls
} // boost

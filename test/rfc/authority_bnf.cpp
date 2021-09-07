//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/authority_bnf.hpp>

#include <boost/url/bnf/parse.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <type_traits>

namespace boost {
namespace urls {
namespace rfc {

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        authority_bnf>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_assignable<
        authority_bnf>::value);

class authority_bnf_test
{
public:
    void
    run()
    {
        using T = authority_bnf;

        bad<T> ("%");

        good<T>("");
        good<T>(":");
        good<T>("me@you.com");
        good<T>("user:pass@");
        good<T>("user:1234");

        {
            authority_bnf p;
            error_code ec;
            using bnf::parse;
            BOOST_TEST(parse(
                "x:y@e.com:8080", ec, p));
            BOOST_TEST(p.str ==
                "x:y@e.com:8080");
            BOOST_TEST(p.host.host_type() ==
                host_type::name);
            BOOST_TEST(p.host.get_name().str
                == "e.com");
            if(BOOST_TEST(p.port.has_value()))
            {
                BOOST_TEST(p.port->str == "8080");
                BOOST_TEST(p.port->number.has_value());
                BOOST_TEST(*p.port->number == 8080);
            }
            if(BOOST_TEST(p.userinfo.has_value()))
            {
                BOOST_TEST(p.userinfo->str == "x:y");
                BOOST_TEST(p.userinfo->username.str == "x");
                if(BOOST_TEST(p.userinfo->password.has_value()))
                    BOOST_TEST(p.userinfo->password->str == "y");
            }
        }
    }
};

TEST_SUITE(
    authority_bnf_test,
    "boost.url.authority_bnf");

} // rfc
} // urls
} // boost

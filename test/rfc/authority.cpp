//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/authority.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class authority_test
{
public:
    void
    run()
    {
        bad<authority>({
            "%"
            });
        good<authority>({
            "",
            ":",
            "me@you.com",
            "user:pass@",
            "user:1234"
            });

        {
            authority p;
            error_code ec;
            using bnf::parse;
            BOOST_TEST(parse(
                "x:y@e.com:8080", ec, p));
            BOOST_TEST(p.str ==
                "x:y@e.com:8080");
            BOOST_TEST(p.host.kind() ==
                host_kind::domain);
            BOOST_TEST(p.host.get_domain().str
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
                BOOST_TEST(p.userinfo->user.str == "x");
                if(BOOST_TEST(p.userinfo->pass.has_value()))
                    BOOST_TEST(p.userinfo->pass->str == "y");
            }
        }
    }
};

TEST_SUITE(
    authority_test,
    "boost.url.authority");

} // rfc
} // urls
} // boost

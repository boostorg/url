//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/host.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class host_test
{
public:
    static
    host
    check(
        string_view s,
        rfc::host_kind k)
    {
        host h;
        error_code ec;
        using bnf::parse;
        if(! BOOST_TEST(
            parse(s, ec, h)))
            return {};
        BOOST_TEST(
            h.kind() == k);
        return h;
    }

    void
    run()
    {
        bad<host>({
            "%"
            });

        good<host>({
            "",
            "[::]",
            "[::1.2.3.4]",
            "[v1.0]",
            "1.2.3.4",
            "boost.org",
            "999.0.0.1"
            });

        BOOST_TEST(check("", host_kind::domain)
            .str() == "");

        BOOST_TEST(check("1.2.3.999", host_kind::domain)
            .get_domain().str == "1.2.3.999");

        BOOST_TEST(check("1.2.3.4", host_kind::ipv4)
            .get_ipv4().octets == (
                std::array<std::uint8_t, 4>({1,2,3,4})));

        BOOST_TEST(check("[1:2:3:4:5:6:7:8]", host_kind::ipv6)
            .get_ipv6().octets == (
                std::array<std::uint8_t, 16>(
                {0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8})));

        BOOST_TEST(check("[v1.2]", host_kind::ipv_future)
            .get_ipv_future() == "v1.2");

        BOOST_TEST(check("www.example.com", host_kind::domain)
            .get_domain().str == "www.example.com");
    }
};

TEST_SUITE(
    host_test,
    "boost.url.host");

} // rfc
} // urls
} // boost

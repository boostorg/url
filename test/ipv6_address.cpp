//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/ipv6_address.hpp>

#include <boost/url/ipv4_address.hpp>
#include <boost/url/detail/network_order.hpp>
#include "test_suite.hpp"
#include <sstream>

namespace boost {
namespace urls {

class ipv6_address_test
{
public:
    void
    bad(string_view s)
    {
        error_code ec;
        make_ipv6_address(s, ec);
        BOOST_TEST(ec.failed());
    }

    void
    check(string_view s)
    {
        BOOST_TEST(make_ipv6_address(
            s).to_string() == s);
    }

    void
    check(string_view s0, string_view s1)
    {
        auto ip0 = make_ipv6_address(s0);
        BOOST_TEST(ip0.to_string() == s1);
        // round-trip
        auto ip1 = make_ipv6_address(
            ip0.to_string());
        BOOST_TEST(ip1 == ip0);
    }

    void
    testSpecial()
    {
        BOOST_TEST(
            ipv6_address().to_string() == "::");
        ipv6_address::bytes_type bytes = {
            1, 0, 2, 0, 3, 0, 4, 0,
            5, 0, 6, 0, 7, 0, 8, 0 };
        ipv6_address a0(bytes);
        BOOST_TEST(a0.to_string() ==
            "10:20:30:40:50:60:70:80");
        ipv6_address a1(a0);
        BOOST_TEST(a1 == a0);
        ipv6_address a2;
        BOOST_TEST(a2 != a1);
        a2 = a1;
        BOOST_TEST(a2 == a1);
        BOOST_TEST(a2.to_bytes() == bytes);
        ipv6_address::bytes_type bytes1{};
        BOOST_TEST(a2.to_bytes() != bytes1);
    }

    void
    testKind()
    {
        // unspecified
        BOOST_TEST(ipv6_address().is_unspecified());

        // loopback
        BOOST_TEST(
            make_ipv6_address("::1").is_loopback());
        BOOST_TEST(
            make_ipv6_address("::1") ==
                ipv6_address::loopback());
        BOOST_TEST(
            make_ipv6_address("0:0:0:0:0:0:0:1").is_loopback());
        BOOST_TEST(
            ! make_ipv6_address("0:0:0:0:0:0:0:2").is_loopback());
    }

    void
    testParse()
    {
        bad(":");
        bad(":::");
        bad("0:");
        bad(":0");

        check("1::");
        check("12::");
        check("123::");
        check("1234::");
        check("abcd::");
        check("ABCD::", "abcd::");
        check("0:0:0:0:0:0:0:0", "::");
        check("1:0:0:0:0:0:0:0", "1::");
        check("0:1:0:0:0:0:0:0", "0:1::");
        check("0:0:1:0:0:0:0:0", "0:0:1::");
        check("0:0:0:1:0:0:0:0", "0:0:0:1::");
        check("0:0:0:0:1:0:0:0", "::1:0:0:0");
        check("0:0:0:0:0:1:0:0", "::1:0:0");
        check("0:0:0:0:0:0:1:0", "::1:0");
        check("0:0:0:0:0:0:0:1", "::1");
        check("1234:1234:1234:1234:1234:1234:1234:1234");
        check("1234:1234:1234:1234:1234:1234:255.255.255.255",
              "1234:1234:1234:1234:1234:1234:ffff:ffff");
        check("::ffff:1.2.3.4");
        check("0:0:0:0:0:ffff:1.2.3.4", "::ffff:1.2.3.4");

        // coverage
        bad("::ffff:260.168.0.1");
        bad("::ffff:999.168.0.1");
        bad("::ffff:1a0.168.0.1");
        bad("::ffff:a.168.0.1");
        bad("::ffff:0a.168.0.1");
        bad("::ffff:0000a.168.0.1");
        bad("::ffff:192.168.0.");
        bad("0:0:0:0:0:0:0:");
        bad("0");
        bad("0:");
        bad("0:0:0:0:ffff");
    }

    void
    testIpv4()
    {
        // ipv4_mapped
        BOOST_TEST(make_ipv6_address(
            "::ffff:1.2.3.4").is_v4_mapped());
        BOOST_TEST(! make_ipv6_address(
            "1::ffff:1.2.3.4").is_v4_mapped());
        BOOST_TEST(make_ipv6_address("::ffff:c0a8:0001"
            ).to_string() == "::ffff:192.168.0.1");
        BOOST_TEST(make_ipv6_address("::ffff:192.168.0.1"
            ).to_string() == "::ffff:192.168.0.1");
        BOOST_TEST(make_ipv6_address("1::ffff:c0a8:0001"
            ).to_string() != "::ffff:192.168.0.1");
        BOOST_TEST(make_ipv6_address("1::ffff:192.168.0.1"
            ).to_string() != "::ffff:192.168.0.1");

        ipv4_address a(0x7f000001);
        BOOST_TEST(make_ipv6_address(a).to_string() ==
            "::ffff:127.0.0.1");
    }

    void
    testOutput()
    {
        std::stringstream ss;
        ss << make_ipv6_address("1:0:0:0:0:0:0:1");
        BOOST_TEST(ss.str() == "1::1");
    }

    void
    run()
    {
        testSpecial();
        testKind();
        testParse();
        testIpv4();
        testOutput();
    }
};

TEST_SUITE(
    ipv6_address_test,
    "boost.url.ipv6_address");

} // urls
} // boost

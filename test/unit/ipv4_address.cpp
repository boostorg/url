//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/ipv4_address.hpp>

#include <boost/url/detail/test/test_suite.hpp>
#include <sstream>

namespace boost {
namespace urls {

class ipv4_address_test
{
public:
    void
    testOutput()
    {
        std::stringstream ss;
        ipv4_address a(0x01020304);
        ss << a;
        BOOST_TEST(
            ss.str() == "1.2.3.4");

        char buf[ipv4_address::max_str_len];
        BOOST_TEST(a.to_buffer(buf,
            sizeof(buf)) == "1.2.3.4");
    }

    void
    run()
    {
        testOutput();
        {
            ipv4_address a;
            BOOST_TEST(a.is_unspecified());
            BOOST_TEST(a ==
                ipv4_address::any());
        }
        {
            ipv4_address a(0xc0a80001);
            BOOST_TEST(! a.is_loopback());
            BOOST_TEST(! a.is_unspecified());
            BOOST_TEST(! a.is_multicast());
            auto v = a.to_bytes();
            BOOST_TEST(v[0] == 0xc0);
            BOOST_TEST(v[1] == 0xa8);
            BOOST_TEST(v[2] == 0x00);
            BOOST_TEST(v[3] == 0x01);
            BOOST_TEST(a.to_uint() ==
                0xc0a80001);
            BOOST_TEST(a.to_string() ==
                "192.168.0.1");
        }
        {
            ipv4_address a1 =
                ipv4_address::loopback();
            BOOST_TEST(a1.is_loopback());
            ipv4_address a2;
            BOOST_TEST(a1 != a2);
            BOOST_TEST(a2.is_unspecified());
            a2 = a1;
            BOOST_TEST(a2.is_loopback());
            BOOST_TEST(a1 == a2);
        }
        {
            BOOST_TEST(make_ipv4_address(
                "1.2.3.4") == ipv4_address(0x01020304));
            BOOST_TEST_THROWS(
                make_ipv4_address("256.0.0.1"),
                std::exception);
        }
    }
};

TEST_SUITE(
    ipv4_address_test,
    "boost.url.ipv4_address");

} // urls
} // boost

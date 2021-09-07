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

#include <boost/url/detail/network_order.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class ipv6_address_test
{
public:
    void
    check(string_view s)
    {
        BOOST_TEST(make_ipv6_address(
            s).to_string() == s);
    }

    void
    check(string_view s0, string_view s1)
    {
        BOOST_TEST(make_ipv6_address(
            s0).to_string() == s1);
    }

    void
    run()
    {
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
    }
};

TEST_SUITE(
    ipv6_address_test,
    "boost.url.ipv6_address");

} // urls
} // boost

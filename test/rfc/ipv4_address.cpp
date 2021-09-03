//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv4_address.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class ip_v4address_test
{
public:
    void
    check(string_view s, std::uint32_t v)
    {
        error_code ec;
        ipv4_address t;
        auto const end =
            s.data() + s.size();
        auto it = parse(
            s.data(), end, ec, t);
        if(! BOOST_TEST(! ec))
            return;
        if(! BOOST_TEST(it == end))
            return;
        std::uint32_t v1 =
            (static_cast<std::uint32_t>(
                t.octets[0]) << 24) |
            (static_cast<std::uint32_t>(
                t.octets[1]) << 16) |
            (static_cast<std::uint32_t>(
                t.octets[2]) <<  8) |
            (static_cast<std::uint32_t>(
                t.octets[3])      );
        BOOST_TEST(v1 == v);
    }

    void
    run()
    {
        bad_ <ipv4_address>("0");
        bad_ <ipv4_address>("0.");
        bad_ <ipv4_address>("0.0");
        bad_ <ipv4_address>("0.0.");
        bad_ <ipv4_address>("0.0.0");
        bad_ <ipv4_address>("0.0.0.");
        bad_ <ipv4_address>("0.0.0.256");
        bad_ <ipv4_address>("1.2.3.4.");
        bad_ <ipv4_address>("1.2.3.4x");
        bad_ <ipv4_address>("1.2.3.300");

        good_<ipv4_address>("0.0.0.0");
        good_<ipv4_address>("1.2.3.4");

        check("0.0.0.0", 0x00000000);
        check("1.2.3.4", 0x01020304);
        check("32.64.128.1", 0x20408001);
        check("255.255.255.255", 0xffffffff);
    }
};

TEST_SUITE(
    ip_v4address_test,
    "boost.url.ipv4_address");

} // rfc
} // urls
} // boost

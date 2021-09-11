//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv4_address_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class ip_v4address_bnf_test
{
public:
    void
    check(string_view s, std::uint32_t v)
    {
        error_code ec;
        ipv4_address_bnf t;
        using bnf::parse_string;
        BOOST_TEST(
            parse_string(s, ec, t));
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(
            t.addr.to_uint() == v);
    }

    void
    run()
    {
        using T = ipv4_address_bnf;

        bad<T>("0");
        bad<T>("0.");
        bad<T>("0.0");
        bad<T>("0.0.");
        bad<T>("0.0.0");
        bad<T>("0.0.0.");
        bad<T>("0.0.0.256");
        bad<T>("00.0.0.0");
        bad<T>("1.2.3.4.");
        bad<T>("1.2.3.4x");
        bad<T>("1.2.3.300");
        good<T>("0.0.0.0");
        good<T>("1.2.3.4");
        good<T>("1.2.3.42");

        check("0.0.0.0", 0x00000000);
        check("1.2.3.4", 0x01020304);
        check("32.64.128.1", 0x20408001);
        check("255.255.255.255", 0xffffffff);
    }
};

TEST_SUITE(
    ip_v4address_bnf_test,
    "boost.url.ipv4_address_bnf");

} // urls
} // boost

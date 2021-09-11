//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/port_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class port_bnf_test
{
public:
    void
    check(
        string_view s,
        optional<std::uint16_t> v)
    {
        port_bnf t;
        error_code ec;
        using bnf::parse_string;
        if(! BOOST_TEST(
            parse_string(
                s, ec, t)))
            return;
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(t.str == s);
        BOOST_TEST(t.number == v);
    }

    void
    run()
    {
        using T = port_bnf;

        bad<T>("x");
        bad<T>("80x");
        bad<T>(":443");

        check("", boost::none);
        check("0", 0);
        check("00", 0);
        check("01", 1);
        check("1", 1);
        check("65535", 65535);
        check("65536", boost::none);
        check("123456789", boost::none);
    }
};

TEST_SUITE(
    port_bnf_test,
    "boost.url.port_bnf");

} // urls
} // boost

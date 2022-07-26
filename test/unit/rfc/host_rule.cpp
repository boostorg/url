//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/host_rule.hpp>

#include <boost/url/grammar/parse.hpp>
#include <boost/static_assert.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <type_traits>

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        host_rule>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_assignable<
        host_rule>::value);

class host_rule_test
{
public:
    static
    host_rule
    check(
        string_view s,
        host_type ht)
    {
        host_rule h;
        error_code ec;
        if(! BOOST_TEST(
            grammar::parse_string(s, ec, h)))
            return {};
        BOOST_TEST(
            h.host_type == ht);
        return h;
    }

    void
    run()
    {
        using T = host_rule;

        bad<T>("%");

        good<T>("");
        good<T>("[::]");
        good<T>("[::1.2.3.4]");
        good<T>("[v1.0]");
        good<T>("1.2.3.4");
        good<T>("boost.org");
        good<T>("999.0.0.1");

        BOOST_TEST(check("", host_type::name)
            .host_part == "");

        BOOST_TEST(check("www.example.com", host_type::name)
            .name.encoded() == "www.example.com");

        BOOST_TEST(check("1.2.3.999", host_type::name)
            .name.encoded() == "1.2.3.999");

        BOOST_TEST(check("1.2.3.4", host_type::ipv4)
            .ipv4.to_bytes() == (
                std::array<std::uint8_t, 4>({{1,2,3,4}})));

        BOOST_TEST(check(
            "[1:2:3:4:5:6:7:8]", host_type::ipv6)
                .ipv6 == ipv6_address(
                    "1:2:3:4:5:6:7:8"));

        BOOST_TEST(check("[v1.2]", host_type::ipvfuture)
            .ipvfuture == "v1.2");
    }
};

TEST_SUITE(
    host_rule_test,
    "boost.url.host_rule");

} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/port.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class port_test
{
public:
    void
    check(
        string_view s,
        string_view str,
        optional<std::uint16_t> v)
    {
        error_code ec;
        port t;
        auto const end =
            s.data() + s.size();
        auto it = parse(
            s.data(), end, ec, t);
        if(! BOOST_TEST(! ec))
            return;
        if(! BOOST_TEST(it == end))
            return;
        BOOST_TEST(t.str() == str);
        BOOST_TEST(t.as_number() == v);
    }

    void
    run()
    {
        using T = optional<
            port::number_type>;

        bad <port>("x");

        // VFALCO TODO
        check("0", "0", boost::none);//T(0));
    }
};

TEST_SUITE(
    port_test,
    "boost.url.port");

} // rfc
} // urls
} // boost

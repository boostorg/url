//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/absolute_uri_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <iostream>

namespace boost {
namespace urls {

class absolute_uri_rule_test
{
public:
    void
    run()
    {
        auto const bad = [](string_view s)
        {
            error_code ec;
            absolute_uri_rule t;
            grammar::parse_string(s, ec, t);
            BOOST_TEST(ec.failed());
        };

        auto const good = [](string_view s)
        {
            error_code ec;
            absolute_uri_rule t;
            grammar::parse_string(s, ec, t);
            BOOST_TEST(! ec.failed());
        };

        bad("");
        bad(":");
        bad("http://#");
        bad("http://x.y.z/?a=b&c=d&#");
        bad("http://x.y.z/?a=b&c=d&#frag");
        bad("http://x.y.z/#frag");
        bad("http://%");
        bad("http://?%");

        good("http:");
        good("http:x");
        good("http:x/");
        good("http:x/x");
        good("http:x//");
        good("http://");
        good("http://x");
        good("http://x.y.z");
        good("http://x.y.z/");
        good("http://x.y.z/?");
        good("http://x.y.z/?a");
        good("http://x.y.z/?a=");
        good("http://x.y.z/?a=b");
        good("http://x.y.z/?a=b&c=d");
        good("http://x.y.z/?a=b&c=d&");
    }
};

TEST_SUITE(
    absolute_uri_rule_test,
    "boost.url.absolute_uri_rule");

} // urls
} // boost

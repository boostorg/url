//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/uri_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <iostream>

namespace boost {
namespace urls {

class uri_rule_test
{
public:
    void
    run()
    {
        using T = uri_rule;

        bad<T>("");
        bad<T>(":");
        bad<T>("http://##");

        good<T>("http:");
        good<T>("http:x");
        good<T>("http:x/");
        good<T>("http:x/x");
        good<T>("http:x//");
        good<T>("http://");
        good<T>("http://x");
        good<T>("http://x.y.z");
        good<T>("http://x.y.z/");
        good<T>("http://x.y.z/?");
        good<T>("http://x.y.z/?a");
        good<T>("http://x.y.z/?a=");
        good<T>("http://x.y.z/?a=b");
        good<T>("http://x.y.z/?a=b&c=d");
        good<T>("http://x.y.z/?a=b&c=d&");
        good<T>("http://x.y.z/?a=b&c=d&#");
        good<T>("http://x.y.z/?a=b&c=d&#1");
        good<T>("http://x.y.z/?a=b&c=d&#12");
        good<T>("http://x.y.z/?a=b&c=d&#12%23");
        good<T>("http://x.y.z/?a=b&c=d&#12%23%20");
    }
};

TEST_SUITE(
    uri_rule_test,
    "boost.url.uri_rule");

} // urls
} // boost

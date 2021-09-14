//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/absolute_uri_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {

class absolute_uri_bnf_test
{
public:
    void
    run()
    {
        using T = absolute_uri_bnf;

        bad<T>("");
        bad<T>(":");
        bad<T>("http://#");
        bad<T>("http://x.y.z/?a=b&c=d&#");
        bad<T>("http://x.y.z/?a=b&c=d&#frag");
        bad<T>("http://x.y.z/#frag");

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
    }
};

TEST_SUITE(
    absolute_uri_bnf_test,
    "boost.url.absolute_uri_bnf");

} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/uri_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class uri_bnf_test
{
public:
    void
    run()
    {
        using T = uri_bnf;

        bad <T>("");
        bad <T>(":");

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
    uri_bnf_test,
    "boost.url.uri_bnf");

} // rfc
} // urls
} // boost

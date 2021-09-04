//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/scheme.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class scheme_test
{
public:
    void
    run()
    {
        bad_ <scheme>("");
        bad_ <scheme>("1");
        bad_ <scheme>(" ");
        bad_ <scheme>(" http");
        bad_ <scheme>("http ");
        good_<scheme>("http");
        good_<scheme>("a1steak");
    }
};

TEST_SUITE(
    scheme_test,
    "boost.url.scheme");

} // rfc
} // urls
} // boost

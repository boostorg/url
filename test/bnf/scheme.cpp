//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/scheme.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace bnf {

class scheme_test
{
public:
    void
    run()
    {
        test::bad<scheme>("");
        test::bad<scheme>("1");
        test::bad<scheme>(" ");
        test::bad<scheme>(" http");
        test::bad<scheme>("http ");
        test::good<scheme>("http");
        test::good<scheme>("a1steak");
    }
};

TEST_SUITE(
    scheme_test,
    "boost.url.scheme");

} // bnf
} // urls
} // boost

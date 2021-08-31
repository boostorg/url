//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/path_absolute.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace bnf {

class path_absolute_test
{
public:
    void
    run()
    {
        test::bad <path_absolute>("");
        test::bad <path_absolute>("//");
        test::bad <path_absolute>(":");
        test::bad <path_absolute>(":/");

        test::good<path_absolute>("/");
        test::good<path_absolute>("/x");
        test::good<path_absolute>("/x//");
        test::good<path_absolute>("/x/y");
        test::good<path_absolute>("/x/y//");
    }
};

TEST_SUITE(
    path_absolute_test,
    "boost.url.path_absolute");

} // bnf
} // urls
} // boost

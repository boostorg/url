//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/path_abempty.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace bnf {

class path_abempty_test
{
public:
    void
    run()
    {
        test::bad<path_abempty>("-");
        test::bad<path_abempty>(" ");
        test::bad<path_abempty>(" /");

        test::good<path_abempty>("");
        test::good<path_abempty>("/");
        test::good<path_abempty>("//");
        test::good<path_abempty>("/x");
        test::good<path_abempty>("/x/");
        test::good<path_abempty>("/x/y/z");
        test::good<path_abempty>("/x/y/z/");
    }
};

TEST_SUITE(
    path_abempty_test,
    "boost.url.path_abempty");

} // bnf
} // urls
} // boost

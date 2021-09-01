//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/path_abempty.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class path_abempty_test
{
public:
    void
    run()
    {
        bad<path_abempty>("-");
        bad<path_abempty>(" ");
        bad<path_abempty>(" /");

        good<path_abempty>("");
        good<path_abempty>("/");
        good<path_abempty>("//");
        good<path_abempty>("/x");
        good<path_abempty>("/x/");
        good<path_abempty>("/x/y/z");
        good<path_abempty>("/x/y/z/");
    }
};

TEST_SUITE(
    path_abempty_test,
    "boost.url.path_abempty");

} // rfc
} // urls
} // boost

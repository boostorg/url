//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/path_rootless.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class path_rootless_test
{
public:
    void
    run()
    {
        bad <path_rootless>("");
        bad <path_rootless>("/");
        bad <path_rootless>("/x");

        good<path_rootless>("x");
        good<path_rootless>("x/");
        good<path_rootless>("x/y");
        good<path_rootless>("x//");
        good<path_rootless>("x/y/");
        good<path_rootless>(":");
    }
};

TEST_SUITE(
    path_rootless_test,
    "boost.url.path_rootless");

} // rfc
} // urls
} // boost

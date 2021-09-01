//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/path_noscheme.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class path_noscheme_test
{
public:
    void
    run()
    {
        bad <path_noscheme>("");
        bad <path_noscheme>(":");
        bad <path_noscheme>(":/");
        bad <path_noscheme>("/:");
        bad <path_noscheme>("/");

        good<path_noscheme>("x");
        good<path_noscheme>("x/");
        good<path_noscheme>("x/:");
        good<path_noscheme>("x//:");
        good<path_noscheme>("x//x/y");
        good<path_noscheme>("x/y/z");
        good<path_noscheme>("x/y/z/");
    }
};

TEST_SUITE(
    path_noscheme_test,
    "boost.url.path_noscheme");

} // rfc
} // urls
} // boost

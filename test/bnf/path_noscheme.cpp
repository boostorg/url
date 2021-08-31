//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/path_noscheme.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace bnf {

class path_noscheme_test
{
public:
    void
    run()
    {
        test::bad <path_noscheme>("");
        test::bad <path_noscheme>(":");
        test::bad <path_noscheme>(":/");
        test::bad <path_noscheme>("/:");
        test::bad <path_noscheme>("/");

        test::good<path_noscheme>("x");
        test::good<path_noscheme>("x/");
        test::good<path_noscheme>("x/:");
        test::good<path_noscheme>("x//:");
        test::good<path_noscheme>("x//x/y");
        test::good<path_noscheme>("x/y/z");
        test::good<path_noscheme>("x/y/z/");
    }
};

TEST_SUITE(
    path_noscheme_test,
    "boost.url.path_noscheme");

} // bnf
} // urls
} // boost

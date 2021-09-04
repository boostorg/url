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
    struct T
    {
        friend
        char const*
        parse(
            char const* start,
            char const* end,
            error_code& ec,
            T const&)
        {
            string_view s;
            return parse(
                start, end, ec,
                    scheme{s});
        }
    };

    void
    run()
    {
        bad <T>("");
        bad <T>("1");
        bad <T>(" ");
        bad <T>(" http");
        bad <T>("http ");
        good<T>("http");
        good<T>("a1steak");
    }
};

TEST_SUITE(
    scheme_test,
    "boost.url.scheme");

} // rfc
} // urls
} // boost

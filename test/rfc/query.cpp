//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/query.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class query_test
{
public:
    struct T
    {
        friend
        bool
        parse(
            char const*& it,
            char const* end,
            error_code& ec,
            T const&)
        {
            query::value_type v;
            return parse(
                it, end, ec,
                    query{v});
        }
    };

    void
    run()
    {
        bad <T>("%");

        good<T>("");
        good<T>("x");
        good<T>("x=");
        good<T>("x=y");
        good<T>("x=y&");
        good<T>("x=y&a");
        good<T>("x=y&a=b&");
    }
};

TEST_SUITE(
    query_test,
    "boost.url.query");

} // rfc
} // urls
} // boost

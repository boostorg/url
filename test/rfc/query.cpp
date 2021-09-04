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
    void
    run()
    {
        using T = query;

        bad <T>("%");

        good<T>("");
        good<T>("x");
        good<T>("x=");
        good<T>("x=y");
        good<T>("x=y&");
        good<T>("x=y&a");
        good<T>("x=y&a=b&");

#if 0
        query p;
        error_code ec;
        using bnf::parse;
        parse("/1/2/3/4/5", ec, p);
        for(auto const& t : p.path())
            std::cout << t.str() << std::endl;
#endif
    }
};

TEST_SUITE(
    query_test,
    "boost.url.query");

} // rfc
} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/query_bnf.hpp>

#include <boost/url/bnf/range.hpp>
#include <boost/url/rfc/query_params_bnf.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"
#include <iostream>

namespace boost {
namespace urls {

class query_bnf_test
{
public:
    void
    run()
    {
        using T = query_bnf;

        bad <T>("%");

        good<T>("");
        good<T>("x");
        good<T>("x=");
        good<T>("x=y");
        good<T>("x=y&");
        good<T>("x=y&a");
        good<T>("x=y&a=b&");

        // some gen-delims
        for(auto c :
            "#[]"
            )
        {
            string_view s( &c, 1 );
            bad<T>(s);
        }

        // pchar / "/" / "?"
        good<T>(
            // unreserved
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            // sub-delims
            "!$&'()*+,;="
            // ":" / "@"
            ":@"
            // "/" / "?"
            "/?"
            );
    }
};

TEST_SUITE(
    query_bnf_test,
    "boost.url.query_bnf");

} // urls
} // boost

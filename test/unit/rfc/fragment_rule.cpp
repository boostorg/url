//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/fragment_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

#include <iostream>

namespace boost {
namespace urls {

class fragment_rule_test
{
public:
    void
    run()
    {
        using T = test_ref<
            fragment_rule,
            pct_encoded_str>;

        bad<T>("#");
        bad<T>("[");
        bad<T>("]");
        bad<T>("%F");

        good<T>("");
        good<T>("");
        good<T>("@");
        good<T>(".%ff");

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
    fragment_rule_test,
    "boost.url.fragment_rule");

} // urls
} // boost

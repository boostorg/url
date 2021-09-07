//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv_future_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class ipv_future_bnf_test
{
public:
    void
    run()
    {
        using T = ipv_future_bnf;

        bad<T>("v");
        bad<T>("v1");
        bad<T>("v1.");
        bad<T>("v1.@$");
        bad<T>("v.1");

        good<T>("v1.0");
        good<T>("v1.minor");
    }
};

TEST_SUITE(
    ipv_future_bnf_test,
    "boost.url.ipv_future_bnf");

} // urls
} // boost

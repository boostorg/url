//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv_future.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class ipv_future_test
{
public:
    void
    run()
    {
        using T = ipv_future;

        bad_ <T>("v");
        bad_ <T>("v1");
        bad_ <T>("v1.");
        bad_ <T>("v1.@$");
        bad_ <T>("v.1");

        good_<T>("v1.0");
        good_<T>("v1.minor");
    }
};

TEST_SUITE(
    ipv_future_test,
    "boost.url.ipv_future");

} // rfc
} // urls
} // boost

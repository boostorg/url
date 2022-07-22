//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv_future_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

class ipv_future_rule_test
{
public:
    void
    run()
    {
        auto const& t =
            ipv_future_rule;

        bad(t, "v");
        bad(t, "v1");
        bad(t, "v1.");
        bad(t, "v1.@$");
        bad(t, "v.1");
        bad(t, "w1.1");

        ok(t, "v1.0");
        ok(t, "v1.minor");
    }
};

TEST_SUITE(
    ipv_future_rule_test,
    "boost.url.ipv_future_rule");

} // urls
} // boost

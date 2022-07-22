//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ip_literal_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

struct ip_literal_rule_test
{
    void
    run()
    {
        auto const& t =
            ip_literal_rule;

        bad(t, "::");
        bad(t, "[");
        bad(t, "[:");
        bad(t, "[::");
        bad(t, "[]");
        bad(t, "[v8]");

        ok(t, "[::]");
        ok(t, "[v1.0]");
    }
};

TEST_SUITE(
    ip_literal_rule_test,
    "boost.url.ip_literal_rule");

} // urls
} // boost

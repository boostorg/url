//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ipv4_address_rule.hpp>

#include "test_rule.hpp"

namespace boost {
namespace urls {

struct ipv4_address_rule_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    ipv4_address_rule_test,
    "boost.url.ipv4_address_rule");

} // urls
} // boost

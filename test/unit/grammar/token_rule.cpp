//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/token_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace rfc {

class token_rule_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    token_rule_test,
    "boost.url.token_rule");

} // rfc
} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/fragment.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class fragment_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    fragment_test,
    "boost.url.fragment");

} // rfc
} // urls
} // boost

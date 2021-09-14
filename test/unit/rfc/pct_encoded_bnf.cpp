//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/pct_encoded_bnf.hpp>

#include <boost/url/detail/test/test_suite.hpp>

namespace boost {
namespace urls {

class pct_encoded_bnf_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    pct_encoded_bnf_test,
    "boost.url.pct_encoded_bnf");

} // urls
} // boost

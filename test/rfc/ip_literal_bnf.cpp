//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/ip_literal_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class ip_literal_bnf_test
{
public:
    void
    run()
    {
        using T = ip_literal_bnf;

        bad<T>("::");
        bad<T>("[");
        bad<T>("[:");
        bad<T>("[::");
        bad<T>("[]");
        bad<T>("[v8]");

        good<T>("[::]");
        good<T>("[v1.0]");
    }
};

TEST_SUITE(
    ip_literal_bnf_test,
    "boost.url.ip_literal_bnf");

} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/reg_name_bnf.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class reg_name_bnf_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    reg_name_bnf_test,
    "boost.url.reg_name_bnf");

} // urls
} // boost

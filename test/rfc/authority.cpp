//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/authority.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class authority_test
{
public:
    void
    run()
    {
        good_<authority>("me@you.com");
        good_<authority>("user:pass@");
        good_<authority>("user:1234");
    }
};

TEST_SUITE(
    authority_test,
    "boost.url.authority");

} // rfc
} // urls
} // boost

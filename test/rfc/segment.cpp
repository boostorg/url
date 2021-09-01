//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/segment.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class segment_test
{
public:
    void
    run()
    {
        good<segment>("");
        good<segment>("a");
        good<segment>("a:");
        good<segment>("%42");
        good<segment>("%42%43");

        bad <segment_nz>("");
        good<segment_nz>("a");
        good<segment_nz>("a:");
        good<segment_nz>("%42a");
        good<segment_nz>("%42%43");

        bad <segment_nz_nc>("");
        bad <segment_nz_nc>("a:");
        good<segment_nz_nc>("a");
        good<segment_nz_nc>("%42a");
        good<segment_nz_nc>("%42%43");
    }
};

TEST_SUITE(
    segment_test,
    "boost.url.segment");

} // rfc
} // urls
} // boost

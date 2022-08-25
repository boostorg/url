//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/encoded_rule.hpp>

#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/pchars.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class encoded_rule_test
{
public:
    void
    run()
    {
        // javadoc
        {
            result< decode_view > rv = grammar::parse( "Program%20Files", encoded_rule( pchars ) );
            (void)rv;
        }
    }
};

TEST_SUITE(
    encoded_rule_test,
    "boost.url.encoded_rule");

} // urls
} // boost

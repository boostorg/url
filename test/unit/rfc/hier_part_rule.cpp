//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/hier_part_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

class hier_part_rule_test
{
public:
    void
    run()
    {
        auto const& t = hier_part_rule;
        {
            ok(t, "");
            ok(t, "/");
            ok(t, "//");
            ok(t, "//user:pass@");
            ok(t, "//boost.org");
            ok(t, "//1.2.3.4:8080");
            ok(t, "//1.2.3.4:8080/");
            ok(t, "//1.2.3.4:8080/x");
            ok(t, "//1.2.3.4:8080/x/");
            ok(t, "//1.2.3.4:8080////");
            ok(t, "/x");
            ok(t, "/x/");
            ok(t, "/x/y");
            ok(t, "/x/y//");
            ok(t, "x");
            ok(t, "x/");
            ok(t, "x//");
            ok(t, "x/y/z");
            ok(t, "x//y///z///");
            ok(t, ":/"); // colon ok in hier-part
        }
        {
            auto rv = grammar::parse(
                "/1/2/3/4/5",
                hier_part_rule);
            auto const& v = *rv;
            BOOST_TEST_EQ(v.path.path, "/1/2/3/4/5");
            BOOST_TEST_EQ(v.path.count, 5u);
        }
    }
};

TEST_SUITE(
    hier_part_rule_test,
    "boost.url.hier_part_rule");

} // urls
} // boost

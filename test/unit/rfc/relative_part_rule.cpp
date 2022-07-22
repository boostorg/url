//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/relative_part_rule.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

class relative_part_rule_test
{
public:
    /* ASCII HEX

        %   25
        .   2e
        /   2f
        :   3a
    */

    void
    testRelativePart()
    {
    /*
        relative-part = "//" authority path-abempty
                      / path-absolute
                      / path-noscheme
		              / path-abempty
                      / path-empty
    */
        auto const& r =
            relative_part_rule;

        // "//" authority path-abempty
        {
            ok(r, "//");
            ok(r, "///");
            ok(r, "////");
            ok(r, "///x");
            ok(r, "///:");
            ok(r, "///x/");
            ok(r, "///%3a/");
            ok(r, "///%20");
            ok(r, "///%20");
            ok(r, "///%25");
            ok(r, "///%25%2e");

            ok(r, "//x");
            ok(r, "//x/");
            ok(r, "//x//");
            ok(r, "//x/x");
            ok(r, "//x/:");
            ok(r, "//x/x/");
            ok(r, "//x/%3a/");
            ok(r, "//x/%20");
            ok(r, "//x/%20");
            ok(r, "//x/%25");
            ok(r, "//x/%25%2e");

            ok(r, "");
            ok(r, "/");
            ok(r, "//");
            ok(r, "//user:pass@");
            ok(r, "//boost.org");
            ok(r, "//1.2.3.4:8080");
            ok(r, "//1.2.3.4:8080/");
            ok(r, "//1.2.3.4:8080/x");
            ok(r, "//1.2.3.4:8080/x/");
            ok(r, "//1.2.3.4:8080////");
            ok(r, "/x");
            ok(r, "/x/");
            ok(r, "/x/y");
            ok(r, "/x/y//");
            ok(r, "x");
            ok(r, "x/");
            ok(r, "x//");
            ok(r, "x/y/z");
            ok(r, "x//y///z///");

            bad(r, ":/"); // colon not ok in relative-part
        }

        // path-absolute
        {
            ok(r, "/");
            ok(r, "/x");
            ok(r, "/x/");
            ok(r, "/:/");
            ok(r, "/x//");
            ok(r, "/%20");
            ok(r, "/:%20");
            ok(r, "/%20");
            ok(r, "/%25");
            ok(r, "/%25%2e");
        }

        // path-noscheme
        {
            ok(r, ".");
            ok(r, "x");
            ok(r, "%20");
            ok(r, "%2f");
            ok(r, "a/");
            ok(r, "a//");
            ok(r, "a/x");
            ok(r, "a/x/");
            ok(r, "a/x//");
            ok(r, "a///");
        }

        // path-abempty
        {
            ok(r, "");
            ok(r, "/");
            ok(r, "//");
            ok(r, "/x");
            ok(r, "/:");
            ok(r, "/x/");
            ok(r, "/%3a/");
            ok(r, "/%20");
            ok(r, "/%20");
            ok(r, "/%25");
            ok(r, "/%25%2e");
        }

        // path/coverage
        {
            auto rv = grammar::parse(
                "/1/2/3/4/5",
                relative_part_rule);
            auto const& v = *rv;
            BOOST_TEST_EQ(v.path.path, "/1/2/3/4/5");
            BOOST_TEST_EQ(v.path.count, 5u);
        }
    }

    void
    run()
    {    
        testRelativePart();
    }
};

TEST_SUITE(
    relative_part_rule_test,
    "boost.url.relative_part_rule");

} // urls
} // boost

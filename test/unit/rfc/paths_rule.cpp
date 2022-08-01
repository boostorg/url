//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/paths_rule.hpp>

#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/static_assert.hpp>
#include "test_rule.hpp"

namespace boost {
namespace urls {

class paths_rule_test
{
public:
    /* ASCII HEX

        %   25
        .   2e
        /   2f
        :   3a
    */
    void
    test_path_abempty_rule()
    {
        // path-abempty  = *( "/" segment )
        auto const& t =
            path_abempty_rule;

        ok(t, "");
        ok(t, "/");
        ok(t, "//");
        ok(t, "/x");
        ok(t, "/:");
        ok(t, "/x/");
        ok(t, "/%3a/");
        ok(t, "/%20");
        ok(t, "/%20");
        ok(t, "/%25");
        ok(t, "/%25%2e");

        bad(t, ".");
        bad(t, ":");
        bad(t, "x");
        bad(t, "%20");
        bad(t, "%2f");
        bad(t, "a/");
        bad(t, " ");
    }

    void
    test_path_absolute_rule()
    {
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        auto const& t =
            path_absolute_rule;

        ok(t, "/");
        ok(t, "/x");
        ok(t, "/x/");
        ok(t, "/:/");
        ok(t, "/x//");
        ok(t, "/%20");
        ok(t, "/:%20");
        ok(t, "/%20");
        ok(t, "/%25");
        ok(t, "/%25%2e");

        bad(t, "");
        bad(t, "//");
        bad(t, ".");
        bad(t, ":");
        bad(t, "x");
        bad(t, "%20");
        bad(t, "%2f");
        bad(t, "a/");
        bad(t, " ");
    }

    void
    test_path_noscheme_rule()
    {
        // path-noscheme = segment-nz-nc *( "/" segment )
        auto const& t =
            path_noscheme_rule;

        ok(t, ".");
        ok(t, "x");
        ok(t, "%20");
        ok(t, "%2f");
        ok(t, "a/");
        ok(t, "a//");
        ok(t, "a/x");
        ok(t, "a/x/");
        ok(t, "a/x//");
        ok(t, "a///");

        bad(t, "");
        bad(t, " ");
        bad(t, ":");
        bad(t, "/");
        bad(t, "/x");
        bad(t, "//");
        bad(t, "/x/");
        bad(t, "/:/");
        bad(t, "/x//");
        bad(t, "/%20");
        bad(t, "/:%20");
        bad(t, "/%20");
        bad(t, "/%25");
        bad(t, "/%25%2e");
    }

    void
    test_path_rootless_rule()
    {
        // path-rootless = segment-nz *( "/" segment )
        auto const& t =
            path_rootless_rule;

        ok(t, ".");
        ok(t, ":");
        ok(t, ":/");
        ok(t, "::/");
        ok(t, "://");
        ok(t, ":/:/");
        ok(t, "x");
        ok(t, "%20");
        ok(t, "%2f");
        ok(t, "a/");
        ok(t, "a//");
        ok(t, "a/x");
        ok(t, "a/x/");
        ok(t, "a/x//");
        ok(t, "a///");

        bad(t, "");
        bad(t, " ");
        bad(t, "/");
        bad(t, "/x");
        bad(t, "//");
        bad(t, "/x/");
        bad(t, "/:/");
        bad(t, "/x//");
        bad(t, "/%20");
        bad(t, "/:%20");
        bad(t, "/%20");
        bad(t, "/%25");
        bad(t, "/%25%2e");
    }

    void
    run()
    {
        test_path_abempty_rule();
        test_path_absolute_rule();
        test_path_noscheme_rule();
        test_path_rootless_rule();
    }
};

TEST_SUITE(
    paths_rule_test,
    "boost.url.paths_rule");

} // urls
} // boost

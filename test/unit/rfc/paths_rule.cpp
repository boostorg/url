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
#include <boost/url/grammar/range.hpp>
#include <boost/static_assert.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(
    grammar::is_range<path_abempty_rule>::value);
BOOST_STATIC_ASSERT(
    grammar::is_range<path_absolute_rule>::value);
BOOST_STATIC_ASSERT(
    grammar::is_range<path_noscheme_rule>::value);
BOOST_STATIC_ASSERT(
    grammar::is_range<path_rootless_rule>::value);

class paths_rule_test
{
public:
    template<class T>
    void
    good(string_view s)
    {
        T t;
        error_code ec;
        if(! BOOST_TEST(
            grammar::parse_string(s,
                ec, t)))
            BOOST_TEST(ec.failed());
        else
            BOOST_TEST(! ec.failed());
    }

    template<class T>
    void
    bad(string_view s)
    {
        T t;
        error_code ec;
        if(! BOOST_TEST(
            ! grammar::parse_string(s,
                ec, t)))
            BOOST_TEST(! ec.failed());
        else
            BOOST_TEST(ec.failed());
    }

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
        using T = path_abempty_rule;

        good<T>("");
        good<T>("/");
        good<T>("//");
        good<T>("/x");
        good<T>("/:");
        good<T>("/x/");
        good<T>("/%3a/");
        good<T>("/%20");
        good<T>("/%20");
        good<T>("/%25");
        good<T>("/%25%2e");

        bad<T>(".");
        bad<T>(":");
        bad<T>("x");
        bad<T>("%20");
        bad<T>("%2f");
        bad<T>("a/");
        bad<T>(" ");
    }

    void
    test_path_absolute_rule()
    {
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        using T = path_absolute_rule;

        good<T>("/");
        good<T>("/x");
        good<T>("/x/");
        good<T>("/:/");
        good<T>("/x//");
        good<T>("/%20");
        good<T>("/:%20");
        good<T>("/%20");
        good<T>("/%25");
        good<T>("/%25%2e");

        bad<T>("");
        bad<T>("//");
        bad<T>(".");
        bad<T>(":");
        bad<T>("x");
        bad<T>("%20");
        bad<T>("%2f");
        bad<T>("a/");
        bad<T>(" ");
    }

    void
    test_path_noscheme_rule()
    {
        // path-noscheme = segment-nz-nc *( "/" segment )
        using T = path_noscheme_rule;

        good<T>(".");
        good<T>("x");
        good<T>("%20");
        good<T>("%2f");
        good<T>("a/");
        good<T>("a//");
        good<T>("a/x");
        good<T>("a/x/");
        good<T>("a/x//");
        good<T>("a///");

        bad<T>("");
        bad<T>(" ");
        bad<T>(":");
        bad<T>("/");
        bad<T>("/x");
        bad<T>("//");
        bad<T>("/x/");
        bad<T>("/:/");
        bad<T>("/x//");
        bad<T>("/%20");
        bad<T>("/:%20");
        bad<T>("/%20");
        bad<T>("/%25");
        bad<T>("/%25%2e");
    }

    void
    test_path_rootless_rule()
    {
        // path-rootless = segment-nz *( "/" segment )
        using T = path_rootless_rule;

        good<T>(".");
        good<T>(":");
        good<T>(":/");
        good<T>("::/");
        good<T>("://");
        good<T>(":/:/");
        good<T>("x");
        good<T>("%20");
        good<T>("%2f");
        good<T>("a/");
        good<T>("a//");
        good<T>("a/x");
        good<T>("a/x/");
        good<T>("a/x//");
        good<T>("a///");

        bad<T>("");
        bad<T>(" ");
        bad<T>("/");
        bad<T>("/x");
        bad<T>("//");
        bad<T>("/x/");
        bad<T>("/:/");
        bad<T>("/x//");
        bad<T>("/%20");
        bad<T>("/:%20");
        bad<T>("/%20");
        bad<T>("/%25");
        bad<T>("/%25%2e");
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

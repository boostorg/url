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
    testRelativePart()
    {
    /*
        relative-part = "//" authority path-abempty
                      / path-absolute
                      / path-noscheme
		              / path-abempty
                      / path-empty
    */
        using T = relative_part_rule;

        // "//" authority path-abempty
        {
            good<T>("//");
            good<T>("///");
            good<T>("////");
            good<T>("///x");
            good<T>("///:");
            good<T>("///x/");
            good<T>("///%3a/");
            good<T>("///%20");
            good<T>("///%20");
            good<T>("///%25");
            good<T>("///%25%2e");

            good<T>("//x");
            good<T>("//x/");
            good<T>("//x//");
            good<T>("//x/x");
            good<T>("//x/:");
            good<T>("//x/x/");
            good<T>("//x/%3a/");
            good<T>("//x/%20");
            good<T>("//x/%20");
            good<T>("//x/%25");
            good<T>("//x/%25%2e");

            good<T>("");
            good<T>("/");
            good<T>("//");
            good<T>("//user:pass@");
            good<T>("//boost.org");
            good<T>("//1.2.3.4:8080");
            good<T>("//1.2.3.4:8080/");
            good<T>("//1.2.3.4:8080/x");
            good<T>("//1.2.3.4:8080/x/");
            good<T>("//1.2.3.4:8080////");
            good<T>("/x");
            good<T>("/x/");
            good<T>("/x/y");
            good<T>("/x/y//");
            good<T>("x");
            good<T>("x/");
            good<T>("x//");
            good<T>("x/y/z");
            good<T>("x//y///z///");

            bad<T>(":/"); // colon not ok in relative-part
        }

        // path-absolute
        {
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
        }

        // path-noscheme
        {
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
        }

        // path-abempty
        {
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
        }

        // path/coverage
        {
            error_code ec;
            relative_part_rule t;
            grammar::parse_string(
                "/1/2/3/4/5", ec, t);
            BOOST_TEST(t.path.path == "/1/2/3/4/5");
            BOOST_TEST(t.path.count == 5);
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

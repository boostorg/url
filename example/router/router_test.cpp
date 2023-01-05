//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <router.hpp>
#include <src.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct router_test
{
    static
    void
    good(
        string_view pattern,
        string_view request)
    {
        router<int> r;
        r.route(pattern, 1);
        router<int>::match_results m = r.match(request);
        if (BOOST_TEST(m))
        {
            BOOST_TEST(m.has_value());
            BOOST_TEST_NOT(m.has_error());
            BOOST_TEST_EQ(*m, 1);
            BOOST_TEST_EQ(m.value(), 1);
        }
    };

    static
    void
    bad(
        string_view pattern,
        string_view request)
    {
        router<int> r;
        r.route(pattern, 1);
        router<int>::match_results rm = r.match(request);
        BOOST_TEST_NOT(rm);
        BOOST_TEST_NOT(rm.has_value());
        BOOST_TEST(rm.has_error());
        BOOST_TEST_THROWS(rm.value(), system_error);
    };

    static
    void
    testPatterns()
    {
        good("user", "user");
        good("user/view", "user/view");

        // match all / no type
        good("user/{}", "user/johndoe");
        good("user/{name}", "user/johndoe");
        good("user/{id}", "user/123");
        good("user/{name}/{op}", "user/johndoe/r");
        good("user/{name}/op/{op}", "user/johndoe/op/r");

        // dot segments
        good("user/{name}", "user/././johndoe");
        good("user/{name}", "user/b/../johndoe");
        good("user/b", "user/c/../b");
        good("user/b", "../a/user/c/../b");
        good("user/././{name}", "user/johndoe");
        good("user/b/../{name}", "user/johndoe");
        good("user/c/../b", "user/b");
        good("../a/user/c/../b", "user/b");

        // pct-encoded segments
        good("%75ser/{name}", "user/johndoe");
        good("user/{name}", "%75ser/johndoe");

        // empty segments
        good("user//{name}", "user//johndoe");
        bad("user//{name}", "user/johndoe");

        // optional segment
        good("user/{name}/{op?}", "user/johndoe");
        good("user/{name}/{op?}", "user/johndoe/");
        good("user/{name}/{op?}", "user/johndoe/r");
        bad("user/{name}/{op?}", "user/johndoe/r/r");
        good("user/{name}/{op?}/b", "user/johndoe/r/b");
        good("user/{name}/{op?}/b", "user/johndoe/b");
        good("user/{name}/{op?}/{op2?}", "user/johndoe");
        good("user/{name}/{op?}/{op2?}", "user/johndoe/");
        good("user/{name}/{op?}/{op2?}", "user/johndoe/r");
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe/b");
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe//b");
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe/r/b");

        // star segment
        good("user/{name}/{op*}", "user/johndoe");
        good("user/{name}/{op*}", "user/johndoe/");
        good("user/{name}/{op*}", "user/johndoe/r");
        good("user/{name}/{op*}", "user/johndoe/r/r1");
        good("user/{name}/{op*}", "user/johndoe/r/r1/r2");
        good("user/{name}/{op*}/{op2*}", "user/johndoe");
        good("user/{name}/{op*}/{op2*}", "user/johndoe/");
        good("user/{name}/{op*}/{op2*}", "user/johndoe/r");
        good("user/{name}/{op*}/b", "user/johndoe/b");
        good("user/{name}/{op*}/b", "user/johndoe/r/b");
        good("user/{name}/{op*}/b", "user/johndoe/r/r1/b");
        good("user/{name}/{op*}/b", "user/johndoe/r/r1/r2/b");
        good("user/{name}/{op*}/b", "user/johndoe/b");

        // plus segment
        bad("user/{name}/{op+}", "user/johndoe");
        good("user/{name}/{op+}", "user/johndoe/");
        good("user/{name}/{op+}", "user/johndoe/r");
        good("user/{name}/{op+}", "user/johndoe/r/r1");
        good("user/{name}/{op+}", "user/johndoe/r/r1/r2");
        bad("user/{name}/{op+}/{op2+}", "user/johndoe");
        bad("user/{name}/{op+}/{op2+}", "user/johndoe/");
        bad("user/{name}/{op+}/{op2+}", "user/johndoe/r");
        bad("user/{name}/{op+}/b", "user/johndoe/b");
        good("user/{name}/{op+}/b", "user/johndoe/r/b");
        good("user/{name}/{op+}/b", "user/johndoe/r/r1/b");
        good("user/{name}/{op+}/b", "user/johndoe/r/r1/r2/b");
        bad("user/{name}/{op+}/b", "user/johndoe/b");
    }

    void
    run()
    {
        testPatterns();

        // to be continued:
        // - return match results
        //     - match_results can lazily return indexes or ids
        // - improve example with more routes, so it makes more sense
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // urls
} // boost

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

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct router_test
{
    void
    testPatterns()
    {
        auto good =[](
            string_view pattern,
            string_view request)
        {
            router<int> r;
            r.route(pattern, 1);
            using match_res = router<int>::match_results;
            result<match_res> rm = r.match(request);
            if (BOOST_TEST(rm))
            {
                match_res m = *rm;
                BOOST_TEST_EQ(*m, 1);
            }
        };

        auto bad =[](
            string_view pattern,
            string_view request)
        {
            router<int> r;
            r.route(pattern, 1);
            using match_res = router<int>::match_results;
            result<match_res> rm = r.match(request);
            BOOST_TEST_NOT(rm);
        };

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
        // - use the urls::format url_template syntax
        // - create lazy range that allows iterating
        //     the match results
        // - complete match results
        // - create base_router
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // urls
} // boost

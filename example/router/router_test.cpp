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
        string_view request,
        std::initializer_list<string_view> matches = {},
        std::initializer_list<std::pair<string_view, string_view>> args = {})
    {
        router<int> r;
        r.route(pattern, 1);
        router<int>::match_results m = r.match(request);
        if (!BOOST_TEST(m))
            return;
        BOOST_TEST(m.has_value());
        BOOST_TEST_NOT(m.has_error());
        BOOST_TEST_EQ(*m, 1);
        BOOST_TEST_EQ(m.value(), 1);

        BOOST_TEST_EQ(matches.size(), m.size());

        {
            auto it0 = matches.begin();
            auto it1 = m.begin();
            auto end0 = matches.end();
            auto end1 = m.end();
            while (
                it0 != end0 &&
                it1 != end1)
            {
                BOOST_TEST_EQ(*it0++, *it1++);
            }
        }

        {
            auto it = args.begin();
            auto end = args.end();
            while (it != end)
            {
                BOOST_TEST(m.find(it->first) != m.end());
                BOOST_TEST_EQ(m[it->first], it->second);
                ++it;
            }
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
        good("user", "user", {});
        good("user/view", "user/view", {});

        // match all / no type
        good("user/{}", "user/johndoe", {"johndoe"});
        good("user/{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{id}", "user/123", {"123"}, {{"id", "123"}});
        good("user/{name}/{op}", "user/johndoe/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/op/{op}", "user/johndoe/op/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});

        // dot segments
        good("user/{name}", "user/././johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{name}", "user/b/../johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/b", "user/c/../b");
        good("user/b", "../a/user/c/../b");
        good("user/././{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/b/../{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/c/../b", "user/b");
        good("../a/user/c/../b", "user/b");

        // pct-encoded segments
        good("%75ser/{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{name}", "%75ser/johndoe", {"johndoe"}, {{"name", "johndoe"}});

        // empty segments
        good("user//{name}", "user//johndoe", {"johndoe"}, {{"name", "johndoe"}});
        bad("user//{name}", "user/johndoe");

        // optional segment
        good("user/{name}/{op?}", "user/johndoe", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op?}", "user/johndoe/", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op?}", "user/johndoe/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        bad("user/{name}/{op?}", "user/johndoe/r/r");
        good("user/{name}/{op?}/b", "user/johndoe/r/b", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/{op?}/b", "user/johndoe/b", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op?}/{op2?}", "user/johndoe", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op?}/{op2?}", "user/johndoe/", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op?}/{op2?}", "user/johndoe/r", {"johndoe", "r", ""}, {{"name", "johndoe"}, {"op", "r"}, {"op2", ""}});
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe/b", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe//b", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op?}/{op2?}/b", "user/johndoe/r/b", {"johndoe", "r", ""}, {{"name", "johndoe"}, {"op", "r"}, {"op2", ""}});

        // star segment
        good("user/{name}/{op*}", "user/johndoe", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op*}", "user/johndoe/", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op*}", "user/johndoe/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/{op*}", "user/johndoe/r/r1", {"johndoe", "r/r1"}, {{"name", "johndoe"}, {"op", "r/r1"}});
        good("user/{name}/{op*}", "user/johndoe/r/r1/r2", {"johndoe", "r/r1/r2"}, {{"name", "johndoe"}, {"op", "r/r1/r2"}});
        good("user/{name}/{op*}/{op2*}", "user/johndoe", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op*}/{op2*}", "user/johndoe/", {"johndoe", "", ""}, {{"name", "johndoe"}, {"op", ""}, {"op2", ""}});
        good("user/{name}/{op*}/{op2*}", "user/johndoe/r", {"johndoe", "r", ""}, {{"name", "johndoe"}, {"op", "r"}, {"op2", ""}});
        good("user/{name}/{op*}/b", "user/johndoe/b", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op*}/b", "user/johndoe/r/b", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/{op*}/b", "user/johndoe/r/r1/b", {"johndoe", "r/r1"}, {{"name", "johndoe"}, {"op", "r/r1"}});
        good("user/{name}/{op*}/b", "user/johndoe/r/r1/r2/b", {"johndoe", "r/r1/r2"}, {{"name", "johndoe"}, {"op", "r/r1/r2"}});
        good("user/{name}/{op*}/b", "user/johndoe/b", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});

        // plus segment
        bad("user/{name}/{op+}", "user/johndoe");
        good("user/{name}/{op+}", "user/johndoe/", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op+}", "user/johndoe/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/{op+}", "user/johndoe/r/r1", {"johndoe", "r/r1"}, {{"name", "johndoe"}, {"op", "r/r1"}});
        good("user/{name}/{op+}", "user/johndoe/r/r1/r2", {"johndoe", "r/r1/r2"}, {{"name", "johndoe"}, {"op", "r/r1/r2"}});
        bad("user/{name}/{op+}/{op2+}", "user/johndoe");
        bad("user/{name}/{op+}/{op2+}", "user/johndoe/");
        bad("user/{name}/{op+}/{op2+}", "user/johndoe/r");
        bad("user/{name}/{op+}/b", "user/johndoe/b");
        good("user/{name}/{op+}/b", "user/johndoe/r/b", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/{op+}/b", "user/johndoe/r/r1/b", {"johndoe", "r/r1"}, {{"name", "johndoe"}, {"op", "r/r1"}});
        good("user/{name}/{op+}/b", "user/johndoe/r/r1/r2/b", {"johndoe", "r/r1/r2"}, {{"name", "johndoe"}, {"op", "r/r1/r2"}});
        bad("user/{name}/{op+}/b", "user/johndoe/b");
    }

    void
    run()
    {
        testPatterns();

        // to be continued:
        // - node::children should have a small buffer
        // - ::route() should count level of matches and throw if route contains more
        //   replacement fields that we can
        // - router<T> can provide storage for all resources
        // - fix matching * and + with ".." in it
        //     - if there are more ".." than segments, there's no match
        // - test branching
        // - coverage...
        // - implement Peter's `matches`
        // - improve example with more routes, so it makes more sense
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // urls
} // boost

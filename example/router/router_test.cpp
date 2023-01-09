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
    testPatterns()
    {
        // literal
        good("user", "user");
        good("user/view", "user/view");

        // empty
        good("", "/");
        good("/", "/");
        good("", "");
        good("/", "");

        // replace segment match
        good("user/{}", "user/johndoe", {"johndoe"});
        good("user/{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{id}", "user/123", {"123"}, {{"id", "123"}});
        good("user/{name}/{op}", "user/johndoe/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        good("user/{name}/op/{op}", "user/johndoe/op/r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});

        // path dot segments
        good("user/{name}", "user/././johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{name}", "user/b/../johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/b", "user/c/../b");
        good("user/b", "../a/user/c/../b");

        // template dot segments
        good("user/././{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/b/../{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/c/../b", "user/b");
        good("user/{name}/../{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("../a/user/c/../b", "user/b");

        // pct-encoded segments
        good("%75ser/{name}", "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good("user/{name}", "%75ser/johndoe", {"johndoe"}, {{"name", "johndoe"}});

        // empty segments
        good("user//{name}", "user//johndoe", {"johndoe"}, {{"name", "johndoe"}});
        bad("user//{name}", "user/johndoe");

        // optional segment match
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

        // optional segment match + dot segments
        good("user/{name}/{op?}", "user/johndoe/.", {"johndoe", ""}, {{"name", "johndoe"}, {"op", ""}});
        good("user/{name}/{op?}", "user/johndoe/./r", {"johndoe", "r"}, {{"name", "johndoe"}, {"op", "r"}});
        bad("user/{name}/{op?}", "user/johndoe/..");
        good("user/{name}/{op?}", "user/johndoe/../maria", {"maria", ""}, {{"name", "maria"}, {"op", ""}});
        good("user/{name}/{op?}", "user/johndoe/../maria/./r", {"maria", "r"}, {{"name", "maria"}, {"op", "r"}});
        good("user/{name}/{op?}", "user/johndoe/r/../s", {"johndoe", "s"}, {{"name", "johndoe"}, {"op", "s"}});

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

        // star segment match + dot segments
        good("user/{name}/{op*}", "user/johndoe/../r/../s", {"s", ""}, {{"name", "s"}, {"op", ""}});
        bad("user/{name}/{op*}", "user/johndoe/../../r/s");
        good("user/{name}/{op*}", "user/johndoe/../../user/s", {"s", ""}, {{"name", "s"}, {"op", ""}});
        good("user/{name}/{op*}", "user/johndoe/r/../../../user/s", {"s", ""}, {{"name", "s"}, {"op", ""}});
        good("user/{name}/{op*}", "user/johndoe/r/../s", {"johndoe", "r/../s"}, {{"name", "johndoe"}, {"op", "r/../s"}});
        good("user/{name}/{op*}", "user/johndoe/r/../s/..", {"johndoe", "r/../s/.."}, {{"name", "johndoe"}, {"op", "r/../s/.."}});
        bad("user/{name}/{op*}", "user/johndoe/r/../s/../..");

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

        // plus segment match + dot segments
        bad("user/{name}/{op+}", "user/johndoe/../r/../s");
        good("user/{name}/{op+}", "user/johndoe/../r/../s/", {"s", ""}, {{"name", "s"}, {"op", ""}});
        bad("user/{name}/{op+}", "user/johndoe/../../r/s");
        bad("user/{name}/{op+}", "user/johndoe/../../user/s");
        good("user/{name}/{op+}", "user/johndoe/../../user/s/", {"s", ""}, {{"name", "s"}, {"op", ""}});
        good("user/{name}/{op+}", "user/johndoe/../../user/s/r", {"s", "r"}, {{"name", "s"}, {"op", "r"}});
        bad("user/{name}/{op+}", "user/johndoe/r/../../../user/s");
        good("user/{name}/{op+}", "user/johndoe/r/../../../user/s/", {"s", ""}, {{"name", "s"}, {"op", ""}});
        good("user/{name}/{op+}", "user/johndoe/r/../s", {"johndoe", "s"}, {{"name", "johndoe"}, {"op", "s"}});
        bad("user/{name}/{op+}", "user/johndoe/r/../s/..");
        bad("user/{name}/{op+}", "user/johndoe/r/../s/../..");

        // segment tree
        good({"user/view", "user/{}"}, 1, "user/johndoe", {"johndoe"});
        good({"user/view", "user/{}"}, 0, "user/view", {});
        good({"user/{}", "user/view"}, 1, "user/view", {});
        good({"user/view", "user/{file*}"}, 1, "user/path/to/file.txt", {"path/to/file.txt"}, {{"file", "path/to/file.txt"}});
        good({"user/view", "user/{name}", "user/{file+}"}, 1, "user/johndoe", {"johndoe"}, {{"name", "johndoe"}});
        good({"user/view", "user/{name}", "user/{file+}"}, 2, "user/path/to/file.txt", {"path/to/file.txt"}, {{"file", "path/to/file.txt"}});

        // above root
        bad("user/../..");

        // invalid path
        bad("user", "invalid{path}");
    }

    static
    void
    good(
        std::initializer_list<string_view> patterns,
        std::size_t match_idx,
        string_view request,
        std::initializer_list<string_view> matches = {},
        std::initializer_list<std::pair<string_view, string_view>> args = {})
    {
        router<int> r;
        std::size_t i = 0;
        for (auto p: patterns)
            r.route(p, i++);
        router<int>::match_results<20> m;
        BOOST_TEST(r.match(request, m));
        if (!BOOST_TEST(m))
            return;
        BOOST_TEST(m.has_value());
        BOOST_TEST_NOT(m.has_error());
        BOOST_TEST_EQ(*m, match_idx);
        BOOST_TEST_EQ(m.value(), match_idx);

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
    good(
        string_view pattern,
        string_view request,
        std::initializer_list<string_view> matches = {},
        std::initializer_list<std::pair<string_view, string_view>> args = {})
    {
        return good({pattern}, 0, request, matches, args);
    };

    static
    void
    bad(
        string_view pattern,
        string_view request)
    {
        router<int> r;
        BOOST_TEST_NO_THROW(r.route(pattern, 0));
        router<int>::match_results<20> rm;
        BOOST_TEST_NOT(r.match(request, rm));
        BOOST_TEST_NOT(rm);
        BOOST_TEST_NOT(rm.has_value());
        BOOST_TEST(rm.has_error());
        BOOST_TEST_THROWS(rm.value(), system_error);
    };

    static
    void
    bad(
        string_view pattern)
    {
        router<int> r;
        BOOST_TEST_THROWS(r.route(pattern, 0), system_error);
    };

    void
    testMatchTo()
    {
        router<int> r;
        r.route("user/{name}/{op}/{id}", 0);
        string_view user;
        std::string op;
        std::size_t id{0};
        result<int> m = r.match_to("user/john/transaction/3%32", user, op, id);
        BOOST_TEST(m);
        BOOST_TEST_EQ(*m, 0);
        BOOST_TEST_EQ(user, "john");
        BOOST_TEST_EQ(op, "transaction");
        BOOST_TEST_EQ(id, 32);
    }

    void
    run()
    {
        testPatterns();
        testMatchTo();
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // urls
} // boost

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_view.hpp>

#include <boost/url/segments_encoded_view.hpp>
#include "test_suite.hpp"
#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

namespace boost {
namespace urls {

class segments_view_test
{
public:
    template<class It>
    static
    std::reverse_iterator<It>
    reverse(It const& it)
    {
        return std::reverse_iterator<It>(it);
    }

    void
    bad(string_view s,
        result<segments_encoded_view> (
            *f)(string_view))
    {
        segments_view sv;
        BOOST_TEST_THROWS(
            sv = f(s).value().decoded(),
            std::exception);
        BOOST_TEST(sv.empty());
        BOOST_TEST_EQ(sv.begin(), sv.end());
    }

    template<class T1, class T2>
    static
    bool
    vec_eq(
        std::vector<T1> const& v1,
        std::vector<T2> const& v2)
    {
        if(v1.size() != v2.size())
            return false;
        for(std::size_t i = 0;
            i < v1.size(); ++i)
            if(v1[i] != v2[i])
                return false;
        return true;
    }

    void
    check(
        string_view s,
        std::vector<
            string_view> const& v0,
        result<segments_encoded_view>(
            *f)(string_view))
    {
        segments_view sv;
        BOOST_TEST_NO_THROW(
            sv = f(s).value().decoded());
        // forward
        {
            std::vector<pct_encoded_view> v1;
            std::copy(
                sv.begin(),
                sv.end(),
                std::back_inserter(v1));
            BOOST_TEST(vec_eq(v0, v1));
        }
        // reverse
        {
            std::vector<pct_encoded_view> v1;
            std::copy(
                reverse(sv.end()),
                reverse(sv.begin()),
                std::back_inserter(v1));
            std::reverse(v1.begin(), v1.end());
            BOOST_TEST(vec_eq(v0, v1));
        }
    }

    //--------------------------------------------

    void
    testMembers()
    {
        // default constructor
        {
            segments_view sv;
            BOOST_TEST(sv.empty());
            BOOST_TEST_EQ(sv.size(), 0u);
            BOOST_TEST(
                sv.begin() == sv.end());
        }

        // operator=(segments_view const&)
        {
            segments_view s1;
            segments_view s2;
            s1 = s2;
            BOOST_TEST_EQ(s1.begin(), s2.begin());
        }

        // decoded
        {
            segments_view sv = parse_path(
                "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded();
            BOOST_TEST_EQ(sv.size(), 3u);
            BOOST_TEST(sv.is_absolute());
        }

        // is_absolute
        {
            segments_view sv;
            sv = parse_path("/path/to/file.txt"
                ).value().decoded();
            BOOST_TEST(sv.is_absolute());
            sv = parse_path("./my/downloads"
                ).value().decoded();
            BOOST_TEST(! sv.is_absolute());
        }
    }

    void
    testElementAccess()
    {
        // front
        // back
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            BOOST_TEST_EQ(sv.front(), "path");
            BOOST_TEST_EQ(sv.back(), "file.txt");
        }
    }

    void
    testIterators()
    {
        using iter_t =
            segments_view::iterator;

        // iterator()
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it1;
            iter_t it2;
            BOOST_TEST_EQ(it1, it2);
            BOOST_TEST_NE(it1, sv.begin());
            BOOST_TEST_NE(it2, sv.begin());
        }

        // iterator(iterator const&)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it1 = sv.begin();
            iter_t it2(it1);
            BOOST_TEST_EQ(it2, it1);
            BOOST_TEST_EQ(*it1, *it2);
            BOOST_TEST_EQ(*it1, "path");
            BOOST_TEST_EQ(*it2, "path");
        }

        // operator=(iterator const&)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it1;
            it1 = sv.begin();
            iter_t it2;
            it2 = sv.end();
            BOOST_TEST_NE(it2, it1);
            it2 = it1;
            BOOST_TEST_EQ(it2, it1);
            BOOST_TEST_EQ(*it1, *it2);
            BOOST_TEST_EQ(*it1, "path");
            BOOST_TEST_EQ(*it2, "path");
        }

        // operator*
        // operator++
        // operator++(int)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it = sv.begin();
            BOOST_TEST_EQ(*it, "path");
            BOOST_TEST_EQ(*++it, "to");
            BOOST_TEST_EQ(*it++, "to");
            BOOST_TEST_EQ(*it++, "file.txt");
            BOOST_TEST_EQ(it, sv.end());
        }

        // operator*
        // operator--
        // operator--(int)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it = sv.end();
            BOOST_TEST_EQ(*--it, "file.txt");
            BOOST_TEST_EQ(*it--, "file.txt");
            BOOST_TEST_EQ(*it, "to");
            BOOST_TEST_EQ(*--it, "path");
            BOOST_TEST_EQ(it, sv.begin());
        }

        // operator ==
        // operator !=
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded();
            iter_t it = sv.begin();
            BOOST_TEST_EQ(it, sv.begin());
            BOOST_TEST_NE(it, sv.end());
            BOOST_TEST_NE(++it, sv.begin());
            BOOST_TEST_NE(it++, sv.end());
        }

        // value_type outlives reference
        {
            segments_view::value_type v;
            {
                segments_view se = parse_path(
                    "path/to/the/file.txt").value();
                segments_view::reference r =
                    *se.begin();
                v = segments_view::value_type(r);
            }
            BOOST_TEST_EQ(v, "path");
        }
    }

    //--------------------------------------------

    void
    test_parse_path()
    {
    /*
        path          = [ "/" ] segment *( "/" segment )
    */
        check("", {}, &parse_path);
        check("/", {}, &parse_path);
        check("/a", {"a"}, &parse_path);
        check("/:", {":"}, &parse_path);
        check("/:/", {":",""}, &parse_path);
        check("/a/", {"a",""}, &parse_path);
        check("/a/b", {"a","b"}, &parse_path);
        check("/%41/b", {"A","b"}, &parse_path);
        check("///b", {"","","b"}, &parse_path);
        check("/%2f/b", {"/","b"}, &parse_path);
        check("/%2541//", {"%41","",""}, &parse_path);
        check("/a/b/c", {"a","b","c"}, &parse_path);
        check("a", {"a"}, &parse_path);
        check("a/", {"a", ""}, &parse_path);

        bad("/%2", &parse_path);
        bad("/%%", &parse_path);
    }

    void
    testCapacity()
    {
        segments_view sv;
        BOOST_TEST(sv.empty());
        BOOST_TEST_EQ(sv.size(), 0u);
        sv = parse_path(
            "/path/to/file.txt").value().decoded();
        BOOST_TEST(! sv.empty());
        BOOST_TEST_EQ(sv.size(), 3u);
        sv = {};
        BOOST_TEST(sv.empty());
        BOOST_TEST_EQ(sv.size(), 0u);
    }

    void
    testOutput()
    {
        {
            // empty
            std::stringstream ss;
            segments_view sv = parse_path(
                "").value().decoded();
            BOOST_TEST(!sv.is_absolute());
            ss << sv;
            BOOST_TEST_EQ(ss.str(), "");
        }
        {
            // absolute
            std::stringstream ss;
            segments_view sv = parse_path(
                "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded();
            ss << sv;
            auto str = ss.str();
            BOOST_TEST_EQ(str, "/path/to/file.txt");
        }
        {
            // relative
            std::stringstream ss;
            segments_view sv = parse_path(
                "%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded();
            ss << sv;
            BOOST_TEST_EQ(ss.str(), "path/to/file.txt");
        }
    }

    void
    testExamples()
    {
    }

    void
    run()
    {
        testMembers();
        testElementAccess();
        testIterators();
        testCapacity();
        testOutput();

        test_parse_path();

        testExamples();
    }
};

TEST_SUITE(
    segments_view_test,
    "boost.url.segments_view");

} // urls
} // boost

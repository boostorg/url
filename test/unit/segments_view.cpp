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
#include <boost/url/static_pool.hpp>
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
#if __cpp_lib_ranges >= 201911
    /*
    BOOST_STATIC_ASSERT(
        std::bidirectional_range<
            segments_view>);
    */

    BOOST_STATIC_ASSERT(
        std::bidirectional_iterator<
            segments_view::iterator>);
#endif

    static_pool<4096> sp_;

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
            sv = f(s).value().decoded(sp_.allocator()),
            std::exception);
        BOOST_TEST(sv.empty());
        BOOST_TEST(sv.begin() == sv.end());
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
            sv = f(s).value().decoded(sp_.allocator()));
        // forward
        {
            std::vector<const_string> v1;
            std::copy(
                sv.begin(),
                sv.end(),
                std::back_inserter(v1));
            BOOST_TEST(vec_eq(v0, v1));
        }
        // reverse
        {
            std::vector<const_string> v1;
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
            BOOST_TEST(sv.size() == 0);
            BOOST_TEST(
                sv.begin() == sv.end());
        }

        // decoded
        {
            segments_view sv = parse_path(
                "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded(sp_.allocator());
            BOOST_TEST(sv.size() == 3);
            BOOST_TEST(sv.is_absolute());
        }

        // is_absolute
        {
            segments_view sv;
            sv = parse_path("/path/to/file.txt"
                ).value().decoded(sp_.allocator());
            BOOST_TEST(sv.is_absolute());
            sv = parse_path("./my/downloads"
                ).value().decoded(sp_.allocator());
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
                "/path/to/file.txt").value().decoded(sp_.allocator());
            BOOST_TEST(sv.front() == "path");
            BOOST_TEST(sv.back() == "file.txt");
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
                "/path/to/file.txt").value().decoded(sp_.allocator());
            iter_t it1;
            iter_t it2;
            BOOST_TEST(it1 == it2);
            BOOST_TEST(it1 != sv.begin());
            BOOST_TEST(it2 != sv.begin());
        }

        // iterator(iterator const&)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded(sp_.allocator());
            iter_t it1 = sv.begin();
            iter_t it2(it1);
            BOOST_TEST(it2 == it1);
            BOOST_TEST(*it1 == *it2);
            BOOST_TEST(*it1 == "path");
            BOOST_TEST(*it2 == "path");
        }

        // operator=(iterator const&)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded(
                    sp_.allocator());
            iter_t it1;
            it1 = sv.begin();
            iter_t it2;
            it2 = sv.end();
            BOOST_TEST(it2 != it1);
            it2 = it1;
            BOOST_TEST(it2 == it1);
            BOOST_TEST(*it1 == *it2);
            BOOST_TEST(*it1 == "path");
            BOOST_TEST(*it2 == "path");
        }

        // operator*
        // operator++
        // operator++(int)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded(
                    sp_.allocator());
            iter_t it = sv.begin();
            BOOST_TEST(*it == "path");
            BOOST_TEST(*++it == "to");
            BOOST_TEST(*it++ == "to");
            BOOST_TEST(*it++ == "file.txt");
            BOOST_TEST(it == sv.end());
        }

        // operator*
        // operator--
        // operator--(int)
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded(
                    sp_.allocator());
            iter_t it = sv.end();
            BOOST_TEST(*--it == "file.txt");
            BOOST_TEST(*it-- == "file.txt");
            BOOST_TEST(*it == "to");
            BOOST_TEST(*--it == "path");
            BOOST_TEST(it == sv.begin());
        }

        // operator ==
        // operator !=
        {
            segments_view sv = parse_path(
                "/path/to/file.txt").value().decoded(
                    sp_.allocator());
            iter_t it = sv.begin();
            BOOST_TEST(it == sv.begin());
            BOOST_TEST(it != sv.end());
            BOOST_TEST(++it != sv.begin());
            BOOST_TEST(it++ != sv.end());
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
    test_parse_path_abempty()
    {
    /*
        path-abempty  = *( "/" segment )
    */
        check("", {}, &parse_path_abempty);
        check("/", {}, &parse_path_abempty);
        check("/a", {"a"}, &parse_path_abempty);
        check("/:", {":"}, &parse_path_abempty);
        check("/:/", {":",""}, &parse_path_abempty);
        check("/a/", {"a",""}, &parse_path_abempty);
        check("/a/b", {"a","b"}, &parse_path_abempty);
        check("/%41/b", {"A","b"}, &parse_path_abempty);
        check("///b", {"","","b"}, &parse_path_abempty);
        check("/%2f/b", {"/","b"}, &parse_path_abempty);
        check("/%2541//", {"%41","",""}, &parse_path_abempty);
        check("/a/b/c", {"a","b","c"}, &parse_path_abempty);

        bad("a", &parse_path_abempty);
        bad("a/", &parse_path_abempty);
        bad("/%2", &parse_path_abempty);
        bad("/%%", &parse_path_abempty);
    }

    void
    test_parse_path_absolute()
    {
    /*
        path-absolute = "/" [ segment-nz *( "/" segment ) ]
    */
        check("/", {}, &parse_path_absolute);
        check("/a", {"a"}, &parse_path_absolute);
        check("/a/", {"a",""}, &parse_path_absolute);
        check("/:", {":"}, &parse_path_absolute);
        check("/:/", {":",""}, &parse_path_absolute);
        check("/a/b", {"a","b"}, &parse_path_absolute);
        check("/%41/b", {"A","b"}, &parse_path_absolute);
        check("/%2f/b", {"/","b"}, &parse_path_absolute);
        check("/%2541//", {"%41","",""}, &parse_path_absolute);
        check("/a/b/c", {"a","b","c"}, &parse_path_abempty);

        bad("", &parse_path_absolute);
        bad("//", &parse_path_absolute);
        bad("///b", &parse_path_absolute);
        bad("a", &parse_path_absolute);
        bad("a/", &parse_path_absolute);
        bad("/%2", &parse_path_absolute);
        bad("/%%", &parse_path_absolute);
    }

    void
    test_parse_path_noscheme()
    {
    /*
        path-noscheme = segment-nz-nc *( "/" segment )
    */
        check("a", {"a"}, &parse_path_noscheme);
        check("a/", {"a",""}, &parse_path_noscheme);
        check("a/b", {"a","b"}, &parse_path_noscheme);
        check("%41/b", {"A","b"}, &parse_path_noscheme);
        check("%2f/b", {"/","b"}, &parse_path_noscheme);
        check("%2541//", {"%41","",""}, &parse_path_noscheme);
        check("http%3a//a.htm", {"http:","","a.htm"}, &parse_path_noscheme);
        check("a/b/c", {"a","b","c"}, &parse_path_noscheme);

        bad("", &parse_path_noscheme);
        bad("%2g/", &parse_path_noscheme);
        bad("/", &parse_path_noscheme);
        bad("/:", &parse_path_noscheme);
        bad(":", &parse_path_noscheme);
        bad("a:", &parse_path_noscheme);
        bad(":/", &parse_path_noscheme);
        bad("a:a", &parse_path_noscheme);
    }

    void
    test_parse_path_rootless()
    {
    /*
        path-rootless = segment-nz *( "/" segment )
    */
        check(":", {":"}, &parse_path_rootless);
        check("a:", {"a:"}, &parse_path_rootless);
        check(":/", {":",""}, &parse_path_rootless);
        check("a:a", {"a:a"}, &parse_path_rootless);
        check("a", {"a"}, &parse_path_rootless);
        check("a/", {"a",""}, &parse_path_rootless);
        check("a/b", {"a","b"}, &parse_path_rootless);
        check("%41/b", {"A","b"}, &parse_path_rootless);
        check("%2f/b", {"/","b"}, &parse_path_rootless);
        check("%2541//", {"%41","",""}, &parse_path_rootless);
        check("http%3a//a.htm", {"http:","","a.htm"}, &parse_path_rootless);
        check("a/b/c", {"a","b","c"}, &parse_path_rootless);

        bad("", &parse_path_rootless);
        bad("/", &parse_path_rootless);
        bad("/:", &parse_path_rootless);
    }

    void
    testCapacity()
    {
        segments_view sv;
        BOOST_TEST(sv.empty());
        BOOST_TEST(sv.size() == 0);
        sv = parse_path(
            "/path/to/file.txt").value().decoded();
        BOOST_TEST(! sv.empty());
        BOOST_TEST(sv.size() == 3);
        sv = {};
        BOOST_TEST(sv.empty());
        BOOST_TEST(sv.size() == 0);
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
            BOOST_TEST(ss.str() == "");
        }
        {
            // absolute
            std::stringstream ss;
            segments_view sv = parse_path(
                "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded();
            ss << sv;
            auto str = ss.str();
            BOOST_TEST(str == "/path/to/file.txt");
        }
        {
            // relative
            std::stringstream ss;
            segments_view sv = parse_path(
                "%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74"
                    ).value().decoded();
            ss << sv;
            BOOST_TEST(ss.str() == "path/to/file.txt");
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
        test_parse_path_abempty();
        test_parse_path_absolute();
        test_parse_path_noscheme();
        test_parse_path_rootless();

        testExamples();
    }
};

TEST_SUITE(
    segments_view_test,
    "boost.url.segments_view");

} // urls
} // boost

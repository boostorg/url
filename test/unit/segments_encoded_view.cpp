//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_encoded_view.hpp>

#include <boost/url/url_view.hpp>
#include "test_suite.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace boost {
namespace urls {

class segments_encoded_view_test
{
public:
#if __cpp_lib_ranges >= 201911
    /*
    BOOST_STATIC_ASSERT(
        std::bidirectional_range<
            segments_encoded_view>);
    */

    BOOST_STATIC_ASSERT(
        std::bidirectional_iterator<
            segments_encoded_view::iterator>);
#endif

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
        segments_encoded_view p;
        BOOST_TEST_THROWS(p = f(s).value(),
            std::exception);
        BOOST_TEST(p.empty());
        BOOST_TEST(p.begin() == p.end());
    }

    void
    check(
        string_view s,
        std::vector<
            string_view> const& v0,
        result<segments_encoded_view>(
            *f)(string_view))
    {
        segments_encoded_view sv;
        BOOST_TEST_NO_THROW(sv = f(s).value());
        // forward
        {
            std::vector<string_view> v1;
            std::copy(
                sv.begin(),
                sv.end(),
                std::back_inserter(v1));
            BOOST_TEST(v0 == v1);
        }
        // reverse
        {
            std::vector<string_view> v1;
            std::copy(
                reverse(sv.end()),
                reverse(sv.begin()),
                std::back_inserter(v1));
            std::reverse(v1.begin(), v1.end());
            BOOST_TEST(v0 == v1);
        }
    }

    //--------------------------------------------

    void
    testMembers()
    {
        // default constructor
        {
            segments_encoded_view sv;
            BOOST_TEST(sv.empty());
            BOOST_TEST(sv.size() == 0);
            BOOST_TEST(
                sv.begin() == sv.end());
        }

        // decoded
        {
            segments_encoded_view sev = parse_path(
                "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74").value();
            segments_view sv = sev.decoded(
                std::allocator<char>{});
            BOOST_TEST(sv.size() == 3);
            BOOST_TEST(sv.is_absolute());
        }

        // is_absolute
        {
            BOOST_TEST(parse_path(
                "/path/to/file.txt").value().is_absolute());
            BOOST_TEST(! parse_path(
                "./my/downloads").value().is_absolute());
        }
    }

    void
    testElementAccess()
    {
        // front
        // back
        {
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
            BOOST_TEST(sv.front() == "path");
            BOOST_TEST(sv.back() == "file.txt");
        }
    }

    void
    testIterators()
    {
        using iter_t =
            segments_encoded_view::iterator;

        // iterator()
        {
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
            iter_t it1;
            iter_t it2;
            BOOST_TEST(it1 == it2);
            BOOST_TEST(it1 != sv.begin());
            BOOST_TEST(it2 != sv.begin());
        }

        // iterator(iterator const&)
        {
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
            iter_t it1 = sv.begin();
            iter_t it2(it1);
            BOOST_TEST(it2 == it1);
            BOOST_TEST(*it1 == *it2);
            BOOST_TEST(*it1 == "path");
            BOOST_TEST(*it2 == "path");
        }

        // operator=(iterator const&)
        {
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
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
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
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
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
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
            segments_encoded_view sv = parse_path(
                "/path/to/file.txt").value();
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
        check("/%41/b", {"%41","b"}, &parse_path);
        check("///b", {"","","b"}, &parse_path);
        check("/%2f/b", {"%2f","b"}, &parse_path);
        check("/%2541//", {"%2541","",""}, &parse_path);
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
        check("/%41/b", {"%41","b"}, &parse_path_abempty);
        check("///b", {"","","b"}, &parse_path_abempty);
        check("/%2f/b", {"%2f","b"}, &parse_path_abempty);
        check("/%2541//", {"%2541","",""}, &parse_path_abempty);
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
        check("/%41/b", {"%41","b"}, &parse_path_absolute);
        check("/%2f/b", {"%2f","b"}, &parse_path_absolute);
        check("/%2541//", {"%2541","",""}, &parse_path_absolute);
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
        check("%41/b", {"%41","b"}, &parse_path_noscheme);
        check("%2f/b", {"%2f","b"}, &parse_path_noscheme);
        check("%2541//", {"%2541","",""}, &parse_path_noscheme);
        check("http%3a//a.htm", {"http%3a","","a.htm"}, &parse_path_noscheme);
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
        check("%41/b", {"%41","b"}, &parse_path_rootless);
        check("%2f/b", {"%2f","b"}, &parse_path_rootless);
        check("%2541//", {"%2541","",""}, &parse_path_rootless);
        check("http%3a//a.htm", {"http%3a","","a.htm"}, &parse_path_rootless);
        check("a/b/c", {"a","b","c"}, &parse_path_rootless);

        bad("", &parse_path_rootless);
        bad("/", &parse_path_rootless);
        bad("/:", &parse_path_rootless);
    }

    void
    testCapacity()
    {
        segments_encoded_view sv;
        BOOST_TEST(sv.empty());
        BOOST_TEST(sv.size() == 0);
        sv = parse_path("/path/to/file.txt").value();
        BOOST_TEST(! sv.empty());
        BOOST_TEST(sv.size() == 3);
        sv = {};
        BOOST_TEST(sv.empty());
        BOOST_TEST(sv.size() == 0);
    }

    void
    testOutput()
    {
        std::stringstream ss;
        segments_encoded_view sv = parse_path(
            "/path/to/file.txt").value();
        ss << sv;
        BOOST_TEST(ss.str() == "/path/to/file.txt");
    }

    void
    testExamples()
    {
        {
    segments_encoded_view sev = parse_path( "/path/to/file.txt" ).value();

    for( auto it = sev.begin(); it != sev.end(); ++it )
        std::cout << *it << std::endl;
        }

        {
    url_view u = parse_uri( "http://example.com/path/to/file.txt" ).value();

    segments_encoded_view sev = u.encoded_segments();

    std::cout << sev << std::endl;
        }
        //---

        {
        segments_encoded_view sev = parse_path( "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74" ).value();

        segments_view sv = sev.decoded();

        std::stringstream ss;

        ss << sv.front() << "/../" << sv.back();

        assert( ss.str() == "path/../file.txt" );
        }
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
    segments_encoded_view_test,
    "boost.url.segments_encoded_view");

} // urls
} // boost

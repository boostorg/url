//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/encoded_segments_view.hpp>

#include "test_suite.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace boost {
namespace urls {

class encoded_segments_view_test
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
        encoded_segments_view (*f)(string_view))
    {
        encoded_segments_view p;
        BOOST_TEST_THROWS(p = f(s),
            std::exception);
        BOOST_TEST(p.empty());
        BOOST_TEST(
            p.begin() == p.end());
    }

    void
    check(
        string_view s,
        std::vector<
            std::string> const& v0,
        encoded_segments_view (*f)(string_view))
    {
        encoded_segments_view p;
        BOOST_TEST_NO_THROW(p = f(s));
        // forward
        {
            std::vector<string_view> v1;
            std::copy(
                p.begin(),
                p.end(),
                std::back_inserter(v1));
            // VFALCO TODO
            //BOOST_TEST(v0 == v1);
        }
        // reverse
        {
            std::vector<string_view> v1;
            std::copy(
                reverse(p.end()),
                reverse(p.begin()),
                std::back_inserter(v1));
            std::reverse(v1.begin(), v1.end());
            // VFALCO TODO
            //BOOST_TEST(v0 == v1);
        }
    }

    void
    testIterator()
    {
        BOOST_TEST(
            encoded_segments_view::iterator() ==
            encoded_segments_view::iterator());
        encoded_segments_view p;
        BOOST_TEST(p.begin() !=
            encoded_segments_view::iterator());
        BOOST_TEST(p.end() !=
            encoded_segments_view::iterator());
        BOOST_TEST(
            p.begin() == p.end());

        auto it = p.begin();
        it = p.end();
        BOOST_TEST(it == p.begin());
    }

    void
    test_parse_path_abempty()
    {
        // path-abempty  = *( "/" segment )
        check("", {}, &parse_path_abempty);
        check("/", {""}, &parse_path_abempty);
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

        // path-abempty  = *( "/" segment )
        check("/%2541//", {"%41","",""}, parse_path);
        bad("a", parse_path);
    }

    void
    test_parse_path_absolute()
    {
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        check("/", {""}, &parse_path_absolute);
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
        // path-noscheme = segment-nz-nc *( "/" segment )
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
        // path-rootless = segment-nz *( "/" segment )
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
    run()
    {
        testIterator();

        test_parse_path_abempty();
        test_parse_path_absolute();
        test_parse_path_noscheme();
        test_parse_path_rootless();
    }
};

TEST_SUITE(
    encoded_segments_view_test,
    "boost.url.encoded_segments_view");

} // urls
} // boost

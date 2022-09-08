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

#include <boost/url/parse.hpp>
#include <boost/url/parse_path.hpp>
#include <boost/url/url.hpp>
#include <boost/static_assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

#include <sstream>

namespace boost {
namespace urls {

using Type = segments_view;

BOOST_STATIC_ASSERT(
    ! std::is_default_constructible<
        Type>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        Type>::value);

BOOST_STATIC_ASSERT(
    ! std::is_copy_assignable<
        Type>::value);

struct segments_view_test
{
    void
    check(
        string_view s,
        std::initializer_list<
            Type::reference> match)
    {
        auto rv = parse_uri_reference(s);
        if(! BOOST_TEST(rv.has_value()))
            return;
        Type const& ps(rv->segments());
        BOOST_TEST_EQ(ps.buffer().data(), s.data());
        BOOST_TEST_EQ(ps.is_absolute(), s.starts_with('/'));
        BOOST_TEST_EQ(ps.empty(), match.size() == 0);
        if(! BOOST_TEST_EQ(ps.size(), match.size()))
            return;
        if(match.size() > 0 && ! ps.empty())
        {
            BOOST_TEST_EQ(ps.front(), *match.begin());
            BOOST_TEST_EQ(ps.back(), *std::prev(match.end()));
        }
        // forward
        {
            auto it0 = ps.begin();
            auto it1 = match.begin();
            auto const end = ps.end();
            while(it0 != end)
            {
                Type::reference r0(*it0);
                Type::reference r1(*it1);
                BOOST_TEST_EQ(r0, r1);
                BOOST_TEST_EQ(*it0, *it1);
                BOOST_TEST_EQ( // arrow
                    it0->size(), it1->size());
                Type::value_type v0(*it0);
                Type::value_type v1(*it1);
                BOOST_TEST_EQ(v0, *it1);
                BOOST_TEST_EQ(v1, *it1);
                auto prev = it0++;
                BOOST_TEST_NE(prev, it0);
                BOOST_TEST_EQ(++prev, it0);
                ++it1;
                BOOST_TEST_EQ(v0, v1);;
            }
        }
        // reverse
        if(match.size() > 0)
        {
            auto const begin = ps.begin();
            auto it0 = ps.end();
            auto it1 = match.end();
            do
            {
                auto prev = it0--;
                BOOST_TEST_NE(prev, it0);
                BOOST_TEST_EQ(--prev, it0);
                --it1;
                Type::reference r0(*it0);
                Type::reference r1(*it1);
                BOOST_TEST_EQ(*it0, *it1);
                BOOST_TEST_EQ(r0, r1);
            }
            while(it0 != begin);
        }
        // ostream
        {
            std::stringstream ss;
            ss << ps;
            BOOST_TEST_EQ(ss.str(),
                rv->encoded_path());
        }
    }

    void
    testMembers()
    {
        // segments_view(
        //      segments_view const&)
        {
            segments_view ps0 =
                parse_path("/path/to/file.txt").value();
            segments_view ps1(ps0);
            BOOST_TEST_EQ(
                ps0.buffer().data(), ps1.buffer().data());
        }

        // ostream
        {
            segments_view ps = parse_path(
                "/path/to/file.txt").value();
            std::stringstream ss;
            ss << ps;
            BOOST_TEST_EQ(ss.str(),
                "/path/to/file.txt");
        }
    }

    void
    testRange()
    {
    /*  Legend

        '#' %23     '?' %3F
        '.' %2E     '[' %5B
        '/' %2F     ']' %5D
    */
        check( "", {});
        check( "./", { "" });
        check( ".//", { "", "" });
        check( "/", {});
        check( "/./", { "" });
        check( "/.//", { "", "" });
        check( "/%3F", {"?"});
        check( "%2E/", {".", ""});
        check( "./usr", { "usr" });
        check( "/index.htm", { "index.htm" });
        check( "/images/cat-pic.gif", { "images", "cat-pic.gif" });
        check( "images/cat-pic.gif", { "images", "cat-pic.gif" });
        check( "/fast//query", { "fast", "", "query" });
        check( "fast//",  { "fast", "", "" });
    }

    void
    testJavadocs()
    {
        // {class}
        {
    url_view u( "/path/to/file.txt" );

    segments_view ps = u.segments();

    assert( ps.buffer().data() == u.string().data() );

    ignore_unused(ps);
        }
    }

    void
    run()
    {
        testMembers();
        testRange();
        testJavadocs();
    }
};

TEST_SUITE(
    segments_view_test,
    "boost.url.segments_view");

} // urls
} // boost

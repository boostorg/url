//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_base.hpp>

#include <boost/url/parse_path.hpp>
#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct segments_base_test
{
    static
    void
    check(
        string_view s,
        std::initializer_list<
            string_view> init)
    {
        auto rv = parse_path(s);
        if(! BOOST_TEST(rv.has_value()))
            return;
        segments_base const& ps(*rv);
        BOOST_TEST_EQ(
            ps.buffer().data(), s.data());
        BOOST_TEST_EQ(
            ps.is_absolute(),
            s.starts_with('/'));
        BOOST_TEST_EQ(ps.empty(), init.size() == 0);
        if(! BOOST_TEST_EQ(ps.size(), init.size()))
            return;
        if(init.size() > 0 && ! ps.empty())
        {
            BOOST_TEST_EQ(ps.front(), *init.begin());
            BOOST_TEST_EQ(ps.back(), *std::prev(init.end()));
        }

        // forward
        {
            auto it0 = ps.begin();
            auto it1 = init.begin();
            auto const end = ps.end();
            while(it0 != end)
            {
                segments_base::reference r0(*it0);
                segments_base::reference r1(*it1);
                BOOST_TEST_EQ(r0, r1);
                BOOST_TEST_EQ(*it0, *it1);
                segments_base::value_type v0(*it0);
                segments_base::value_type v1(*it1);
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
        if(init.size() > 0)
        {
            auto const begin = ps.begin();
            auto it0 = ps.end();
            auto it1 = init.end();
            do
            {
                auto prev = it0--;
                BOOST_TEST_NE(prev, it0);
                BOOST_TEST_EQ(--prev, it0);
                --it1;
                segments_base::reference r0(*it0);
                segments_base::reference r1(*it1);
                BOOST_TEST_EQ(*it0, *it1);
                BOOST_TEST_EQ(r0, r1);
            }
            while(it0 != begin);
        }
    }

    void
    testMembers()
    {
    /*  Legend

        '.' 0x2e
        '/' 0x2f
    */
        check( "", { });
        check( "/", { });
        check( "./", { "" });
        check( "./usr", { "usr" });
        check("/index%2ehtm", { "index.htm" });
        check("/images/cat-pic.gif", { "images", "cat-pic.gif" });
        check("images/cat-pic.gif", { "images", "cat-pic.gif" });
        check("/fast//query", { "fast", "", "query" });
        check("fast//",  { "fast", "", "" });
        check("/./", { "" });
        check(".//", { "", "" });
    }

    void
    testJavadoc()
    {
        // value_type
        {
        segments_view::value_type ps( url_view( "/path/to/file.txt" ).segments().back() );

        ignore_unused(ps);
        }

        // buffer()
        {
        assert( url_view( "/path/to/file.txt" ).segments().buffer() == "/path/to/file.txt" );
        }

        // is_absolute()
        {
        assert( url_view( "/path/to/file.txt" ).segments().is_absolute() == true );
        }

        // empty()
        {
        assert( ! url_view( "/index.htm" ).segments().empty() );
        }

        // size()
        {
        assert( url_view( "/path/to/file.txt" ).segments().size() == 3 );
        }

        // front()
        {
        assert( url_view( "/path/to/file.txt" ).segments().front() == "path" );
        }

        // back()
        {
        assert( url_view( "/path/to/file.txt" ).segments().back() == "file.txt" );
        }
    }

    void
    run()
    {
        testMembers();
        testJavadoc();
    }
};

TEST_SUITE(
    segments_base_test,
    "boost.url.segments_base");

} // urls
} // boost

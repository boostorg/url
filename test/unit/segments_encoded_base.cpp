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
#include <boost/url/segments_encoded_base.hpp>

#include <boost/url/parse_path.hpp>
#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

#include <iterator>

namespace boost {
namespace urls {

struct segments_encoded_base_test
{
    using Type = segments_encoded_base;
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
        Type const& ps(*rv);
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
            BOOST_TEST_EQ(
                ps.front(), *init.begin());
            BOOST_TEST_EQ(
                ps.back(), *std::prev(init.end()));
        }

        // ostream
        {
            std::stringstream ss;
            ss << ps;
            BOOST_TEST_EQ(ss.str(), s);
        }

        // forward
        {
            auto it0 = ps.begin();
            auto it1 = init.begin();
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
                Type::reference r0(*it0);
                Type::reference r1(*it1);
                BOOST_TEST_EQ(*it0, *it1);
                BOOST_TEST_EQ(r0, r1);
            }
            while(it0 != begin);
        }
    }

    void
    testSequence()
    {
        check( "", { });
        check( "/", { });
        check( "./", { "" });
        check( "./usr", { "usr" });
        check("/index.htm", { "index.htm" });
        check("/images/cat-pic.gif", { "images", "cat-pic.gif" });
        check("images/cat-pic.gif", { "images", "cat-pic.gif" });
        check("/fast//query", { "fast", "", "query" });
        check("fast//",  { "fast", "", "" });
        check("/./", { "" });
        check(".//", { "", "" });
    }

    void
    testJavadocs()
    {
        // value_type
        {
        segments_encoded_base::value_type ps( *url_view( "/path/to/file.txt" ).encoded_segments().back() );

        ignore_unused(ps);
        }

        // buffer()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().buffer() == "/path/to/file.txt" );
        }

        // is_absolute()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().is_absolute() == true );
        }

        // empty()
        {
        assert( ! url_view( "/index.htm" ).encoded_segments().empty() );
        }

        // size()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().size() == 3 );
        }

        // front()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().front() == "path" );
        }

        // back()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().back() == "file.txt" );
        }
    }

    void
    run()
    {
        testSequence();
        testJavadocs();
    }
};

TEST_SUITE(
    segments_encoded_base_test,
    "boost.url.segments_encoded_base");

} // urls
} // boost

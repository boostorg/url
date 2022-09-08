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
#include <boost/url/segments_encoded_ref.hpp>

#include <boost/url/url.hpp>
#include <boost/static_assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

using Type = segments_encoded_ref;

BOOST_STATIC_ASSERT(
    ! std::is_default_constructible<
        Type>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        Type>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_assignable<
        Type>::value);

BOOST_STATIC_ASSERT(
    std::is_default_constructible<
        Type::iterator>::value);

struct segments_encoded_ref_test
{
    // check that parsed string
    // produces a sequence
    static
    void
    check(
        string_view s,
        std::initializer_list<
            string_view> init)
    {
        auto rv = parse_uri_reference(s);
        if(! BOOST_TEST(rv.has_value()))
            return;
        url u = *rv;
        Type ps(u.encoded_segments());
        BOOST_TEST_EQ(
            ps.is_absolute(), s.starts_with('/'));
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
                segments_encoded_base::reference r0(*it0);
                segments_encoded_base::reference r1(*it1);
                BOOST_TEST_EQ(r0, r1);
                BOOST_TEST_EQ(*it0, *it1);
                segments_encoded_base::value_type v0(*it0);
                segments_encoded_base::value_type v1(*it1);
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
                segments_encoded_base::reference r0(*it0);
                segments_encoded_base::reference r1(*it1);
                BOOST_TEST_EQ(*it0, *it1);
                BOOST_TEST_EQ(r0, r1);
            }
            while(it0 != begin);
        }
    }

    // check that modification produces
    // the string and correct sequence
    static
    void
    check(
        void(*f)(Type),
        string_view s0,
        string_view s1,
        std::initializer_list<
            string_view> init)
    {
        auto rv = parse_uri_reference(s0);
        if(! BOOST_TEST(rv.has_value()))
            return;
        url u = *rv;
        Type ps(u.encoded_segments());
        f(ps);
        BOOST_TEST_EQ(u.encoded_path(), s1);
        if(! BOOST_TEST_EQ(
                ps.size(), init.size()))
            return;
        auto it0 = ps.begin();
        auto it1 = init.begin();
        auto const end = ps.end();
        while(it0 != end)
        {
            BOOST_TEST_EQ(*it0, *it1);
            ++it0;
            ++it1;
        }
    }

    static
    void
    check(
        void(*f1)(Type), void(*f2)(Type),
        string_view s0, string_view s1,
        std::initializer_list<
            string_view> init)
    {
        check(f1, s0, s1, init);
        check(f2, s0, s1, init);
    }

    //--------------------------------------------

    void
    testSpecial()
    {
        // segments_encoded_ref(
        //      segments_encoded_ref const&)
        {
            url u("/index.htm");
            Type ps0 = u.encoded_segments();
            Type ps1(ps0);
            BOOST_TEST_EQ(&ps0.url(), &ps1.url());
            BOOST_TEST_EQ(
                ps0.url().string().data(),
                ps1.url().string().data());
        }

        // operator=(segments_encoded_ref)
        {
            url u1("/index.htm");
            url u2("/path/to/file.txt");
            Type ps1 =
                u1.encoded_segments();
            Type ps2 =
                u2.encoded_segments();
            BOOST_TEST_NE(
                ps1.buffer().data(),
                ps2.buffer().data());
            ps1 = ps2;
            BOOST_TEST_EQ(
                u1.encoded_path(),
                u2.encoded_path());
            BOOST_TEST_NE(
                ps1.buffer().data(),
                ps2.buffer().data());
        }

        // operator=(segments_encoded_view)
        {
            url u1("/index.htm");
            url_view u2("/path/to/file.txt");
            Type ps1 =
                u1.encoded_segments();
            segments_encoded_view ps2 =
                u2.encoded_segments();
            BOOST_TEST_NE(
                ps1.buffer().data(),
                ps2.buffer().data());
            ps1 = ps2;
            BOOST_TEST_EQ(
                u1.encoded_path(),
                u2.encoded_path());
            BOOST_TEST_NE(
                ps1.buffer().data(),
                ps2.buffer().data());
        }

        // operator=(initializer_list)
        {
        }

        // operator segments_encoded_view()
        {
        }
    }

    void
    testObservers()
    {
        // url()
        {
            url u0( "/" );
            url u1( "/" );
            BOOST_TEST_EQ(
                &u0.encoded_segments().url(), &u0);
            BOOST_TEST_EQ(
                &u1.encoded_segments().url(), &u1);
            BOOST_TEST_NE(
                &u0.encoded_segments().url(),
                &u1.encoded_segments().url());
        }
    }

    void
    testModifiers()
    {
        //
        // clear()
        //

        {
            auto const f = [](Type ps)
            {
                ps.clear();
            };
            check(f, "", "", {} );
            check(f, "/", "/", {});
            check(f, "/index.htm", "/", {});
            check(f, "index.htm", "", {});
            check(f, "/path/to/file.txt", "/", {});
            check(f, "Program%20Files", "", {});
            check(f, "x://y/", "/", {});
        }

        //
        // assign(initializer_list)
        // assign(FwdIt, FwdIt)
        //

        {
            auto const f = [](Type ps)
            {
                ps.assign({ "path", "to", "file.txt" });
            };
            auto const g = [](Type ps)
            {
                auto const assign = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.assign(init.begin(), init.end());
                };
                assign({ "path", "to", "file.txt" });
            };
            check(f, g, "", "path/to/file.txt", {"path", "to", "file.txt"});
            check(f, g, "/", "/path/to/file.txt", {"path", "to", "file.txt"});
            check(f, g, "/index.htm", "/path/to/file.txt", {"path", "to", "file.txt"});
            check(f, g, "index.htm", "path/to/file.txt", {"path", "to", "file.txt"});
            check(f, g, "/path/to/file.txt", "/path/to/file.txt", {"path", "to", "file.txt"});
            check(f, g, "Program%20Files", "path/to/file.txt", {"path", "to", "file.txt"});
        }

        //
        // insert(iterator, pct_string_view)
        //

        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.begin(), "");
            };
            check(f, "", "./", {""});
            check(f, "/", "/./", {""});
            check(f, "/index.htm", "/.//index.htm", {"", "index.htm"});
            check(f, "index.htm", ".//index.htm", {"", "index.htm"});
            check(f, "path/to/file.txt", ".//path/to/file.txt", {"", "path", "to", "file.txt"});
            check(f, "/path/to/file.txt", "/.//path/to/file.txt", {"", "path", "to", "file.txt"});
            check(f, "Program%20Files", ".//Program%20Files", {"", "Program%20Files"});
            check(f, "x:", "./", {""});
        }
        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.begin(), "my%20seg");
            };
            check(f, "", "my%20seg", {"my%20seg"});
            check(f, "/", "/my%20seg", {"my%20seg"});
            check(f, "/index.htm", "/my%20seg/index.htm", {"my%20seg", "index.htm"});
            check(f, "index.htm", "my%20seg/index.htm", {"my%20seg", "index.htm"});
            check(f, "path/to/file.txt", "my%20seg/path/to/file.txt", {"my%20seg", "path", "to", "file.txt"});
            check(f, "/path/to/file.txt", "/my%20seg/path/to/file.txt", {"my%20seg", "path", "to", "file.txt"});
            check(f, "Program%20Files", "my%20seg/Program%20Files", {"my%20seg", "Program%20Files"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.insert(std::next(ps.begin(), 1), "my%20seg");
            };
            check(f, "path/to/file.txt", "path/my%20seg/to/file.txt", {"path", "my%20seg", "to", "file.txt"});
            check(f, "/path/to/file.txt", "/path/my%20seg/to/file.txt", {"path", "my%20seg", "to", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.end(), "my%20seg");
            };
            check(f, "", "my%20seg", {"my%20seg"});
            check(f, "/", "/my%20seg", {"my%20seg"});
            check(f, "/index.htm", "/index.htm/my%20seg", {"index.htm", "my%20seg"});
            check(f, "index.htm", "index.htm/my%20seg", {"index.htm", "my%20seg"});
            check(f, "path/to/file.txt", "path/to/file.txt/my%20seg", {"path", "to", "file.txt", "my%20seg"});
            check(f, "/path/to/file.txt", "/path/to/file.txt/my%20seg", {"path", "to", "file.txt", "my%20seg"});
            check(f, "Program%20Files", "Program%20Files/my%20seg", {"Program%20Files", "my%20seg"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.end(), "");
            };
            check(f, "", "./", {""});
            check(f, "/", "/./", {""});
            check(f, "/index.htm", "/index.htm/", {"index.htm", ""});
            check(f, "index.htm", "index.htm/", {"index.htm", ""});
            check(f, "path/to/file.txt", "path/to/file.txt/", {"path", "to", "file.txt", ""});
            check(f, "/path/to/file.txt", "/path/to/file.txt/", {"path", "to", "file.txt", ""});
        }

        //
        // insert(iterator, initializer_list)
        // insert(iterator, FwdIt, FwdIt)
        //

        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.begin(), { "u", "v" });
            };
            auto const g = [](Type ps)
            {
                auto const insert = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.insert(ps.begin(),
                        init.begin(), init.end());
                };
                insert({ "u", "v" });
            };
            check(f, g, "", "u/v", {"u", "v"});
            check(f, g, "/", "/u/v", {"u", "v"});
            check(f, g, "/index.htm", "/u/v/index.htm", {"u", "v", "index.htm"});
            check(f, g, "index.htm", "u/v/index.htm", {"u", "v", "index.htm"});
            check(f, g, "path/to/file.txt", "u/v/path/to/file.txt", {"u", "v", "path", "to", "file.txt"});
            check(f, g, "/path/to/file.txt", "/u/v/path/to/file.txt", {"u", "v", "path", "to", "file.txt"});
            check(f, g, "Program%20Files", "u/v/Program%20Files", {"u", "v", "Program%20Files"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.insert(ps.begin(), { "", "" });
            };
            auto const g = [](Type ps)
            {
                auto const insert = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.insert(ps.begin(),
                        init.begin(), init.end());
                };
                insert({ "", "" });
            };
            check(f, g, "", ".//", {"", ""});
            check(f, g, "/", "/.//", {"", ""});
            check(f, g, "/index.htm", "/.///index.htm", {"", "", "index.htm"});
            check(f, g, "index.htm", ".///index.htm", {"", "", "index.htm"});
            check(f, g, "path/to/file.txt", ".///path/to/file.txt", {"", "", "path", "to", "file.txt"});
            check(f, g, "/path/to/file.txt", "/.///path/to/file.txt", {"", "", "path", "to", "file.txt"});
            check(f, g, "x", ".///x", {"", "", "x"});
        }

        //
        // erase(iterator)
        //

        {
            auto const f = [](Type ps)
            {
                ps.erase(std::next(ps.begin(), 0));
            };
            check(f, "path/to/file.txt", "to/file.txt", {"to", "file.txt"});
            check(f, "/path/to/file.txt", "/to/file.txt", {"to", "file.txt"});
            check(f, "//x/y/", "/./", {""});
            check(f, "/x/", "/./", {""});
            check(f, "x/", "./", {""});
            check(f, "x:.//", "./", {""});
        }
        {
            auto const f = [](Type ps)
            {
                ps.erase(std::next(ps.begin(), 1));
            };
            check(f, "path/to/file.txt", "path/file.txt", {"path", "file.txt"});
            check(f, "/path/to/file.txt", "/path/file.txt", {"path", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.erase(std::next(ps.begin(), 2));
            };
            check(f, "path/to/file.txt", "path/to", {"path", "to"});
            check(f, "/path/to/file.txt", "/path/to", {"path", "to"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.erase(std::next(ps.begin(), 1));
            };
            check(f, "x://y///", "//", {"", ""});
            check(f, ".///", ".//", {"", ""});
        }

        //
        // erase(iterator, iterator
        //

        {
            auto const f = [](Type ps)
            {
                ps.erase(
                    std::next(ps.begin(), 0),
                    std::next(ps.begin(), 2));
            };
            check(f, "path/to/the/file.txt", "the/file.txt", {"the", "file.txt"});
            check(f, "/path/to/the/file.txt", "/the/file.txt", {"the", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.erase(
                    std::next(ps.begin(), 1),
                    std::next(ps.begin(), 3));
            };
            check(f, "path/to/the/file.txt", "path/file.txt", {"path", "file.txt"});
            check(f, "/path/to/the/file.txt", "/path/file.txt", {"path", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.erase(
                    std::next(ps.begin(), 2),
                    std::next(ps.begin(), 4));
            };
            check(f, "path/to/the/file.txt", "path/to", {"path", "to"});
            check(f, "/path/to/the/file.txt", "/path/to", {"path", "to"});
        }

        //
        // replace(iterator, pct_string_view)
        //

        {
            auto const f = [](Type ps)
            {
                ps.replace(std::next(ps.begin(), 0), "");
            };
            check(f, "path/to/file.txt", ".//to/file.txt", {"", "to", "file.txt"});
            check(f, "/path/to/file.txt", "/.//to/file.txt", {"", "to", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(std::next(ps.begin(), 1), "");
            };
            check(f, "path/to/file.txt", "path//file.txt", {"path", "", "file.txt"});
            check(f, "/path/to/file.txt", "/path//file.txt", {"path", "", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(std::next(ps.begin(), 0), "test");
            };
            check(f, "path/to/file.txt", "test/to/file.txt", {"test", "to", "file.txt"});
            check(f, "/path/to/file.txt", "/test/to/file.txt", {"test", "to", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(std::next(ps.begin(), 1), "test");
            };
            check(f, "path/to/file.txt", "path/test/file.txt", {"path", "test", "file.txt"});
            check(f, "/path/to/file.txt", "/path/test/file.txt", {"path", "test", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(std::next(ps.begin(), 2), "test");
            };
            check(f, "path/to/file.txt", "path/to/test", {"path", "to", "test"});
            check(f, "/path/to/file.txt", "/path/to/test", {"path", "to", "test"});
        }

        //
        // replace(iterator, pct_string_view)
        //

        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 0),
                    std::next(ps.begin(), 2),
                    "");
            };
            check(f, "path/to/the/file.txt", ".//the/file.txt", {"", "the", "file.txt"});
            check(f, "/path/to/the/file.txt", "/.//the/file.txt", {"", "the", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 1),
                    std::next(ps.begin(), 3),
                    "");
            };
            check(f, "path/to/the/file.txt", "path//file.txt", {"path", "", "file.txt"});
            check(f, "/path/to/the/file.txt", "/path//file.txt", {"path", "", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 2),
                    std::next(ps.begin(), 4),
                    "");
            };
            check(f, "path/to/the/file.txt", "path/to/", {"path", "to", ""});
            check(f, "/path/to/the/file.txt", "/path/to/", {"path", "to", ""});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 0),
                    std::next(ps.begin(), 2),
                    "test");
            };
            check(f, "path/to/the/file.txt", "test/the/file.txt", {"test", "the", "file.txt"});
            check(f, "/path/to/the/file.txt", "/test/the/file.txt", {"test", "the", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 1),
                    std::next(ps.begin(), 3),
                    "test");
            };
            check(f, "path/to/the/file.txt", "path/test/file.txt", {"path", "test", "file.txt"});
            check(f, "/path/to/the/file.txt", "/path/test/file.txt", {"path", "test", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 2),
                    std::next(ps.begin(), 4),
                    "test");
            };
            check(f, "path/to/the/file.txt", "path/to/test", {"path", "to", "test"});
            check(f, "/path/to/the/file.txt", "/path/to/test", {"path", "to", "test"});
        }

        //
        // replace(iterator, iterator. initializer_list)
        // replace(iterator, iterator. FwdIt, FwdIt)
        //

        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 0),
                    std::next(ps.begin(), 2),
                    { "t", "u", "v" });
            };
            auto const g = [](Type ps)
            {
                auto const replace = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.replace(
                        std::next(ps.begin(), 0),
                        std::next(ps.begin(), 2),
                        init.begin(), init.end());
                };
                replace({ "t", "u", "v" });
            };
            check(f, g, "path/to/the/file.txt", "t/u/v/the/file.txt", {"t", "u", "v", "the", "file.txt"});
            check(f, g, "/path/to/the/file.txt", "/t/u/v/the/file.txt", {"t", "u", "v", "the", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 1),
                    std::next(ps.begin(), 3),
                    { "t", "u", "v" });
            };
            auto const g = [](Type ps)
            {
                auto const replace = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.replace(
                        std::next(ps.begin(), 1),
                        std::next(ps.begin(), 3),
                        init.begin(), init.end());
                };
                replace({ "t", "u", "v" });
            };
            check(f, g, "path/to/the/file.txt", "path/t/u/v/file.txt", {"path", "t", "u", "v", "file.txt"});
            check(f, g, "/path/to/the/file.txt", "/path/t/u/v/file.txt", {"path", "t", "u", "v", "file.txt"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.replace(
                    std::next(ps.begin(), 2),
                    std::next(ps.begin(), 4),
                    { "t", "u", "v" });
            };
            auto const g = [](Type ps)
            {
                auto const replace = [&ps](
                    std::initializer_list<
                        pct_string_view> init)
                {
                    ps.replace(
                        std::next(ps.begin(), 2),
                        std::next(ps.begin(), 4),
                        init.begin(), init.end());
                };
                replace({ "t", "u", "v" });
            };
            check(f, g, "path/to/the/file.txt", "path/to/t/u/v", {"path", "to", "t", "u", "v"});
            check(f, g, "/path/to/the/file.txt", "/path/to/t/u/v", {"path", "to", "t", "u", "v"});
        }
    }

    void
    testEditSegments()
    {
    /*  Legend

        '#' 0x23    '/' 0x2f
        '%' 0x25    ':' 0x3a
        '.' 0x2e    '?' 0x3f
    */
        {
            auto const f = [](Type ps)
            {
                ps.push_back("");
            };
            check(f, "",    "./",   {""});
            check(f, "/",   "/./",  {""});
            check(f, "./",  ".//",  {"", ""});
            check(f, "/./", "/.//", {"", ""});
        }
        {
            auto const f = [](Type ps)
            {
                ps.push_back("/");
            };
            check(f, "",  "%2F",  {"%2F"});
            check(f, "/", "/%2F", {"%2F"});
        }
        {
            auto const f = [](Type ps)
            {
                ps.push_back(":");
            };
            check(f, "",  "./:",  {":"});
            check(f, "/", "/:", {":"});
        }
    }

    void
    testRange()
    {
        check( "", {});
        check( "/", {});
        check( "./", { "" });
        check( "./usr", { "usr" });
        check( "/index.htm", { "index.htm" });
        check( "/images/cat-pic.gif", { "images", "cat-pic.gif" });
        check( "images/cat-pic.gif", { "images", "cat-pic.gif" });
        check( "/fast//query", { "fast", "", "query" });
        check( "fast//",  { "fast", "", "" });
        check( "/./", { "" });
        check( ".//", { "", "" });
    }

    void
    testJavadocs()
    {
        // {class}
        {
    url u( "/path/to/file.txt" );

    segments_encoded_ref ps = u.encoded_segments();

    ignore_unused(ps);
        }

        // operator=(initializer_list)
        {
        url u;

        u.encoded_segments() = {"path", "to", "file.txt"};
        }

        // url()
        {
        url u( "?key=value" );

        assert( &u.encoded_segments().url() == &u );
        }
    }

    void
    run()
    {
        testSpecial();
        testObservers();
        testModifiers();
        testEditSegments();
        testRange();
        testJavadocs();
    }
};

TEST_SUITE(
    segments_encoded_ref_test,
    "boost.url.segments_encoded_ref");

} // urls
} // boost

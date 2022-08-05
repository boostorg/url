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
#include <boost/url/segments_encoded.hpp>

#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/static_assert.hpp>
#include <initializer_list>
#include <iterator>
#include "test_suite.hpp"

namespace boost {
namespace urls {

//------------------------------------------------

class segments_encoded_test
{
public:
    BOOST_STATIC_ASSERT(
        std::is_default_constructible<
            segments_encoded::iterator>::value);

    void
    testMembers()
    {
        // operator=(segments const&)
        {
            url u1;
            url u2;
            segments_encoded p1 = u1.encoded_segments();
            segments_encoded p2 = u2.encoded_segments();
            p2 = p1;
            BOOST_TEST_EQ(p1.begin(), p2.begin());
        }

        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt?q#f").value();

        {
            url u = u0;
            u.encoded_segments() = { "etc", "index.htm" };
            BOOST_TEST_EQ(u.encoded_path(), "/etc/index.htm");
            BOOST_TEST_EQ(u.string(), "x://y/etc/index.htm?q#f");
        }
    }

    void
    testElementAccess()
    {
        url_view const u0 = parse_relative_ref(
            "/path/to/the/file.txt").value();

        // at
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(*se.begin(), "path");
            BOOST_TEST_EQ(*std::next(se.begin()), "to");
            BOOST_TEST_EQ(*std::next(se.begin(), 2), "the");
            BOOST_TEST_EQ(*std::next(se.begin(), 3), "file.txt");

            // assign
            se.replace(std::next(se.begin()), "from");
            // comparison
            BOOST_TEST_EQ(*std::next(se.begin()), "from");
            BOOST_TEST_NE(*std::next(se.begin()), "path");
        }

        // operator[]
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(*se.begin(), "path");
            BOOST_TEST_EQ(*std::next(se.begin()), "to");
            BOOST_TEST_EQ(*std::next(se.begin(), 2), "the");
            BOOST_TEST_EQ(*std::next(se.begin(), 3), "file.txt");

            // assign
            se.replace(std::next(se.begin()), "from");
            // comparison
            BOOST_TEST_EQ(*std::next(se.begin()), "from");
            BOOST_TEST_NE(*std::next(se.begin()), "path");
        }

        // front
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(se.front(), "path");

            // assign
            se.replace(se.begin(), "etc");
            // comparison
            BOOST_TEST_EQ(se.front(), "etc");
            BOOST_TEST_NE(se.front(), "path");
        }

        // back
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(se.back(), "file.txt");

            // assign
            se.replace(std::prev(se.end()), "index.htm");
            // comparison
            BOOST_TEST_EQ(se.back(), "index.htm");
            BOOST_TEST_NE(se.back(), "file.txt");
        }
    }

    void
    testIterators()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt").value();

        // (default-ctor)
        {
            segments_encoded::iterator it;
            (void)it;
        }

        // begin
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_NE(se.begin(), se.end());
        }

        // end
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_NE(se.end(), se.begin());
        }

        //
        // iterator
        //

        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            auto const& cs(se);

            segments_encoded::iterator it = se.begin();
            BOOST_TEST_EQ(*it, "path");
            BOOST_TEST_EQ(*++it, "to");
            BOOST_TEST_EQ(*it++, "to");
            BOOST_TEST_EQ(*it--, "the");
            BOOST_TEST_EQ(*it, "to");
            BOOST_TEST_EQ(*--it, "path");
            BOOST_TEST_EQ(it, se.begin());
            BOOST_TEST_NE(it, se.end());

            BOOST_TEST_EQ(*(++it), "to");
            BOOST_TEST_EQ(*std::next(it), "the");
            BOOST_TEST_EQ(*std::next(it), "the");
            BOOST_TEST_EQ(*(--it), "path");
            std::advance(it, 2);
            BOOST_TEST_EQ(*std::prev(it), "to");
            --it;

            BOOST_TEST_NE(it, se.begin());
            BOOST_TEST_NE(it, cs.begin());
        }

        // value_type outlives reference
        {
            segments_encoded::value_type v;
            {
                url u = u0;
                segments_encoded se = u.encoded_segments();
                segments_encoded::reference r =
                    *se.begin();
                v = segments_encoded::value_type(r);
            }
            BOOST_TEST_EQ(v, "path");
        }
    }

    void
    testCapacity()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt").value();

        // empty
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            BOOST_TEST(! se.empty());
        }

        // size
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(se.size(), 4u);
        }
    }

    void
    testModifiers()
    {
        // clear
        {
            url u = parse_uri("x://y/path/to/the/file.txt").value();
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(! se.empty());
            BOOST_TEST_EQ(se.size(), 4u);
            se.clear();
            BOOST_TEST(se.empty());
            BOOST_TEST_EQ(se.size(), 0u);
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.string(), "x://y/");
        }

        // insert( const_iterator, string_view )
        {
            url u = parse_uri("x://y/path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            BOOST_TEST_EQ(se.size(), 2u);
            segments_encoded::iterator it =
                se.insert(std::next(se.begin()), "to");
            BOOST_TEST_EQ(se.size(), 3u);
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/path/to/file.txt?q#f");
            BOOST_TEST_EQ(*it, "to");

            it = se.insert(cs.end(), "");
            BOOST_TEST_EQ(se.size(), 4u);
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/file.txt/");
            BOOST_TEST_EQ(u.string(), "x://y/path/to/file.txt/?q#f");
            BOOST_TEST_EQ(*it, "");

            it = se.insert(se.begin(), "etc");
            BOOST_TEST_EQ(se.size(), 5u);
            BOOST_TEST_EQ(u.encoded_path(), "/etc/path/to/file.txt/");
            BOOST_TEST_EQ(u.string(), "x://y/etc/path/to/file.txt/?q#f");
            BOOST_TEST_EQ(*it, "etc");

            BOOST_TEST_THROWS(se.insert(se.begin(), "%"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "/"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "%2g"), std::invalid_argument);
        }

        {
            // rootless
            url u = parse_uri("x:path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            BOOST_TEST_EQ(se.size(), 2u);
            segments_encoded::iterator it =
                se.insert(std::next(se.begin()), "to");
            BOOST_TEST_EQ(se.size(), 3u);
            BOOST_TEST_EQ(u.encoded_path(), "path/to/file.txt");
            BOOST_TEST_EQ(u.string(), "x:path/to/file.txt?q#f");
            BOOST_TEST_EQ(*it, "to");

            it = se.insert(cs.end(), "");
            BOOST_TEST_EQ(se.size(), 4u);
            BOOST_TEST_EQ(u.encoded_path(), "path/to/file.txt/");
            BOOST_TEST_EQ(u.string(), "x:path/to/file.txt/?q#f");
            BOOST_TEST_EQ(*it, "");

            it = se.insert(se.begin(), "etc");
            BOOST_TEST_EQ(se.size(), 5u);
            BOOST_TEST_EQ(u.encoded_path(), "etc/path/to/file.txt/");
            BOOST_TEST_EQ(u.string(), "x:etc/path/to/file.txt/?q#f");
            BOOST_TEST_EQ(*it, "etc");

            BOOST_TEST_THROWS(se.insert(se.begin(), "%"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "/"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "%2g"), std::invalid_argument);
        }

        // insert( const_iterator, FwdIt, FwdIt )
        {
            url u = parse_uri("x://y/path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            std::initializer_list<string_view> init = {"to", "the" };
            auto it = se.insert(
                std::next(se.begin()), init.begin(), init.end());
            BOOST_TEST_EQ(cs.size(), 4u);
            BOOST_TEST_EQ(*it, "to");
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/path/to/the/file.txt?q#f");

            std::initializer_list<string_view> bad = {"%"};
            BOOST_TEST_THROWS(se.insert(
                std::next(se.begin()), bad.begin(), bad.end()),
                std::invalid_argument);

            // empty range
            it = se.insert(std::next(se.begin()),
                init.begin(), init.begin());
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/the/file.txt");
            BOOST_TEST_EQ(it, std::next(se.begin()));
        }
        {
            // rootless
            url u = parse_uri("x:the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            std::initializer_list<string_view> init = {"path", "to" };
            auto it = se.insert(
                se.begin(), init.begin(), init.end());
            BOOST_TEST_EQ(cs.size(), 4u);
            BOOST_TEST_EQ(*it, "path");
            BOOST_TEST_EQ(u.encoded_path(), "path/to/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x:path/to/the/file.txt?q#f");

            std::initializer_list<string_view> bad = {"%"};
            BOOST_TEST_THROWS(se.insert(
                std::next(se.begin()), bad.begin(), bad.end()),
                std::invalid_argument);

            // empty range
            it = se.insert(std::next(se.begin()),
                init.begin(), init.begin());
            BOOST_TEST_EQ(u.encoded_path(), "path/to/the/file.txt");
            BOOST_TEST_EQ(it, std::next(se.begin()));
        }

        // insert( const_iterator, initializer_list )
        {
            url u = parse_uri("x://y/path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            std::initializer_list<
                string_view> init = {
                    "to", "the" };
            auto it = se.insert(std::next(se.begin()), init);
            BOOST_TEST_EQ(cs.size(), 4u);
            BOOST_TEST_EQ(*it, "to");
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/path/to/the/file.txt?q#f");
        }

        // erase( const_iterator )
        {
            url u = parse_uri("x://y/path/to/the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            se.erase(std::next(se.begin()));
            BOOST_TEST_EQ(se.size(), 3u);
            BOOST_TEST_EQ(u.encoded_path(), "/path/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/path/the/file.txt?q#f");

            se.erase(se.begin());
            BOOST_TEST_EQ(se.size(), 2u);
            BOOST_TEST_EQ(u.encoded_path(), "/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/the/file.txt?q#f");

            se.erase(std::prev(se.end()));
            BOOST_TEST_EQ(se.size(), 1u);
            BOOST_TEST_EQ(u.encoded_path(), "/the");
            BOOST_TEST_EQ(u.string(), "x://y/the?q#f");

            se.erase(se.begin());
            BOOST_TEST(se.empty());
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.string(), "x://y/?q#f");
        }

        // erase( const_iterator, const_iterator )
        {
            url u = parse_uri("x://y/home/etc/path/to/the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            se.erase(se.begin(), std::next(se.begin(), 2));
            BOOST_TEST_EQ(u.encoded_path(), "/path/to/the/file.txt");
            BOOST_TEST_EQ(u.string(), "x://y/path/to/the/file.txt?q#f");

            se.erase(se.begin(), se.end());
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.string(), "x://y/?q#f");
        }

        // replace(iterator, iterator, initializer-list)
        {
            // initializer_list<String>
            url u = parse_relative_ref("/a/b/c/d/e/f/g").value();
            segments_encoded se = u.encoded_segments();
            auto it = se.replace(
                std::next(se.begin(), 1),
                std::next(se.begin(), 3),
                { "x", "y", "z" });
            BOOST_TEST_EQ(it, std::next(se.begin()));
            BOOST_TEST(u.encoded_path() ==
                "/a/x/y/z/d/e/f/g");
        }
        {
            // initializer_list<string_view>
            url u = parse_relative_ref("/a/b/c/d/e/f/g").value();
            segments_encoded se = u.encoded_segments();
            auto it = se.replace(
                std::next(se.begin(), 1),
                std::next(se.begin(), 3), {
                    string_view("x"),
                    string_view("y"),
                    string_view("z") });
            BOOST_TEST_EQ(it, std::next(se.begin()));
            BOOST_TEST(u.encoded_path() ==
                "/a/x/y/z/d/e/f/g");
        }

        // push_back
        {
            url u = parse_uri("x://y/home/etc/path/to/the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_THROWS(se.push_back("%"), std::invalid_argument);
            BOOST_TEST_THROWS(se.push_back("/"), std::invalid_argument);
            BOOST_TEST_THROWS(se.push_back("%2g"), std::invalid_argument);
        }

        // pop_back
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_EQ(se.size(), 3u);
            se.pop_back();
            BOOST_TEST_EQ(se.size(), 2u);
            BOOST_TEST_EQ(u.encoded_path(), "/path/to");
            BOOST_TEST_EQ(u.string(), "x://y/path/to?q#f");
            se.pop_back();
            BOOST_TEST_EQ(se.size(), 1u);
            BOOST_TEST_EQ(u.encoded_path(), "/path");
            BOOST_TEST_EQ(u.string(), "x://y/path?q#f");
            se.pop_back();
            BOOST_TEST_EQ(se.size(), 0u);
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.string(), "x://y/?q#f");
        }
    }

    void
    run()
    {
        testMembers();
        testElementAccess();
        testIterators();
        testCapacity();
        testModifiers();
    }
};

TEST_SUITE(
    segments_encoded_test,
    "boost.url.segments_encoded");

} // urls
} // boost

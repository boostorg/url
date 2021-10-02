//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_encoded.hpp>

#include <boost/url/url.hpp>
#include <boost/static_assert.hpp>
#include <initializer_list>
#include <iterator>
#if __cpp_lib_ranges >= 201911
//#include <ranges>
#endif
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

#if __cpp_lib_ranges >= 201911
    /*
    BOOST_STATIC_ASSERT(
        std::random_access_range<
            segments_encoded>);
    */

    BOOST_STATIC_ASSERT(
        std::random_access_iterator<
            segments_encoded::iterator>);

    BOOST_STATIC_ASSERT(
        std::random_access_iterator<
            segments_encoded::const_iterator>);
#endif

    void
    testMembers()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt?q#f").value();

        {
            url u = u0;
            u.encoded_segments() = { "etc", "index.htm" };
            BOOST_TEST(u.encoded_path() == "/etc/index.htm");
            BOOST_TEST(u.encoded_url() == "x://y/etc/index.htm?q#f");
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

            BOOST_TEST(se.at(0) == "path");
            BOOST_TEST(se.at(1) == "to");
            BOOST_TEST(se.at(2) == "the");
            BOOST_TEST(se.at(3) == "file.txt");
            BOOST_TEST_THROWS(se.at(4), std::out_of_range);

            // assign
            se.replace(se.begin() + 1, "from");
            // comparison
            BOOST_TEST(se.at(1) == "from");
            BOOST_TEST(se.at(1) != "path");
        }

        // operator[]
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(se[0] == "path");
            BOOST_TEST(se[1] == "to");
            BOOST_TEST(se[2] == "the");
            BOOST_TEST(se[3] == "file.txt");

            // assign
            se.replace(se.begin() + 1, "from");
            // comparison
            BOOST_TEST(se[1] == "from");
            BOOST_TEST(se[1] != "path");
        }

        // front
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(se.front() == "path");

            // assign
            se.replace(se.begin(), "etc");
            // comparison
            BOOST_TEST(se.front() == "etc");
            BOOST_TEST(se.front() != "path");
        }

        // back
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(se.back() == "file.txt");

            // assign
            se.replace(se.end() - 1, "index.htm");
            // comparison
            BOOST_TEST(se.back() == "index.htm");
            BOOST_TEST(se.back() != "file.txt");
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
        }

        // begin
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(se.begin() != se.end());
        }

        // end
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();

            BOOST_TEST(se.end() != se.begin());
        }

        //
        // iterator
        //

        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            auto const& cs(se);

            segments_encoded::iterator it = se.begin();
            BOOST_TEST(*it == "path");
            BOOST_TEST(*++it == "to");
            BOOST_TEST(*it++ == "to");
            BOOST_TEST(*it-- == "the");
            BOOST_TEST(*it == "to");
            BOOST_TEST(*--it == "path");
            BOOST_TEST(it == se.begin());
            BOOST_TEST(it != se.end());

            BOOST_TEST(*(it += 1) == "to");
            BOOST_TEST(*(it + 1) == "the");
            BOOST_TEST(*(1 + it) == "the");
            BOOST_TEST(*(it -= 1) == "path");
            it += 2;
            BOOST_TEST(*(it - 1) == "to");
            --it;
            BOOST_TEST(it - se.begin() == 1);
            BOOST_TEST(se.end() - it == 3);

            BOOST_TEST(it[0] == "to");
            BOOST_TEST(it[1] == "the")
;
            BOOST_TEST(it < se.end());
            BOOST_TEST(it < cs.end());
            BOOST_TEST(it <= se.end());
            BOOST_TEST(it <= cs.end());
            BOOST_TEST(it > se.begin());
            BOOST_TEST(it > cs.begin());
            BOOST_TEST(it >= se.begin());
            BOOST_TEST(it >= cs.begin());
            BOOST_TEST(it != se.begin());
            BOOST_TEST(it != cs.begin());
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

            BOOST_TEST(se.size() == 4);
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
            BOOST_TEST(se.size() == 4);
            se.clear();
            BOOST_TEST(se.empty());
            BOOST_TEST(se.size() == 0);
            BOOST_TEST(u.encoded_path() == "");
            BOOST_TEST(u.encoded_url() == "x://y");
        }

        // insert( const_iterator, string_view )
        {
            url u = parse_uri("x://y/path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            BOOST_TEST(se.size() == 2);
            segments_encoded::iterator it =
                se.insert(se.begin() + 1, "to");
            BOOST_TEST(se.size() == 3);
            BOOST_TEST(u.encoded_path() == "/path/to/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/file.txt?q#f");
            BOOST_TEST(*it == "to");

            it = se.insert(cs.end(), "");
            BOOST_TEST(se.size() == 4);
            BOOST_TEST(u.encoded_path() == "/path/to/file.txt/");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/file.txt/?q#f");
            BOOST_TEST(*it == "");
            BOOST_TEST(it[-1] == "file.txt");

            it = se.insert(se.begin(), "etc");
            BOOST_TEST(se.size() == 5);
            BOOST_TEST(u.encoded_path() == "/etc/path/to/file.txt/");
            BOOST_TEST(u.encoded_url() == "x://y/etc/path/to/file.txt/?q#f");
            BOOST_TEST(*it == "etc");

            BOOST_TEST_THROWS(se.insert(se.begin(), "%"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "/"), std::invalid_argument);
            BOOST_TEST_THROWS(se.insert(se.begin(), "%2g"), std::invalid_argument);
        }

        {
            // rootless
            url u = parse_uri("x:path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            BOOST_TEST(se.size() == 2);
            segments_encoded::iterator it =
                se.insert(se.begin() + 1, "to");
            BOOST_TEST(se.size() == 3);
            BOOST_TEST(u.encoded_path() == "path/to/file.txt");
            BOOST_TEST(u.encoded_url() == "x:path/to/file.txt?q#f");
            BOOST_TEST(*it == "to");

            it = se.insert(cs.end(), "");
            BOOST_TEST(se.size() == 4);
            BOOST_TEST(u.encoded_path() == "path/to/file.txt/");
            BOOST_TEST(u.encoded_url() == "x:path/to/file.txt/?q#f");
            BOOST_TEST(*it == "");
            BOOST_TEST(it[-1] == "file.txt");

            it = se.insert(se.begin(), "etc");
            BOOST_TEST(se.size() == 5);
            BOOST_TEST(u.encoded_path() == "etc/path/to/file.txt/");
            BOOST_TEST(u.encoded_url() == "x:etc/path/to/file.txt/?q#f");
            BOOST_TEST(*it == "etc");

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
                se.begin() + 1, init.begin(), init.end());
            BOOST_TEST(cs.size() == 4);
            BOOST_TEST(*it == "to");
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");

            std::initializer_list<string_view> bad = {"%"};
            BOOST_TEST_THROWS(se.insert(
                se.begin() + 1, bad.begin(), bad.end()),
                std::invalid_argument);

            // empty range
            it = se.insert(se.begin() + 1,
                init.begin(), init.begin());
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(it == se.begin() + 1);
        }
        {
            // rootless
            url u = parse_uri("x:the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            std::initializer_list<string_view> init = {"path", "to" };
            auto it = se.insert(
                se.begin(), init.begin(), init.end());
            BOOST_TEST(cs.size() == 4);
            BOOST_TEST(*it == "path");
            BOOST_TEST(u.encoded_path() == "path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x:path/to/the/file.txt?q#f");

            std::initializer_list<string_view> bad = {"%"};
            BOOST_TEST_THROWS(se.insert(
                se.begin() + 1, bad.begin(), bad.end()),
                std::invalid_argument);

            // empty range
            it = se.insert(se.begin() + 1,
                init.begin(), init.begin());
            BOOST_TEST(u.encoded_path() == "path/to/the/file.txt");
            BOOST_TEST(it == se.begin() + 1);
        }

        // insert( const_iterator, initializer_list )
        {
            url u = parse_uri("x://y/path/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            std::initializer_list<
                string_view> init = {
                    "to", "the" };
            auto it = se.insert(se.begin() + 1, init);
            BOOST_TEST(cs.size() == 4);
            BOOST_TEST(*it == "to");
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");
        }

        // erase( const_iterator )
        {
            url u = parse_uri("x://y/path/to/the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            se.erase(se.begin() + 1);
            BOOST_TEST(se.size() == 3);
            BOOST_TEST(u.encoded_path() == "/path/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/the/file.txt?q#f");

            se.erase(se.begin());
            BOOST_TEST(se.size() == 2);
            BOOST_TEST(u.encoded_path() == "/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/the/file.txt?q#f");

            se.erase(se.end() - 1);
            BOOST_TEST(se.size() == 1);
            BOOST_TEST(u.encoded_path() == "/the");
            BOOST_TEST(u.encoded_url() == "x://y/the?q#f");

            se.erase(se.begin());
            BOOST_TEST(se.empty());
            BOOST_TEST(u.encoded_path().empty());
            BOOST_TEST(u.encoded_url() == "x://y?q#f");
        }

        // erase( const_iterator, const_iterator )
        {
            url u = parse_uri("x://y/home/etc/path/to/the/file.txt?q#f").value();
            segments_encoded se = u.encoded_segments();

            se.erase(se.begin(), se.begin() + 2);
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");

            se.erase(se.begin(), se.end());
            BOOST_TEST(u.encoded_path() == "");
            BOOST_TEST(u.encoded_url() == "x://y?q#f");
        }

        // replace(iterator, iterator, initializer-list)
        {
            // initializer_list<String>
            url u = parse_relative_ref("/a/b/c/d/e/f/g").value();
            segments_encoded se = u.encoded_segments();
            auto it = se.replace(
                se.begin() + 1,
                se.begin() + 3,
                { "x", "y", "z" });
            BOOST_TEST(it == se.begin() + 1);
            BOOST_TEST(u.encoded_path() ==
                "/a/x/y/z/d/e/f/g");
        }
        {
            // initializer_list<string_view>
            url u = parse_relative_ref("/a/b/c/d/e/f/g").value();
            segments_encoded se = u.encoded_segments();
            auto it = se.replace(
                se.begin() + 1,
                se.begin() + 3, {
                    string_view("x"),
                    string_view("y"),
                    string_view("z") });
            BOOST_TEST(it == se.begin() + 1);
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

            BOOST_TEST(se.size() == 3);
            se.pop_back();
            BOOST_TEST(se.size() == 2);
            BOOST_TEST(u.encoded_path() == "/path/to");
            BOOST_TEST(u.encoded_url() == "x://y/path/to?q#f");
            se.pop_back();
            BOOST_TEST(se.size() == 1);
            BOOST_TEST(u.encoded_path() == "/path");
            BOOST_TEST(u.encoded_url() == "x://y/path?q#f");
            se.pop_back();
            BOOST_TEST(se.size() == 0);
            BOOST_TEST(u.encoded_path().empty());
            BOOST_TEST(u.encoded_url() == "x://y?q#f");
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

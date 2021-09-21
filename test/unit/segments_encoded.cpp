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
#include <sstream>
#include "test_suite.hpp"

namespace boost {
namespace urls {

//------------------------------------------------

class segments_encoded_test
{
public:
    // const_reference

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::const_reference,
            string_view>::value);
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::const_reference,
            std::string>::value);
#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::const_reference,
            std::string_view>::value);
#endif
    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments_encoded::const_reference,
            std::wstring>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments_encoded::const_reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments_encoded::const_reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments_encoded::const_reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments_encoded::const_reference,
            char const(&)[2]>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments_encoded::const_reference,
            char const*>::value);

    // reference

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::reference,
            string_view>::value);
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::reference,
            std::string>::value);
#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments_encoded::reference,
            std::string_view>::value);
#endif
    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments_encoded::reference,
            std::wstring>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments_encoded::reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments_encoded::reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments_encoded::reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments_encoded::reference,
            char const(&)[2]>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments_encoded::reference,
            char const*>::value);

    void
    testConvert()
    {
        // convert reference to stringish
        {
            url u = parse_relative_ref("/");
            segments_encoded se =
                u.encoded_segments();

            string_view() = *se.begin();
            std::string() = *se.begin();
        #ifdef BOOST_URL_HAS_STRING_VIEW
            std::string_view() = *se.begin();
        #endif

            string_view() = *se.cbegin();
            std::string() = *se.cbegin();
        #ifdef BOOST_URL_HAS_STRING_VIEW
            std::string_view() = *se.cbegin();
        #endif
        }
    }

    void
    testAssign()
    {
        // assign stringish to reference
        {
            url u = parse_relative_ref("/");
            segments_encoded se =
                u.encoded_segments();

            *se.begin() = string_view();
            *se.begin() = std::string();
        #ifdef BOOST_URL_HAS_STRING_VIEW
            *se.begin() = std::string_view();
        #endif
            *se.begin() = "x";
        }
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            segments_encoded se =
                u.encoded_segments();

            BOOST_TEST(se.front() == "path");
            BOOST_TEST(se.back() == "file.txt");

            se.front() = "x";
            se[1] = "p";
            se[2] = "i.htm";
            se[0] = "path";
            BOOST_TEST(u.encoded_url() ==
                "/path/p/i.htm");
            se[1] = "to";
            se[2] = "file.txt";
            se[1] = se[2];
            se[0] = se[2];
            BOOST_TEST(u.encoded_url() ==
                "/file.txt/file.txt/file.txt");
        }
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            segments_encoded seg =
                u.encoded_segments();
            std::reverse(
                seg.begin(), seg.end());
            BOOST_TEST(u.encoded_url() ==
                "/file.txt/to/path");
            seg[1] = "";
            BOOST_TEST(u.encoded_url() ==
                "/file.txt//path");
        }
    }

    void
    testModifiers()
    {
        // push_back
        {
            url u;
            auto es = u.encoded_segments();
            BOOST_TEST(u.segment_count() == 0);
            es.push_back("path");
            BOOST_TEST(u.segment_count() == 1);
            BOOST_TEST(u.encoded_url() == "/path");
            es.push_back("to");
            BOOST_TEST(u.segment_count() == 2);
            BOOST_TEST(u.encoded_url() == "/path/to");
            es.push_back("file.txt");
            BOOST_TEST(u.segment_count() == 3);
            BOOST_TEST(u.encoded_url() == "/path/to/file.txt");

            BOOST_TEST_THROWS(es.push_back("%"), std::invalid_argument);
            BOOST_TEST_THROWS(es.push_back("/"), std::invalid_argument);
            BOOST_TEST_THROWS(es.push_back("%2g"), std::invalid_argument);
        }

        // pop_back
        {
            url u = parse_relative_ref("/path/to/file.txt");
            auto es = u.encoded_segments();
            BOOST_TEST(es.size() == 3);
            es.pop_back();
            BOOST_TEST(es.size() == 2);
            BOOST_TEST(u.encoded_path() == "/path/to");
            es.pop_back();
            BOOST_TEST(es.size() == 1);
            BOOST_TEST(u.encoded_path() == "/path");
            es.pop_back();
            BOOST_TEST(es.size() == 0);
            BOOST_TEST(u.encoded_path().empty());
        }

        // erase
        {
            url u = parse_relative_ref("/home/etc/path/to/the/file.txt");
            auto es = u.encoded_segments();
            es.erase(es.begin() + 1);
            BOOST_TEST(u.segment_count() == 5);
            BOOST_TEST(u.encoded_url() == "/home/path/to/the/file.txt");
            es.erase(es.begin() + 3);
            BOOST_TEST(u.segment_count() == 4);
            BOOST_TEST(u.encoded_url() == "/home/path/to/file.txt");
            es.erase(es.begin() + 1, es.begin() + 3);
            BOOST_TEST(u.segment_count() == 2);
            BOOST_TEST(u.encoded_url() == "/home/file.txt");
            auto it = es.erase(es.begin() + 1);
            BOOST_TEST(it == es.end());
            BOOST_TEST(u.segment_count() == 1);
            BOOST_TEST(u.encoded_url() == "/home");
            BOOST_TEST(es.erase(es.begin()) == es.begin());
            BOOST_TEST(u.segment_count() == 0);
            BOOST_TEST(u.encoded_url().empty());
        }

        // clear
        {
            url u = parse_relative_ref("/path/to/file.txt");
            auto es = u.encoded_segments();
            es.clear();
            BOOST_TEST(u.encoded_url().empty());
            BOOST_TEST(u.segment_count() == 0);
        }

        // insert
        {
            url u = parse_relative_ref("");
            auto es = u.encoded_segments();
            es.insert(es.end(), "file.txt");
            BOOST_TEST(es.size() == 1);
            BOOST_TEST(u.encoded_path() == "/file.txt");
            es.insert(es.begin(), "path");
            BOOST_TEST(es.size() == 2);
            BOOST_TEST(u.encoded_path() == "/path/file.txt");
            es.insert(es.end() - 1, "to");
            BOOST_TEST(es.size() == 3);
            BOOST_TEST(u.encoded_path() == "/path/to/file.txt");

            BOOST_TEST_THROWS(es.insert(es.end(), "%"), std::invalid_argument);
            BOOST_TEST_THROWS(es.insert(es.end(), "/"), std::invalid_argument);
            BOOST_TEST_THROWS(es.insert(es.end(), "%2g"), std::invalid_argument);
        }
    }

    void
    testAlgorithms()
    {
        // these should compile
        url u;
        {
            std::stringstream ss;
            segments_encoded seg =
                u.encoded_segments();
            std::copy(
                seg.begin(),
                seg.end(),
                std::ostream_iterator<
                    std::string>( ss ) );
        }
        {
            segments_encoded seg =
                u.encoded_segments();
            (void)std::remove(
                seg.begin(),
                seg.end(),
                "" );
        }
        {
            segments_encoded seg =
                u.encoded_segments();
            (void)std::remove(
                seg.begin(),
                seg.end(),
                std::string("") );
        }
    }

    void
    run()
    {
        testConvert();
        testAssign();
        testModifiers();
        testAlgorithms();
    }
};

TEST_SUITE(
    segments_encoded_test,
    "boost.url.segments_encoded");

} // urls
} // boost

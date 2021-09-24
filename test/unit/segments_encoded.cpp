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
#include <sstream>
#include "test_suite.hpp"

namespace boost {
namespace urls {

//BOOST_ASSERT(std::_Is_string_view_ish<segments_encoded::iterator>::value);

//------------------------------------------------

class segments_encoded_test
{
public:
    BOOST_STATIC_ASSERT(
        std::is_default_constructible<
            segments_encoded::iterator>::value);

    BOOST_STATIC_ASSERT(
        std::is_default_constructible<
            segments_encoded::const_iterator>::value);

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
    testMembers()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt?q#f");

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
            "/path/to/the/file.txt");

        {
            using T1 = segments_encoded::iterator;
            using T2 = segments_encoded::const_iterator;
            using T3 = segments_encoded::reference;
            using T4 = segments_encoded::const_reference;
            using T5 = segments_encoded::value_type;
            using T6 = segments_encoded::size_type;
            using T7 = segments_encoded::difference_type;
        }

        // at
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(string_view(se.at(0)) == "path");
            BOOST_TEST(string_view(se.at(1)) == "to");
            BOOST_TEST(string_view(se.at(2)) == "the");
            BOOST_TEST(string_view(se.at(3)) == "file.txt");
            BOOST_TEST_THROWS(se.at(4), std::out_of_range);

            BOOST_TEST(string_view(cs.at(0)) == "path");
            BOOST_TEST(string_view(cs.at(1)) == "to");
            BOOST_TEST(string_view(cs.at(2)) == "the");
            BOOST_TEST(string_view(cs.at(3)) == "file.txt");
            BOOST_TEST_THROWS(cs.at(4), std::out_of_range);

            // assign
            se.at(1) = "from";
            // comparison
            BOOST_TEST(se.at(1) == "from");
            BOOST_TEST(cs.at(1) == "from");
            BOOST_TEST(se.at(1) != "path");
            BOOST_TEST(cs.at(1) != "path");
        }

        // operator[]
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(string_view(se[0]) == "path");
            BOOST_TEST(string_view(se[1]) == "to");
            BOOST_TEST(string_view(se[2]) == "the");
            BOOST_TEST(string_view(se[3]) == "file.txt");

            BOOST_TEST(string_view(cs[0]) == "path");
            BOOST_TEST(string_view(cs[1]) == "to");
            BOOST_TEST(string_view(cs[2]) == "the");
            BOOST_TEST(string_view(cs[3]) == "file.txt");

            // assign
            se[1] = "from";
            // comparison
            BOOST_TEST(se[1] == "from");
            BOOST_TEST(cs[1] == "from");
            BOOST_TEST(se[1] != "path");
            BOOST_TEST(cs[1] != "path");
        }

        // front
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(se.front() == "path");
            BOOST_TEST(cs.front() == "path");

            // assign
            se.front() = "etc";
            // comparison
            BOOST_TEST(se.front() == "etc");
            BOOST_TEST(cs.front() == "etc");
            BOOST_TEST(se.front() != "path");
            BOOST_TEST(cs.front() != "path");
        }

        // back
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(se.back() == "file.txt");
            BOOST_TEST(cs.back() == "file.txt");

            // assign
            se.back() = "index.htm";
            // comparison
            BOOST_TEST(se.back() == "index.htm");
            BOOST_TEST(cs.back() == "index.htm");
            BOOST_TEST(se.back() != "file.txt");
            BOOST_TEST(cs.back() != "file.txt");
        }
    }

    void
    testIterators()
    {
        auto const f = [](
            segments_encoded::iterator i,
            segments_encoded::const_iterator j,
            std::ptrdiff_t n)
        {
            // check that these expressions
            // compile without executing them
            ++i;
            i++;
            i += n;
            j + n;
            n + j;
            i -= n;
            j - n;
            j[n];
        };

        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt");

        // (default-ctor)
        {
            segments_encoded::iterator it;
            segments_encoded::const_iterator cit;

            url u = u0;
            segments_encoded se = u.encoded_segments();

            it = se.begin();

            // assign from non-const
            cit = se.begin();

            // heterogeneous comparison
            BOOST_TEST(cit == it);
            BOOST_TEST(it != se.end());
            BOOST_TEST(it != se.cend());
            BOOST_TEST(cit != se.end());
        }

        // begin
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(se.begin() == cs.begin());
            BOOST_TEST(se.cbegin() == cs.begin());
            BOOST_TEST(se.end() != se.begin());
            BOOST_TEST(se.end() != se.cbegin());
        }

        // end
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(se.end() == cs.end());
            BOOST_TEST(se.cend() == cs.end());
            BOOST_TEST(se.begin() != se.end());
            BOOST_TEST(se.begin() != se.cend());
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
            BOOST_TEST(it - se.cbegin() == 1);
            BOOST_TEST(se.end() - it == 3);
            BOOST_TEST(se.cend() - it == 3);

            BOOST_TEST(it[0] == "to");
            BOOST_TEST(it[1] == "the")
;
            BOOST_TEST(it < se.end());
            BOOST_TEST(it < se.cend());
            BOOST_TEST(it < cs.end());
            BOOST_TEST(it <= se.end());
            BOOST_TEST(it <= se.cend());
            BOOST_TEST(it <= cs.end());
            BOOST_TEST(it > se.begin());
            BOOST_TEST(it > se.cbegin());
            BOOST_TEST(it > cs.begin());
            BOOST_TEST(it >= se.begin());
            BOOST_TEST(it >= se.cbegin());
            BOOST_TEST(it >= cs.begin());
            BOOST_TEST(it != se.begin());
            BOOST_TEST(it != se.cbegin());
            BOOST_TEST(it != cs.begin());

            segments_encoded::const_iterator cit =
                cs.cbegin();
            BOOST_TEST(*cit == "path");
            BOOST_TEST(*++cit == "to");
            BOOST_TEST(*cit++ == "to");
            BOOST_TEST(*cit-- == "the");
            BOOST_TEST(*cit == "to");
            BOOST_TEST(*--cit == "path");
            BOOST_TEST(cit == se.begin());
            BOOST_TEST(cit != se.end());

            BOOST_TEST(*(cit += 1) == "to");
            BOOST_TEST(*(cit + 1) == "the");
            BOOST_TEST(*(1 + cit) == "the");
            BOOST_TEST(*(cit -= 1) == "path");
            cit += 2;
            BOOST_TEST(*(cit - 1) == "to");
            --cit;
            BOOST_TEST(cit - se.begin() == 1);
            BOOST_TEST(cit - se.cbegin() == 1);
            BOOST_TEST(se.end() - cit == 3);
            BOOST_TEST(se.cend() - cit == 3);

            BOOST_TEST(cit[0] == "to");
            BOOST_TEST(cit[1] == "the")
;
            BOOST_TEST(cit < se.end());
            BOOST_TEST(cit < se.cend());
            BOOST_TEST(cit < cs.end());
            BOOST_TEST(cit <= se.end());
            BOOST_TEST(cit <= se.cend());
            BOOST_TEST(cit <= cs.end());
            BOOST_TEST(cit > se.begin());
            BOOST_TEST(cit > se.cbegin());
            BOOST_TEST(cit > cs.begin());
            BOOST_TEST(cit >= se.begin());
            BOOST_TEST(cit >= se.cbegin());
            BOOST_TEST(cit >= cs.begin());
            BOOST_TEST(cit != se.begin());
            BOOST_TEST(cit != se.cbegin());
            BOOST_TEST(cit != cs.begin());

            // heterogeneous assignment
            cit = it;
            BOOST_TEST(cit == it);
            ++it;
            BOOST_TEST(cit != it);
            BOOST_TEST(it > cit);
            BOOST_TEST(cit < it);
        }

    }

    void
    testCapacity()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt");

        // empty
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(! se.empty());
            BOOST_TEST(! cs.empty());
        }

        // size
        {
            url u = u0;
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs = se;

            BOOST_TEST(se.size() == 4);
            BOOST_TEST(cs.size() == 4);
        }
    }

    void
    testModifiers()
    {
        // clear
        {
            url u = parse_uri("x://y/path/to/the/file.txt");
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
            url u = parse_uri("x://y/path/file.txt?q#f");
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

            it = se.insert(se.cbegin(), "etc");
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
            url u = parse_uri("x:path/file.txt?q#f");
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

            it = se.insert(se.cbegin(), "etc");
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
            url u = parse_uri("x://y/path/file.txt?q#f");
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
            url u = parse_uri("x:the/file.txt?q#f");
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
            url u = parse_uri("x://y/path/file.txt?q#f");
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
            url u = parse_uri("x://y/path/to/the/file.txt?q#f");
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
            url u = parse_uri("x://y/home/etc/path/to/the/file.txt?q#f");
            segments_encoded se = u.encoded_segments();

            se.erase(se.begin(), se.begin() + 2);
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");

            se.erase(se.begin(), se.end());
            BOOST_TEST(u.encoded_path() == "");
            BOOST_TEST(u.encoded_url() == "x://y?q#f");
        }

        // push_back
        {
            url u = parse_uri("x://y/home/etc/path/to/the/file.txt?q#f");
            segments_encoded se = u.encoded_segments();

            BOOST_TEST_THROWS(se.push_back("%"), std::invalid_argument);
            BOOST_TEST_THROWS(se.push_back("/"), std::invalid_argument);
            BOOST_TEST_THROWS(se.push_back("%2g"), std::invalid_argument);
        }

        // pop_back
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
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
    testConversion()
    {
        auto const implicit =
            [](std::string s)
            {
                return s;
            };

        // reference
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
            segments_encoded se = u.encoded_segments();

            segments_encoded::reference r = se[2];
            BOOST_TEST(string_view(r) == "file.txt");
            BOOST_TEST(static_cast<std::string>(r) == "file.txt");
        #ifdef BOOST_URL_HAS_STRING_VIEW
            BOOST_TEST(std::string_view(r) == "file.txt");
        #endif
            BOOST_TEST("file.txt" == r);
            BOOST_TEST("path" != r);
            // force operator std::string()
            BOOST_TEST(implicit(r) == "file.txt");
        }

        // const_reference
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
            segments_encoded se = u.encoded_segments();
            segments_encoded const& cs(se);

            segments_encoded::const_reference cr = cs[1];
            BOOST_TEST(string_view(cr) == "to");
            BOOST_TEST(static_cast<std::string>(cr) == "to");
        #ifdef BOOST_URL_HAS_STRING_VIEW
            BOOST_TEST(std::string_view(cr) == "to");
        #endif
            BOOST_TEST("to" == cr);
            BOOST_TEST("path" != cr);
            // force operator std::string()
            BOOST_TEST(implicit(cr) == "to");

            segments_encoded::reference r = se[2];

            // construct from reference
            segments_encoded::const_reference cr2(r);
            BOOST_TEST(cr2 == r);
            BOOST_TEST(r == cr2);
            BOOST_TEST(cr2 == "file.txt");
            BOOST_TEST("file.txt" == cr2);

            segments_encoded::const_reference cr3(se[0]);
            BOOST_TEST(cr3 != r);
            BOOST_TEST(r != cr3);
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

        // assign const_reference to reference
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            segments_encoded se = u.encoded_segments();
            auto const& cs(se);

            se[0] = cs[2];
            BOOST_TEST(u.encoded_url() == "/file.txt/to/file.txt");
        }

        // grammar compliance
        {
            // path-noscheme
            url u = parse_relative_ref("y?q#f");
            u.encoded_segments()[0] = "a:b";
            BOOST_TEST(u.encoded_path() == "./a:b");
            BOOST_TEST(u.encoded_url() == "./a:b?q#f");
        }
        {
            // path-absolute
            url u = parse_relative_ref("/a/b/c?q#f");
            u.encoded_segments() = { "", "" };
            BOOST_TEST(u.encoded_path() == "/.//");
            BOOST_TEST(u.encoded_url() == "/.//?q#f");
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
        testMembers();
        testElementAccess();
        testIterators();
        testCapacity();
        testModifiers();

        testConversion();
        testAssign();
        testAlgorithms();;
    }
};

TEST_SUITE(
    segments_encoded_test,
    "boost.url.segments_encoded");

} // urls
} // boost

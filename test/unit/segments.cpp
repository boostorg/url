//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments.hpp>

#include <boost/url/static_pool.hpp>
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

//------------------------------------------------

class segments_test
{
public:
    using Alloc = std::allocator<char>;
    using alloc_type =
        basic_static_pool::allocator_type<char>;

#if __cpp_lib_ranges >= 201911
    /*
    BOOST_STATIC_ASSERT(
        std::random_access_range<
            segments>);
    */

    BOOST_STATIC_ASSERT(
        std::random_access_iterator<
            segments<Alloc>::iterator>);

    BOOST_STATIC_ASSERT(
        std::random_access_iterator<
            segments<Alloc>::const_iterator>);
#endif

    // const_reference

    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::const_reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments<Alloc>::const_reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::const_reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::const_reference,
            std::wstring>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments<Alloc>::const_reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments<Alloc>::const_reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments<Alloc>::const_reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments<Alloc>::const_reference,
            char const(&)[2]>::value);

    BOOST_STATIC_ASSERT(
        ! std::is_assignable<
            segments<Alloc>::const_reference,
            char const*>::value);

    // reference

    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            segments<Alloc>::reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        ! std::is_convertible<
            segments<Alloc>::reference,
            std::wstring>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments<Alloc>::reference,
            string_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments<Alloc>::reference,
            std::string>::value);

#ifdef BOOST_URL_HAS_STRING_VIEW
    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments<Alloc>::reference,
            std::string_view>::value);
#endif

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments<Alloc>::reference,
            char const(&)[2]>::value);

    BOOST_STATIC_ASSERT(
        std::is_assignable<
            segments<Alloc>::reference,
            char const*>::value);

    static_pool<4096> p_;

    void
    testMembers()
    {
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt?q#f");

        {
            url u = u0;
            u.segments(p_.allocator()) = { "etc", "index.htm" };
            BOOST_TEST(u.encoded_path() == "/etc/index.htm");
            BOOST_TEST(u.encoded_url() == "x://y/etc/index.htm?q#f");
        }
    }

    void
    testElementAccess()
    {
        url_view const u0 = parse_relative_ref(
            "/path/to/the/file.txt");

        // at
        {
            url u = u0;
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

            BOOST_TEST(se.at(0) == "path");
            BOOST_TEST(se.at(1) == "to");
            BOOST_TEST(se.at(2) == "the");
            BOOST_TEST(se.at(3) == "file.txt");
            BOOST_TEST_THROWS(se.at(4), std::out_of_range);

            BOOST_TEST(cs.at(0) == "path");
            BOOST_TEST(cs.at(1) == "to");
            BOOST_TEST(cs.at(2) == "the");
            BOOST_TEST(cs.at(3) == "file.txt");
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
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

            BOOST_TEST(se[0] == "path");
            BOOST_TEST(se[1] == "to");
            BOOST_TEST(se[2] == "the");
            BOOST_TEST(se[3] == "file.txt");

            BOOST_TEST(cs[0] == "path");
            BOOST_TEST(cs[1] == "to");
            BOOST_TEST(cs[2] == "the");
            BOOST_TEST(cs[3] == "file.txt");

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
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

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
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

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
        url_view const u0 = parse_uri(
            "x://y/path/to/the/file.txt");

        // (default-ctor)
        {
            segments<Alloc>::iterator it;
            segments<Alloc>::const_iterator cit;

            url u = u0;
            auto se = u.segments();

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
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

            BOOST_TEST(se.begin() == cs.begin());
            BOOST_TEST(se.cbegin() == cs.begin());
            BOOST_TEST(se.end() != se.begin());
            BOOST_TEST(se.end() != se.cbegin());
        }

        // end
        {
            url u = u0;
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

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
            auto se = u.segments();
            auto const& cs(se);

            segments<Alloc>::iterator it = se.begin();
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

            segments<Alloc>::const_iterator cit =
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
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

            BOOST_TEST(! se.empty());
            BOOST_TEST(! cs.empty());
        }

        // size
        {
            url u = u0;
            auto se = u.segments(p_.allocator());
            auto const& cs = se;

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
            auto se = u.segments(p_.allocator());

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
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            BOOST_TEST(se.size() == 2);
            auto it =
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
        }

        {
            // rootless
            url u = parse_uri("x:path/file.txt?q#f");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            BOOST_TEST(se.size() == 2);
            auto it =
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
        }

        // insert( const_iterator, FwdIt, FwdIt )
        {
            url u = parse_uri("x://y/path/file.txt?q#f");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            std::initializer_list<string_view> init = {"to", "the" };
            auto it = se.insert(
                se.begin() + 1, init.begin(), init.end());
            BOOST_TEST(cs.size() == 4);
            BOOST_TEST(*it == "to");
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");

            // empty range
            it = se.insert(se.begin() + 1,
                init.begin(), init.begin());
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(it == se.begin() + 1);
        }
        {
            // rootless
            url u = parse_uri("x:the/file.txt?q#f");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            std::initializer_list<string_view> init = {"path", "to" };
            auto it = se.insert(
                se.begin(), init.begin(), init.end());
            BOOST_TEST(cs.size() == 4);
            BOOST_TEST(*it == "path");
            BOOST_TEST(u.encoded_path() == "path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x:path/to/the/file.txt?q#f");

            // empty range
            it = se.insert(se.begin() + 1,
                init.begin(), init.begin());
            BOOST_TEST(u.encoded_path() == "path/to/the/file.txt");
            BOOST_TEST(it == se.begin() + 1);
        }

        // insert( const_iterator, initializer_list )
        {
            url u = parse_uri("x://y/path/file.txt?q#f");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

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
            auto se = u.segments(p_.allocator());

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
            auto se = u.segments(p_.allocator());

            se.erase(se.begin(), se.begin() + 2);
            BOOST_TEST(u.encoded_path() == "/path/to/the/file.txt");
            BOOST_TEST(u.encoded_url() == "x://y/path/to/the/file.txt?q#f");

            se.erase(se.begin(), se.end());
            BOOST_TEST(u.encoded_path() == "");
            BOOST_TEST(u.encoded_url() == "x://y?q#f");
        }

        // push_back
        {
            // VFALCO TODO
        }

        // pop_back
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
            auto se = u.segments(p_.allocator());

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
        // reference
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
            auto se = u.segments(p_.allocator());

            auto r = se[2];
            BOOST_TEST(r == "file.txt");
            BOOST_TEST("file.txt" == r);
            BOOST_TEST("path" != r);
        }

        // const_reference
        {
            url u = parse_uri("x://y/path/to/file.txt?q#f");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            auto cr = cs[1];
            BOOST_TEST(cr == "to");
            BOOST_TEST("to" == cr);
            BOOST_TEST("path" != cr);
            // force operator std::string()

            auto r = se[2];

            // construct from reference
            segments<alloc_type>::const_reference cr2(r);
            BOOST_TEST(cr2 == r);
            BOOST_TEST(r == cr2);
            BOOST_TEST(cr2 == "file.txt");
            BOOST_TEST("file.txt" == cr2);

            segments<alloc_type>::const_reference cr3(se[0]);
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
            auto se =
                u.segments(p_.allocator());

            *se.begin() = string_view();
            *se.begin() = std::string();
            *se.begin() = "x";
        }
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            auto se = u.segments(p_.allocator());

            BOOST_TEST(se.front() == "path");
            BOOST_TEST(se.back() == "file.txt");

            se.front() = "x";
            se[1] = "p_";
            se[2] = "i.htm";
            se[0] = "path";
            BOOST_TEST(u.encoded_url() ==
                "/path/p_/i.htm");
            se[1] = "to";
            se[2] = "file.txt";
            se[1] = se[2];
            se[0] = se[2];
            BOOST_TEST(u.encoded_url() ==
                "/file.txt/file.txt/file.txt");
        }
#if 0
        // VFALCO BROKEN
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            segments<Alloc> seg =
                u.segments(p_.allocator());
            std::reverse(
                seg.begin(), seg.end());
            BOOST_TEST(u.encoded_url() ==
                "/file.txt/to/path");
            seg[1] = "";
            BOOST_TEST(u.encoded_url() ==
                "/file.txt//path");
        }
#endif

        // assign const_reference to reference
        {
            url u = parse_relative_ref(
                "/path/to/file.txt");
            auto se = u.segments(p_.allocator());
            auto const& cs(se);

            se[0] = cs[2];
            BOOST_TEST(u.encoded_url() == "/file.txt/to/file.txt");
        }

        // grammar compliance
        {
            // path-noscheme
            url u = parse_relative_ref("y?q#f");
            u.segments(p_.allocator())[0] = "a:b";
            BOOST_TEST(u.encoded_path() == "./a:b");
            BOOST_TEST(u.encoded_url() == "./a:b?q#f");
        }
        {
            // path-absolute
            url u = parse_relative_ref("/a/b/c?q#f");
            u.segments(p_.allocator()) = { "", "" };
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
            auto seg =
                u.segments(p_.allocator());
            std::copy(
                seg.begin(),
                seg.end(),
                std::ostream_iterator<
                    string_type<alloc_type>>( ss ) );
        }
        {
            auto seg =
                u.segments(p_.allocator());
            (void)std::remove(
                seg.begin(),
                seg.end(),
                "" );
        }
        {
            auto seg =
                u.segments(p_.allocator());
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
        testAlgorithms();

        {

        }
    }
};

TEST_SUITE(
    segments_test,
    "boost.url.segments");

} // urls
} // boost

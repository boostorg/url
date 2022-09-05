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
#include <boost/url/segments_ref.hpp>

#include <boost/url/url.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

//------------------------------------------------

struct segments_ref_test
{
    using Type = segments_encoded_ref;

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
        if(! BOOST_TEST_EQ(ps.size(), init.size()))
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
    run()
    {
#if 0
        testEditSegments();
        url u( "/path/to/the/file.txt" );
        segments_ref ps = u.segments();
        ps.erase(
            std::next(ps.begin(), 1),
            std::next(ps.begin(), 3));
#endif
    }
};

TEST_SUITE(
    segments_ref_test,
    "boost.url.segments_ref");

} // urls
} // boost

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/params_base.hpp>

#include <boost/url/params_const_view.hpp>
#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class params_base_test
{
public:
    void
    testCapacity()
    {
        // empty
        // size
        {
            url_view u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_const_view p = u.params();
            BOOST_TEST(! p.empty());
            BOOST_TEST_EQ(p.size(), 5u);
        }
        {
            url_view u;
            params_const_view p = u.params();
            BOOST_TEST(p.empty());
            BOOST_TEST_EQ(p.size(), 0u);
        }
    }

    void
    testLookup()
    {
        // count(string_view)
        // count(Key)
        // find(string_view)
        // find(Key)
        // find(iterator, string_view)
        // find(iterator, Key)
        // contains(string_view)
        // contains(Key)
        {
            url_view u = parse_uri_reference(
                "/?a=1&%62=2&c=3&c=4&c=5&d=6&e=7&d=8&f=9#f").value();
            params_const_view p = u.params();
            BOOST_TEST_EQ(p.count("a"), 1u);
            BOOST_TEST_EQ(p.count("b"), 1u);
            BOOST_TEST_EQ(p.count("c"), 3u);
            BOOST_TEST_EQ(p.count("d"), 2u);
            BOOST_TEST_EQ(p.count("e"), 1u);
            BOOST_TEST_EQ(p.count("f"), 1u);
            BOOST_TEST_EQ(p.count("g"), 0u);

            BOOST_TEST(p.find("b") ==
                std::next(p.begin()));
            BOOST_TEST(p.find(
                std::next(p.begin(), 6), "d") ==
                std::next(p.begin(), 7));

            BOOST_TEST(p.contains("a"));
            BOOST_TEST(p.contains("b"));
            BOOST_TEST(p.contains("c"));
            BOOST_TEST(p.contains("d"));
            BOOST_TEST(p.contains("e"));
            BOOST_TEST(p.contains("f"));
            BOOST_TEST(! p.contains("g"));
        }
    }

    void
    testIterators()
    {
        // operator++
        // operator++(int)
        {
            url_view u = parse_uri_reference(
                "/?a=1&bb=22&ccc=333&dddd=4444#f").value();
            params_const_view p = u.params();
            auto it = p.begin();
            BOOST_TEST_EQ((*it).key, "a");
            BOOST_TEST_EQ((*++it).key, "bb");
            BOOST_TEST_EQ((*it++).key, "bb");
            BOOST_TEST_EQ((*it).key, "ccc");
            auto it2 = p.end();
            BOOST_TEST_EQ(it2, p.end());
            BOOST_TEST_NE(it, it2);
        }

        // operator*
        {
            url_view u = parse_uri_reference(
                "/?&x&y=&z=3#f").value();
            params_const_view p = u.params();
            BOOST_TEST_EQ(p.size(), 4u);
            auto it = p.begin();

            params_const_view::value_type v;

            v = *it++;
            BOOST_TEST_EQ(v.key, "");
            BOOST_TEST_EQ(v.value, "");
            BOOST_TEST_EQ(v.has_value, false);

            v = *it++;
            BOOST_TEST_EQ(v.key, "x");
            BOOST_TEST_EQ(v.value, "");
            BOOST_TEST_EQ(v.has_value, false);

            v = *it++;
            BOOST_TEST_EQ(v.key, "y");
            BOOST_TEST_EQ(v.value, "");
            BOOST_TEST_EQ(v.has_value, true);

            v = *it++;
            BOOST_TEST_EQ(v.key, "z");
            BOOST_TEST_EQ(v.value, "3");
            BOOST_TEST_EQ(v.has_value, true);
        }

        // value_type outlives reference
        {
            url_view u("/?a=1&bb=22&ccc=333&dddd=4444#f");
            params_const_view::value_type v;
            {
                params_const_view ps = u.params();
                params_const_view::reference r = *ps.begin();
                v = params_const_view::value_type(r);
            }
            BOOST_TEST_EQ(v.key, "a");
            BOOST_TEST_EQ(v.value, "1");
            BOOST_TEST_EQ(v.has_value, true);
        }

        // separate references
        {
            url_view u("/?a=1&bb=22&ccc=333&dddd=4444#f");
            params_const_view pv = u.params();
            auto it0 = pv.begin();
            auto const& v0 = *it0;
            auto it1 = it0;
            ++it1;
            auto const& v1 = *it1;
            auto it2 = it1;
            ++it2;
            auto const& v2 = *it2;
            BOOST_TEST_EQ(it0->key, "a");
            BOOST_TEST_EQ(it1->key, "bb");
            BOOST_TEST_EQ(it2->key, "ccc");
            BOOST_TEST_EQ(v0.key, "a");
            BOOST_TEST_EQ(v1.key, "bb");
            BOOST_TEST_EQ(v2.key, "ccc");
        }
    }

    void
    testEncoding()
    {
        /*
        {
            params_const_view u = parse_query_params(
                "a=1&b=2+2&c=%61%70%70%6c%65").value();
            BOOST_TEST_EQ(u.at("b"), "2 2");
            BOOST_TEST_EQ(u.at("c"), "apple");
        }
        */
    }

    void
    testRange()
    {
        // issue 129
        // empty range iterates once
        {
            url_view u = parse_uri(
                "http://example.com/index.htm").value();
            auto const r = u.params();
            BOOST_TEST(
                r.begin() == r.end());
        }
    }

    void
    testEmpty()
    {
        // issue 129
        // empty range iterates once
        {
            url_view u( "x:?" );
            auto const v = u.params();
            auto it = v.begin();
            auto t = *it++;
            BOOST_TEST(it == v.end());
            BOOST_TEST(t.has_value == false);
            BOOST_TEST(t.key.empty());
            BOOST_TEST(t.value.empty());
        }
    }

    void
    testJavadocs()
    {
        // class
        {
    url u( "?first=John&last=Doe" );

    params_const_view p = u.params();
    (void)p;
        }
    }

    void
    run()
    {
        testCapacity();
        testLookup();
        testIterators();
        testEncoding();
        testRange();
        testEmpty();
        testJavadocs();
    }
};

TEST_SUITE(
    params_base_test,
    "boost.url.params_base");

} // urls
} // boost

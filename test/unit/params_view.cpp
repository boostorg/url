//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/params_view.hpp>

#include <boost/url/url_view.hpp>
#include <boost/url/static_pool.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class params_view_test
{
public:
    using pool_t = static_pool<4096>;
    pool_t pa;

    void
    testMembers()
    {
        // operator=(params_view const&)
        {
            url_view u1;
            url_view u2;
            params_view p1 = u1.params();
            params_view p2 = u2.params();
            p2 = p1;
            BOOST_TEST(p1.begin() == p2.begin());
        }
    }

    void
    testElements()
    {
        // at(string_view)
        // at(Key)
        {
            url_view u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_view p = u.params(pa.allocator());
            BOOST_TEST(p.at("k0") == "0");
            BOOST_TEST(p.at("k1") == "1");
            BOOST_TEST(p.at("k2") == "");
            BOOST_TEST_THROWS(p.at("k3") == "0",
                std::out_of_range);
            BOOST_TEST(p.at("k4") == "4444");
            BOOST_TEST_THROWS(p.at("k5"),
                std::out_of_range);
        }
    }

    void
    testCapacity()
    {
        // empty
        // size
        {
            url_view u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_view p = u.params(pa.allocator());
            BOOST_TEST(! p.empty());
            BOOST_TEST(p.size() == 5);
        }
        {
            url_view u;
            params_view p = u.params(pa.allocator());
            BOOST_TEST(p.empty());
            BOOST_TEST(p.size() == 0);
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
            params_view p = u.params(pa.allocator());
            BOOST_TEST(p.count("a") == 1);
            BOOST_TEST(p.count("b") == 1);
            BOOST_TEST(p.count("c") == 3);
            BOOST_TEST(p.count("d") == 2);
            BOOST_TEST(p.count("e") == 1);
            BOOST_TEST(p.count("f") == 1);
            BOOST_TEST(p.count("g") == 0);

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
            params_view p = u.params(pa.allocator());
            auto it = p.begin();
            BOOST_TEST((*it).key == "a");
            BOOST_TEST((*++it).key == "bb");
            BOOST_TEST((*it++).key == "bb");
            BOOST_TEST((*it).key == "ccc");
            auto it2 = p.end();
            BOOST_TEST(it2 == p.end());
            BOOST_TEST(it != it2);
        }

        // operator*
        {
            url_view u = parse_uri_reference(
                "/?&x&y=&z=3#f").value();
            params_view p = u.params();
            BOOST_TEST(p.size() == 4);
            auto it = p.begin();

            params_view::value_type v;

            v = *it++;
            BOOST_TEST(v.key == "");
            BOOST_TEST(v.value == "");
            BOOST_TEST(v.has_value == false);

            v = *it++;
            BOOST_TEST(v.key == "x");
            BOOST_TEST(v.value == "");
            BOOST_TEST(v.has_value == false);

            v = *it++;
            BOOST_TEST(v.key == "y");
            BOOST_TEST(v.value == "");
            BOOST_TEST(v.has_value == true);

            v = *it++;
            BOOST_TEST(v.key == "z");
            BOOST_TEST(v.value == "3");
            BOOST_TEST(v.has_value == true);
        }
    }

    void
    testEncoding()
    {
        {
            params_view u = parse_query_params(
                "a=1&b=2+2&c=%61%70%70%6c%65").value().decoded();
            BOOST_TEST(u.at("b") == "2 2");
            BOOST_TEST(u.at("c") == "apple");
        }
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
    run()
    {
        testMembers();
        testElements();
        testCapacity();
        testLookup();
        testIterators();
        testEncoding();
        testRange();
    }
};

TEST_SUITE(
    params_view_test,
    "boost.url.params_view");

} // urls
} // boost

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
#include <boost/url/params_encoded_view.hpp>

#include <boost/url/url_view.hpp>
#include <algorithm>
#include <initializer_list>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class params_encoded_view_test
{
public:
    using T = params_encoded_view;
    struct V
    {
        string_view k;
        string_view v;
        bool bv = false;

        V() = default;

        V(string_view k_)
            : k(k_)
        {
        }

        V(  string_view k_,
            string_view v_)
            : k(k_)
            , v(v_)
            , bv(true)
        {
        }

        friend
        bool
        operator==(
            V const& t0,
            query_param_view const& t1) noexcept
        {
            return
                t0.k == t1.key &&
                t0.bv == t1.has_value &&
                ( ! t0.bv || t0.v == t1.value );
        }

        friend
        bool
        operator==(
            query_param_view const& t1,
            V const& t0) noexcept
        {
            return operator==(t0, t1);
        }
    };

    // ensure string s parses to
    // the range indicated in init.
    static
    void
    check(
        string_view s,
        std::initializer_list<V> init)
    {
        auto rv = parse_query_params(s);
        if(! BOOST_TEST(rv.has_value()))
            return;
        if(! BOOST_TEST_EQ(
            rv->size(), init.size()))
            return;
        BOOST_TEST(std::equal(
            rv->begin(),
            rv->end(),
            init.begin()));
    }

    void
    testMembers()
    {
        // params_encoded_view()
        {
            params_encoded_view v;
            BOOST_TEST(v.empty());
            BOOST_TEST_EQ(v.size(), 0U);
            BOOST_TEST_EQ(
                std::distance(
                    v.begin(), v.end()), 0);
        }

        // operator=(T const&)
        {
            url_view u1;
            url_view u2;
            T p1 = u1.encoded_params();
            T p2 = u2.encoded_params();
            p2 = p1;
            BOOST_TEST_EQ(p1.begin(), p2.begin());
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
            T p = u.encoded_params();
            BOOST_TEST_EQ(p.at("k0"), "0");
            BOOST_TEST_EQ(p.at("k1"), "1");
            BOOST_TEST_EQ(p.at("k2"), "");
            BOOST_TEST_THROWS(p.at("k3") == "0",
                std::out_of_range);
            BOOST_TEST_EQ(p.at("k4"), "4444");
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
            T p = u.encoded_params();
            BOOST_TEST(! p.empty());
            BOOST_TEST_EQ(p.size(), 5u);
        }
        {
            url_view u;
            T p = u.encoded_params();
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
            T p = u.encoded_params();
            BOOST_TEST_EQ(p.count("a"), 1u);
            BOOST_TEST_EQ(p.count("%62"), 1u); // pct-encoded
            BOOST_TEST_EQ(p.count("c"), 3u);
            BOOST_TEST_EQ(p.count("d"), 2u);
            BOOST_TEST_EQ(p.count("e"), 1u);
            BOOST_TEST_EQ(p.count("f"), 1u);
            BOOST_TEST_EQ(p.count("g"), 0u);

            BOOST_TEST(p.find("%62") ==
                std::next(p.begin()));
            BOOST_TEST(p.find(
                std::next(p.begin(), 6), "d") ==
                std::next(p.begin(), 7));

            BOOST_TEST(p.contains("a"));
            BOOST_TEST(p.contains("b"));
            BOOST_TEST(p.contains("%62"));
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
            T p = u.encoded_params();
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
            T p =
                u.encoded_params();
            BOOST_TEST_EQ(p.size(), 4u);
            auto it = p.begin();

            {
                auto v = *it++;
                BOOST_TEST_EQ(v.key, "");
                BOOST_TEST_EQ(v.value, "");
                BOOST_TEST_EQ(v.has_value, false);
            }

            {
                auto v = *it++;
                BOOST_TEST_EQ(v.key, "x");
                BOOST_TEST_EQ(v.value, "");
                BOOST_TEST_EQ(v.has_value, false);
            }

            {
                auto v = *it++;
                BOOST_TEST_EQ(v.key, "y");
                BOOST_TEST_EQ(v.value, "");
                BOOST_TEST_EQ(v.has_value, true);
            }

            {
                auto v = *it++;
                BOOST_TEST_EQ(v.key, "z");
                BOOST_TEST_EQ(v.value, "3");
                BOOST_TEST_EQ(v.has_value, true);
            }
        }

        // value_type outlives reference
        {
            url_view u = parse_uri_reference(
                        "/?a=1&bb=22&ccc=333&dddd=4444#f").value();
            T::value_type v;
            {
                T ps = u.encoded_params();
                T::reference r = *ps.begin();
                v = T::value_type(r);
            }
            BOOST_TEST_EQ(v.key, "a");
            BOOST_TEST_EQ(v.value, "1");
            BOOST_TEST_EQ(v.has_value, true);
        }
    }

    void
    testEncoding()
    {
        // parse_query_params(string_view)
        {
            params_view u = parse_query_params(
                "a=1&b=2+2&c=%61%70%70%6c%65").value();
            BOOST_TEST_EQ(u.at("b"), "2 2");
            BOOST_TEST_EQ(u.at("c"), "apple");

            BOOST_TEST_THROWS(parse_query_params("#a").value(),
                std::exception);
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
            auto const r = u.encoded_params();
            BOOST_TEST(
                r.begin() == r.end());
        }
    }

    void
    testParse()
    {
        //check( "k", { {} });
        check( "k", { {"k"} });
        check( "k=", { {"k",""} });
        check( "k=v", { {"k","v"} });
        check( "u&", { {"u"}, {} });
        check( "u&k", { {"u"}, {"k"} });
        check( "u&k=", { {"u"}, {"k",""} });
        check( "u&k=v", { {"u"}, {"k","v"} });
    }

    void
    testEmpty()
    {
        // issue 129
        // empty range iterates once
        {
            url_view u( "x:?" );
            auto const v = u.encoded_params();
            auto it = v.begin();
            auto t = *it++;
            BOOST_TEST(it == v.end());
            BOOST_TEST(t.has_value == false);
            BOOST_TEST(t.key.empty());
            BOOST_TEST(t.value.empty());
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
        testParse();
        testEmpty();
    }
};

TEST_SUITE(
    params_encoded_view_test,
    "boost.url.params_encoded_view");

} // urls
} // boost

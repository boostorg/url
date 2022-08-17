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
#include <boost/url/params_encoded.hpp>

#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class params_encoded_test
{
public:
    void
    testMembers()
    {
        // operator=(params_encoded const&)
        {
            url u1;
            url u2;
            params_encoded p1 = u1.encoded_params();
            params_encoded p2 = u2.encoded_params();
            p2 = p1;
            BOOST_TEST_EQ(p1.begin(), p2.begin());
        }

        // operator=
        // assign(initializer_list)
        // assign(FwdIt, FwdIt)
        {
            url u = parse_uri_reference(
                "/?x#f").value();
            u.encoded_params() = {
                { "k1", "1", true },
                { "k2", "2", true },
                { "k3", "", true },
                { "k4", "", false },
                { "k5", "55555", true } };
            BOOST_TEST(u.encoded_query() ==
                "k1=1&k2=2&k3=&k4&k5=55555");
            BOOST_TEST(u.string() ==
                "/?k1=1&k2=2&k3=&k4&k5=55555#f");
        }
        {
            url u = parse_uri_reference(
                "/?x#f").value();
            auto ps = u.encoded_params();
            BOOST_TEST_THROWS(
                ps.assign({
                    { "k1", "1#", true }}),
                std::invalid_argument);
            ignore_unused(ps);
        }
        {
            url u = parse_uri_reference(
                "/?x#f").value();
            auto ps = u.encoded_params();
            BOOST_TEST_THROWS(
                ps.assign({
                    { "#k1", "", false }}),
                std::invalid_argument);
            ignore_unused(ps);
        }
        {
            url u = parse_uri_reference("/?x#f").value();
            u.encoded_params() = {};
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST_EQ(u.string(), "/?#f");
        }
    }

    void
    testElements()
    {
        // at
        // operator[]
        // front
        // back
        {
            url u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST_EQ(p.at(0).key, "k0");
            BOOST_TEST_EQ(p.at(0).value, "0");
            BOOST_TEST(p.at(0).has_value);

            BOOST_TEST_EQ(p.at(2).key, "k2");
            BOOST_TEST_EQ(p.at(2).value, "");
            BOOST_TEST(p.at(2).has_value);

            BOOST_TEST_EQ(p.at(3).key, "k3");
            BOOST_TEST_EQ(p.at(3).has_value, false);

            BOOST_TEST_EQ(p.at(4).key, "k4");
            BOOST_TEST_EQ(p.at(4).has_value, true);

            BOOST_TEST_THROWS(
                p.at(5), std::out_of_range);
        }

        // at(string_view)
        // at(Key)
        {
            url u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
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
            url u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST(! p.empty());
            BOOST_TEST_EQ(p.size(), 5u);
        }
        {
            url u;
            params_encoded p = u.encoded_params();
            BOOST_TEST(p.empty());
            BOOST_TEST_EQ(p.size(), 0u);
        }
    }

    void
    testModifiers()
    {
        // clear
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            p.clear();
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST_EQ(u.string(), "/?#f");
        }

        // insert(iterator, value_type)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(std::next(p.begin()),
                {"k1", "1", true});
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST_EQ((*it).key, "k1");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // insert(iterator, initializer-list)
        // insert(iterator, FwdIt, FwdIt)
        {
            url u = parse_uri_reference(
                "/?k0=0&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(std::next(p.begin()),{
                {"k1", "1", true},
                {"k2", "", true}});
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3#f");
        }

        // replace(iterator, value_type)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                std::next(p.begin(), p.size() - 1),
                {"k2", "", true});
            BOOST_TEST_EQ(it, std::next(p.begin(), p.size() - 1));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // replace(iterator, iterator, FwdIt, FwdIt)
        // replace(iterator, iterator, initializer_list)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                std::next(p.begin()), std::next(p.begin(), 3), {
                    {"a","aa",true},
                    {"b","bbb",true},
                    {"c","ccccc",true}});
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&a=aa&b=bbb&c=ccccc&k3&k4=4444");
            BOOST_TEST(u.string() ==
                "/?k0=0&a=aa&b=bbb&c=ccccc&k3&k4=4444#f");
        }

        // remove_value(iterator)
        {
            url u = parse_uri_reference(
                "/?k0=0&k%31=1&k2=#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST_EQ(p.at(1).key, "k%31");
            auto it = p.remove_value(std::next(p.begin()));
            BOOST_TEST_EQ(u.encoded_query(), "k0=0&k%31&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k%31&k2=#f");
            BOOST_TEST_EQ(it, std::next(p.begin()));
        }

        // replace_value(iterator, string_view)
        {
            url u = parse_uri_reference(
                        "/?k0=0&k%31=0&k2=#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace_value(
                std::next(p.begin()),
                "1");
            BOOST_TEST(it == std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                       "k0=0&k%31=1&k2=");
            BOOST_TEST(u.string() ==
                       "/?k0=0&k%31=1&k2=#f");
        }

        // replace(iterator, string_view, string_view)
        // replace(iterator, Key, Value)
        {
            url u = parse_uri_reference(
                "/?k0=0&k%31=1&k2=#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                std::next(p.begin()),
                "k1", "1");
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // replace(iterator, string_view)
        // replace(iterator, Key)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                std::next(p.begin(), 2),
                "hello_world");
            BOOST_TEST_EQ(it, std::next(p.begin(), 2));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&hello_world&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&hello_world&k3#f");
        }

        // insert(iterator, string_view, string_view)
        // insert(iterator, Key, Value)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(
                std::next(p.begin()), "k1", "1");
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3#f");
        }

        // insert(iterator, string_view)
        // insert(iterator, Key)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(
                std::next(p.begin()), "k1");
            BOOST_TEST_EQ(it, std::next(p.begin()));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1&k2=&k3#f");
        }

        // erase(iterator)
        // erase(iterator, iterator)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            p.erase(std::next(p.begin(), 2));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k3&k4=4444");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k3&k4=4444#f");
            p.erase(
                std::next(p.begin()), std::next(p.begin(), 3));
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k4=4444");
            BOOST_TEST(u.string() ==
                "/?k0=0&k4=4444#f");
        }

        // erase(string_view)
        // erase(Key)
        {
            url u = parse_uri_reference(
                "/?a=1&%62=2&c=3&c=4&c=5&d=6&e=7&d=8&f=9#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST_EQ(p.erase("c"), 3u);
            BOOST_TEST(u.encoded_query() ==
                "a=1&%62=2&d=6&e=7&d=8&f=9");
            BOOST_TEST(u.string() ==
                "/?a=1&%62=2&d=6&e=7&d=8&f=9#f");
            BOOST_TEST_EQ(p.erase("b"), 1u);
            BOOST_TEST(u.encoded_query() ==
                "a=1&d=6&e=7&d=8&f=9");
            BOOST_TEST(u.string() ==
                "/?a=1&d=6&e=7&d=8&f=9#f");
            BOOST_TEST_EQ(p.erase("d"), 2u);
            BOOST_TEST(u.encoded_query() ==
                "a=1&e=7&f=9");
            BOOST_TEST(u.string() ==
                "/?a=1&e=7&f=9#f");
            BOOST_TEST_EQ(p.erase("g"), 0u);
        }

        // append(Key, Value)
        // append(Key)
        {
            url u = parse_uri_reference("/#f").value();
            params_encoded p = u.encoded_params();
            p.append("k0", "0");
            BOOST_TEST_EQ(u.encoded_query(), "k0=0");
            BOOST_TEST_EQ(u.string(), "/?k0=0#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 1u);
            p.append("k1", "1");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 2u);
            p.append("k2", "");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 3u);
            p.append("k3");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 4u);
            p.append("", "4444");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3&=4444");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3&=4444#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 5u);
        }

        // push_back(value_type)
        // pop_back()
        {
            url u = parse_uri_reference("/#f").value();
            params_encoded p = u.encoded_params();

            p.push_back({"k0", "0", true});
            BOOST_TEST_EQ(u.encoded_query(), "k0=0");
            BOOST_TEST_EQ(u.string(), "/?k0=0#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 1u);

            p.push_back({"k1", "1", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 2u);

            p.push_back({"k2", "", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 3u);

            p.push_back({"k3", "", false});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 4u);

            p.push_back({"", "4444", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3&=4444");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3&=4444#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 5u);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 4u);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 3u);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.string() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 2u);

            p.pop_back();
            BOOST_TEST_EQ(u.encoded_query(), "k0=0");
            BOOST_TEST_EQ(u.string(), "/?k0=0#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 1u);

            p.pop_back();
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST_EQ(u.string(), "/?#f");
            BOOST_TEST_EQ(u.encoded_params().size(), 0u);
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
            url u = parse_uri_reference(
                "/?a=1&%62=2&c=3&c=4&c=5&d=6&e=7&d=8&f=9#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST_EQ(p.count("a"), 1u);
            BOOST_TEST_EQ(p.count("b"), 1u);
            BOOST_TEST_EQ(p.count("c"), 3u);
            BOOST_TEST_EQ(p.count("d"), 2u);
            BOOST_TEST_EQ(p.count("e"), 1u);
            BOOST_TEST_EQ(p.count("f"), 1u);
            BOOST_TEST_EQ(p.count("g"), 0u);

            BOOST_TEST_EQ(p.find("b"), std::next(p.begin()));
            BOOST_TEST(p.find(std::next(p.begin(), 6), "d") ==
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
        {
            url u = parse_uri_reference(
                "/?a=1&bb=22&ccc=333&dddd=4444#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.begin();
            BOOST_TEST_EQ((*it).key, "a");
            BOOST_TEST_EQ((*++it).key, "bb");
            BOOST_TEST_EQ((*it++).key, "bb");
            BOOST_TEST_EQ((*it).key, "ccc");
            it = p.begin();
            BOOST_TEST_EQ((*it).key, "a");
            auto it2 = p.end();
            BOOST_TEST_EQ(it, p.begin());
            BOOST_TEST_NE(it, it2);
            BOOST_TEST_EQ((*(++it)).key, "bb");
            BOOST_TEST_EQ((*(std::next(it))).value, "333");
            BOOST_TEST_EQ((*(std::next(it))).value, "333");
            BOOST_TEST_EQ((*it).value, "22");
            BOOST_TEST_EQ((*std::next(it)).value, "333");
        }

        // operator*
        {
            url u = parse_uri_reference(
                "/?a&b=&c=3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.begin();
            BOOST_TEST_EQ((*it).has_value, false);
            BOOST_TEST_EQ((*++it).has_value, true);
            BOOST_TEST_EQ((*++it).value, "3");
        }

        // value_type outlives reference
        {
            url u = parse_uri_reference(
                        "/?a=1&bb=22&ccc=333&dddd=4444#f").value();
            params_encoded::value_type v;
            {
                params_encoded ps = u.encoded_params();
                params_encoded::reference r = *ps.begin();
                v = params_encoded::value_type(r);
            }
            BOOST_TEST_EQ(v.key, "a");
            BOOST_TEST_EQ(v.value, "1");
            BOOST_TEST_EQ(v.has_value, true);
        }
    }

    void
    testRange()
    {
        // issue 129
        // empty range iterates once
        {
            url u = parse_uri(
                "http://example.com/index.htm").value();
            auto const r = u.encoded_params();
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
            url u( "x:?" );
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
        testModifiers();
        testLookup();
        testIterators();
        testRange();
        testEmpty();
    }
};

TEST_SUITE(
    params_encoded_test,
    "boost.url.params_encoded");

} // urls
} // boost

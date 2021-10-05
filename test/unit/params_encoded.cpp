//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/params_encoded.hpp>

#include <boost/url/url.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class params_encoded_test
{
public:
    void
    testMembers()
    {
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
            BOOST_TEST(u.encoded_url() ==
                "/?k1=1&k2=2&k3=&k4&k5=55555#f");
        }
        {
            url u = parse_uri_reference("/?x#f").value();
            u.encoded_params() = {};
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.encoded_url() == "/#f");
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
            BOOST_TEST(p.at(0).key == "k0");
            BOOST_TEST(p.at(0).value == "0");
            BOOST_TEST(p.at(0).has_value);
            BOOST_TEST(p.at(2).key == "k2");
            BOOST_TEST(p.at(2).value == "");
            BOOST_TEST(p.at(2).has_value);
            BOOST_TEST(p.at(3).key == "k3");
            BOOST_TEST(p.at(3).has_value == false);
            BOOST_TEST_THROWS(
                p.at(5), std::out_of_range);

            BOOST_TEST(p.front().key == "k0");
            BOOST_TEST(p.front().value == "0");
            BOOST_TEST(p.front().has_value);

            BOOST_TEST(p.back().key == "k4");
            BOOST_TEST(p.back().value == "4444");
            BOOST_TEST(p.back().has_value);
        }

        // at(string_view)
        // at(Key)
        {
            url u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
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
            url u = parse_uri_reference(
                "?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST(! p.empty());
            BOOST_TEST(p.size() == 5);
        }
        {
            url u;
            params_encoded p = u.encoded_params();
            BOOST_TEST(p.empty());
            BOOST_TEST(p.size() == 0);
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
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.encoded_url() == "/#f");
        }

        // insert(iterator, value_type)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(p.begin() + 1,
                {"k1", "1", true});
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST((*it).key == "k1");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // insert(iterator, initializer-list)
        // insert(iterator, FwdIt, FwdIt)
        {
            url u = parse_uri_reference(
                "/?k0=0&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.insert(p.begin() + 1,{
                {"k1", "1", true},
                {"k2", "", true}});
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3#f");
        }

        // replace(iterator, value_type)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                p.end() - 1,
                {"k2", "", true});
            BOOST_TEST(it == p.end() - 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // replace(iterator, iterator, FwdIt, FwdIt)
        // replace(iterator, iterator, initializer_list)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.replace(
                p.begin() + 1, p.begin() + 3, {
                    {"a","aa",true},
                    {"b","bbb",true},
                    {"c","ccccc",true}});
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&a=aa&b=bbb&c=ccccc&k3&k4=4444");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&a=aa&b=bbb&c=ccccc&k3&k4=4444#f");
        }

        // remove_value(iterator)
        {
            url u = parse_uri_reference(
                "/?k0=0&k%31=1&k2=#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST(p.at(1).key == "k%31");
            auto it = p.remove_value(p.begin() + 1);
            BOOST_TEST(u.encoded_query() == "k0=0&k%31&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k%31&k2=#f");
            BOOST_TEST(it == p.begin() + 1);
        }

        // replace_value(iterator, string_view)
        // replace_value(iterator, Value)
        {
            // VFALCO TODO
        }

        // emplace_at(iterator, string_view, string_view)
        // emplace_at(iterator, Key, Value)
        {
            url u = parse_uri_reference(
                "/?k0=0&k%31=1&k2=#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.emplace_at(
                p.begin() + 1,
                "k1", "1");
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
        }

        // emplace_at(iterator, string_view)
        // emplace_at(iterator, Key)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.emplace_at(
                p.begin() + 2,
                "hello_world");
            BOOST_TEST(it == p.begin() + 2);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&hello_world&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&hello_world&k3#f");
        }

        // emplace_before(iterator, string_view, string_view)
        // emplace_before(iterator, Key, Value)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.emplace_before(
                p.begin() + 1, "k1", "1");
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3#f");
        }

        // emplace_before(iterator, string_view)
        // emplace_before(iterator, Key)
        {
            url u = parse_uri_reference(
                "/?k0=0&k2=&k3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.emplace_before(
                p.begin() + 1, "k1");
            BOOST_TEST(it == p.begin() + 1);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1&k2=&k3#f");
        }

        // erase(iterator)
        // erase(iterator, iterator)
        {
            url u = parse_uri_reference(
                "/?k0=0&k1=1&k2=&k3&k4=4444#f").value();
            params_encoded p = u.encoded_params();
            p.erase(p.begin() + 2);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k3&k4=4444");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k3&k4=4444#f");
            p.erase(
                p.begin() + 1, p.begin() + 3);
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k4=4444");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k4=4444#f");
        }

        // erase(string_view)
        // erase(Key)
        {
            url u = parse_uri_reference(
                "/?a=1&%62=2&c=3&c=4&c=5&d=6&e=7&d=8&f=9#f").value();
            params_encoded p = u.encoded_params();
            BOOST_TEST(p.erase("c") == 3);
            BOOST_TEST(u.encoded_query() ==
                "a=1&%62=2&d=6&e=7&d=8&f=9");
            BOOST_TEST(u.encoded_url() ==
                "/?a=1&%62=2&d=6&e=7&d=8&f=9#f");
            BOOST_TEST(p.erase("b") == 1);
            BOOST_TEST(u.encoded_query() ==
                "a=1&d=6&e=7&d=8&f=9");
            BOOST_TEST(u.encoded_url() ==
                "/?a=1&d=6&e=7&d=8&f=9#f");
            BOOST_TEST(p.erase("d") == 2);
            BOOST_TEST(u.encoded_query() ==
                "a=1&e=7&f=9");
            BOOST_TEST(u.encoded_url() ==
                "/?a=1&e=7&f=9#f");
            BOOST_TEST(p.erase("g") == 0);
        }

        // emplace_back(Key, Value)
        // emplace_back(Key)
        {
            url u = parse_uri_reference("/#f").value();
            params_encoded p = u.encoded_params();
            p.emplace_back("k0", "0");
            BOOST_TEST(u.encoded_query() == "k0=0");
            BOOST_TEST(u.encoded_url() == "/?k0=0#f");
            BOOST_TEST(u.param_count() == 1);
            p.emplace_back("k1", "1");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST(u.param_count() == 2);
            p.emplace_back("k2", "");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST(u.param_count() == 3);
            p.emplace_back("k3");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST(u.param_count() == 4);
            p.emplace_back("", "4444");
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3&=4444");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3&=4444#f");
            BOOST_TEST(u.param_count() == 5);
        }

        // push_back(value_type)
        // pop_back()
        {
            url u = parse_uri_reference("/#f").value();
            params_encoded p = u.encoded_params();

            p.push_back({"k0", "0", true});
            BOOST_TEST(u.encoded_query() == "k0=0");
            BOOST_TEST(u.encoded_url() == "/?k0=0#f");
            BOOST_TEST(u.param_count() == 1);

            p.push_back({"k1", "1", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST(u.param_count() == 2);

            p.push_back({"k2", "", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST(u.param_count() == 3);

            p.push_back({"k3", "", false});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST(u.param_count() == 4);

            p.push_back({"", "4444", true});
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3&=4444");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3&=4444#f");
            BOOST_TEST(u.param_count() == 5);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=&k3");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=&k3#f");
            BOOST_TEST(u.param_count() == 4);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1&k2=");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1&k2=#f");
            BOOST_TEST(u.param_count() == 3);

            p.pop_back();
            BOOST_TEST(u.encoded_query() ==
                "k0=0&k1=1");
            BOOST_TEST(u.encoded_url() ==
                "/?k0=0&k1=1#f");
            BOOST_TEST(u.param_count() == 2);

            p.pop_back();
            BOOST_TEST(u.encoded_query() == "k0=0");
            BOOST_TEST(u.encoded_url() == "/?k0=0#f");
            BOOST_TEST(u.param_count() == 1);

            p.pop_back();
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.encoded_url() == "/#f");
            BOOST_TEST(u.param_count() == 0);
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
            BOOST_TEST(p.count("a") == 1);
            BOOST_TEST(p.count("b") == 1);
            BOOST_TEST(p.count("c") == 3);
            BOOST_TEST(p.count("d") == 2);
            BOOST_TEST(p.count("e") == 1);
            BOOST_TEST(p.count("f") == 1);
            BOOST_TEST(p.count("g") == 0);

            BOOST_TEST(p.find("b") == p.begin() + 1);
            BOOST_TEST(p.find(p.begin() + 6, "d") ==
                p.begin() + 7);

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
            BOOST_TEST((*it).key == "a");
            BOOST_TEST((*++it).key == "bb");
            BOOST_TEST((*it++).key == "bb");
            BOOST_TEST((*it).key == "ccc");
            BOOST_TEST((*--it).key == "bb");
            BOOST_TEST((*it--).key == "bb");
            BOOST_TEST((*it).key == "a");
            auto it2 = p.end();
            BOOST_TEST(it == p.begin());
            BOOST_TEST(it != it2);
            BOOST_TEST((*(it += 1)).key == "bb");
            BOOST_TEST((*(it + 1)).value == "333");
            BOOST_TEST((*(1 + it)).value == "333");
            BOOST_TEST((*it).value == "22");
            BOOST_TEST((*(it2 -= 1)).value == "4444");
            BOOST_TEST((*(it2 - 1)).value == "333");
            BOOST_TEST((*it2).value == "4444");
            BOOST_TEST(it2 - it == 2);
            BOOST_TEST(it[1].value == "333");

            BOOST_TEST(it < it2);
            BOOST_TEST(it <= it2);
            BOOST_TEST(it2 > it);
            BOOST_TEST(it2 >= it);
        }

        // operator*
        {
            url u = parse_uri_reference(
                "/?a&b=&c=3#f").value();
            params_encoded p = u.encoded_params();
            auto it = p.begin();
            BOOST_TEST((*it).has_value == false);
            BOOST_TEST((*++it).has_value == true);
            BOOST_TEST((*++it).value == "3");
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
    }
};

TEST_SUITE(
    params_encoded_test,
    "boost.url.params_encoded");

} // urls
} // boost

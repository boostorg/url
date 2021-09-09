//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/query_params_view.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class query_params_view_test
{
public:
    using qpv_t =
        query_params_view;

    struct T
    {
        std::string key;
        std::string value;
        std::string encoded_key;
        std::string encoded_value;
        bool has_value;

        T(
            std::string key_,
            std::string value_,
            std::string encoded_key_,
            std::string encoded_value_,
            bool has_value_)
            : key(key_)
            , value(value_)
            , encoded_key(encoded_key_)
            , encoded_value(encoded_value_)
            , has_value(has_value_)
        {
        }

        T(qpv_t::value_type const& v)
            : key(v.key())
            , value(v.value())
            , encoded_key(
                v.encoded_key())
            , encoded_value(
                v.encoded_value())
            , has_value(v.has_value())
        {
        }

        friend
        bool
        operator==(
            T const& t1,
            T const& t2) noexcept
        {
            return
                t1.encoded_key == t2.encoded_key &&
                t1.encoded_value == t2.encoded_value &&
                t1.key == t2.key &&
                t1.value == t2.value &&
                t1.has_value == t2.has_value;
        }
    };

    void
    bad(string_view s)
    {
        qpv_t p;
        BOOST_TEST_THROWS(p =
            parse_query_params(s),
            std::exception);
        BOOST_TEST(p.empty());
        BOOST_TEST(
            p.begin() == p.end());
    }

    void
    check(
        string_view s,
        std::vector<T> const& v0)
    {
        qpv_t p;
        BOOST_TEST_NO_THROW(
            p = parse_query_params(s));
        {
            std::vector<T> v1;
            std::copy(
                p.begin(),
                p.end(),
                std::back_inserter(v1));
            BOOST_TEST(v0 == v1);
        }
    }

    void
    testIterator()
    {
        BOOST_TEST(
            qpv_t::iterator() ==
            qpv_t::iterator());
        qpv_t qp;
        BOOST_TEST(qp.begin() !=
            qpv_t::iterator());
        BOOST_TEST(qp.end() !=
            qpv_t::iterator());
        BOOST_TEST(
            qp.begin() == qp.end());

        auto it = qp.begin();
        it = qp.end();
        BOOST_TEST(it == qp.begin());
    }

    void
    testParse()
    {
        check("",
            {
            });

        check("a=1&b&c=&d=%34",
            {
            { "a", "1", "a", "1", true },
            { "b", "", "b", "", false },
            { "c", "", "c", "", true },
            { "d", "4", "d", "%34", true }
            });

        check("&",
            {
            { "", "", "", "", false },
            { "", "", "", "", false }
            });

        check("x&",
            {
            { "x", "", "x", "", false },
            { "", "", "", "", false }
            });

        check("&x=",
            {
            { "", "", "", "", false },
            { "x", "", "x", "", true }
            });

        bad("#");
        bad("%2g");
        bad("a=%2g");
        bad("a=#");
        bad("&#");
        bad("&%2g");
        bad("&a=%2g");
        bad("&a=#");
    }

    void
    testMembers()
    {
        {
            qpv_t p;
            BOOST_TEST_NO_THROW(
                p = parse_query_params(
                 "a=1"
                "&b=2"
                "&b=3"
                "&c=4"
                "&b=5"
                "&d=6"
                "&%65=7" // 'e'
                "&f=%38" // '8'
            ));
            BOOST_TEST(! p.empty());
            BOOST_TEST(p.size() == 8);
            BOOST_TEST(p.contains("a"));
            BOOST_TEST(p.count("b") == 3);
            BOOST_TEST(p.find("z") == p.end());
            qpv_t::iterator it;
            it = p.find("b");
            BOOST_TEST(it->value() == "2");
            it = p.find(it, "b");
            BOOST_TEST(it->value() == "3");
            it = p.find(it, "b");
            BOOST_TEST(it->value() == "5");
            it = p.find(it, "b");
            BOOST_TEST(it == p.end());
            BOOST_TEST(p["f"] == "8");
            BOOST_TEST_THROWS(p.at("y"),
                std::exception);
        }
    }

    void
    run()
    {
        testIterator();
        testParse();
        testMembers();
    }
};

TEST_SUITE(
    query_params_view_test,
    "boost.url.query_params_view");

} // urls
} // boost

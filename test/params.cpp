//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/params.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/url/value.hpp>

#include "test_suite.hpp"

#include <map>

namespace boost {
namespace url {

class params_test
{
public:
    //------------------------------------------------------

    test_suite::log_type log;

    void
    dump(params const& p)
    {
        for(auto const& e : p)
        {
            log <<
                "\"" << e.key() <<
                "\" = \"" << e.value() << "\"\n";
        }
    }

    void
    testParams()
    {
#if 0
        value u("?x=a&y=b");
        dump(u.params());

        auto qp = u.params();
        std::map<std::string, std::string> m(
            qp.begin(), qp.end());

        for(auto it = qp.begin();
            it != qp.end(); ++it)
        {
            log << it->key() << "\n";
        }
#endif
        {
            params qp;
            BOOST_TEST(qp.empty());
            BOOST_TEST(qp.size() == 0);
        }
        {
            value u("?x=1&y=2&y=3&z");
            auto qp = u.params();
            BOOST_TEST(! qp.empty());
            BOOST_TEST(qp.size() == 4);
            BOOST_TEST(qp.begin() != qp.end());
            BOOST_TEST(qp.end() == qp.end());
            BOOST_TEST(qp.contains("x"));
            BOOST_TEST(qp.contains("y"));
            BOOST_TEST(! qp.contains("a"));
            BOOST_TEST(qp.count("x") == 1);
            BOOST_TEST(qp.count("y") == 2);
            BOOST_TEST(qp.count("a") == 0);
            BOOST_TEST(qp.find("x")->encoded_value() == "1");
            BOOST_TEST(qp.find("y")->encoded_value() == "2");
            BOOST_TEST(qp.find("a") == qp.end());
            BOOST_TEST(qp[0].encoded_key() == "x");
            BOOST_TEST(qp[1].encoded_key() == "y");
            BOOST_TEST(qp[2].encoded_key() == "y");
            BOOST_TEST(qp[3].encoded_key() == "z");
            BOOST_TEST(qp[0].encoded_value() == "1");
            BOOST_TEST(qp[1].encoded_value() == "2");
            BOOST_TEST(qp[2].encoded_value() == "3");
            BOOST_TEST(qp[3].encoded_value() == "");
            BOOST_TEST(qp.at(0).encoded_key() == "x");
            BOOST_TEST(qp.at(1).encoded_key() == "y");
            BOOST_TEST(qp.at(2).encoded_key() == "y");
            BOOST_TEST(qp.at(3).encoded_key() == "z");
            BOOST_TEST(qp.at(0).encoded_value() == "1");
            BOOST_TEST(qp.at(1).encoded_value() == "2");
            BOOST_TEST(qp.at(2).encoded_value() == "3");
            BOOST_TEST(qp.at(3).encoded_value() == "");
            BOOST_TEST(qp["x"] == "1");
            BOOST_TEST(qp["y"] == "2");
            BOOST_TEST(qp["a"] == "");
            BOOST_TEST(qp.at("x") == "1");
            BOOST_TEST(qp.at("y") == "2");

            BOOST_TEST_THROWS(
                qp.at("a"),
                std::out_of_range);
            BOOST_TEST_THROWS(
                qp.at(900),
                std::out_of_range);

            BOOST_TEST(qp[0].key() == "x");

            static_pool<4000> sp;
            BOOST_TEST(qp[0].key(sp.allocator()) == "x");
            BOOST_TEST(qp[1].key(sp.allocator()) == "y");
            BOOST_TEST(qp[2].key(sp.allocator()) == "y");
            BOOST_TEST(qp[3].key(sp.allocator()) == "z");
            BOOST_TEST(qp[0].value(sp.allocator()) == "1");
            BOOST_TEST(qp[1].value(sp.allocator()) == "2");
            BOOST_TEST(qp[2].value(sp.allocator()) == "3");
            BOOST_TEST(qp[3].value(sp.allocator()) == "");

            auto it = qp.begin();
            BOOST_TEST(it->encoded_key() == "x");
            ++it;
            BOOST_TEST((*it++).encoded_key() == "y");
            ++it;
            BOOST_TEST((*it--).encoded_key() == "z");
            --it;
            BOOST_TEST((*--it).encoded_key() == "x");
        }
    }

    void
    run()
    {
        testParams();
    }
};

TEST_SUITE(params_test, "boost.url.params");

} // url
} // boost

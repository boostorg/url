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
#include <map>
#include <utility>

namespace boost {
namespace urls {

class query_params_view_test
{
public:
    void
    testIterator()
    {
        BOOST_TEST(
            query_params_view::iterator() ==
            query_params_view::iterator());
        query_params_view qp;
        BOOST_TEST(qp.begin() !=
            query_params_view::iterator());
        BOOST_TEST(qp.end() !=
            query_params_view::iterator());
        BOOST_TEST(
            qp.begin() == qp.end());

        auto it = qp.begin();
        it = qp.end();
        BOOST_TEST(it == qp.begin());
    }

    void
    testContents()
    {
        query_params_view qp;
        BOOST_TEST_NO_THROW(
            qp = parse_query_params(
                "a=1&b=2&c=3&d=%34"));
        using map_type = std::map<
            std::string, std::string>;
        map_type m(qp.begin(), qp.end());
        auto const match = map_type(
            { { "a", "1" }, { "b", "2" },
              { "c", "3" }, { "d", "4" } });
        BOOST_TEST(match == m);
    }

    void
    run()
    {
        testIterator();
        testContents();
    }
};

TEST_SUITE(
    query_params_view_test,
    "boost.url.query_params_view");

} // urls
} // boost

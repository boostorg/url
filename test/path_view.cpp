//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/path_view.hpp>

#include "test_suite.hpp"
#include <map>
#include <utility>

namespace boost {
namespace urls {

class path_view_test
{
public:
    void
    testIterator()
    {
        BOOST_TEST(
            path_view::iterator() ==
            path_view::iterator());
        path_view p;
        BOOST_TEST(p.begin() !=
            path_view::iterator());
        BOOST_TEST(p.end() !=
            path_view::iterator());
        BOOST_TEST(
            p.begin() == p.end());

        auto it = p.begin();
        it = p.end();
        BOOST_TEST(it == p.begin());
    }

    void
    testContents()
    {
        auto p = parse_path(
            "/");
    }

    void
    run()
    {
        testIterator();
        testContents();
    }
};

TEST_SUITE(
    path_view_test,
    "boost.url.path_view");

} // urls
} // boost

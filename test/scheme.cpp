//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/scheme.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class scheme_test
{
public:
    void
    run()
    {
        BOOST_TEST(is_special("ftp"));
        BOOST_TEST(is_special("file"));
        BOOST_TEST(is_special("http"));
        BOOST_TEST(is_special("https"));
        BOOST_TEST(is_special("ws"));
        BOOST_TEST(is_special("wss"));

        BOOST_TEST(! is_special("gopher"));
        BOOST_TEST(! is_special("magnet"));
        BOOST_TEST(! is_special("mailto"));
    }
};

TEST_SUITE(scheme_test, "boost.url.scheme");

} // urls
} // boost

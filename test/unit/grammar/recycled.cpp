//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/recycled.hpp>

#include "test_suite.hpp"
#include <string>

namespace boost {
namespace urls {
namespace grammar {

struct recycled_test
{
    void
    testPtr()
    {
        {
            recycled_ptr<std::string> sp;
            sp->reserve(1000);
            BOOST_TEST(sp->capacity() >= 1000);
        }
        {
            recycled_ptr<std::string> sp;
            BOOST_TEST(sp->capacity() >= 1000);
        }
    }

    void
    run()
    {
    }
};

TEST_SUITE(
    recycled_test,
    "boost.url.recycled");

} // grammar
} // urls
} // boost

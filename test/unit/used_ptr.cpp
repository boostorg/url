//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/used_ptr.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct used_ptr_test
{
    void
    testUsedPtr()
    {
        {
            used_ptr<std::string> sp;
            sp->reserve(1000);
            BOOST_TEST(sp->capacity() >= 1000);
        }
        {
            used_ptr<std::string> sp;
            BOOST_TEST(sp->capacity() >= 1000);
        }
    }

    void
    run()
    {
    }
};

TEST_SUITE(
    used_ptr_test,
    "boost.url.used_ptr");

} // urls
} // boost

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/const_string.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

#ifdef BOOST_URL_HAS_STRING_VIEW
BOOST_STATIC_ASSERT(is_stringlike<
    std::string_view>::value);
#endif

class string_test
{
public:
    void
    run()
    {
        const_string sv("hello");
        auto sv2 = sv;
        BOOST_TEST(sv2 == sv);
        sv = {};
        BOOST_TEST(sv2 != sv);
    }
};

TEST_SUITE(
    string_test,
    "boost.url.string");

} // urls
} // boost

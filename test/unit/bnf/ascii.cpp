//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/ascii.hpp>

#include <boost/url/string_view.hpp>
#include "test_suite.hpp"

#include <memory>

namespace boost {
namespace urls {

class ascii_test
{
public:
    void
    run()
    {
        string_view s0 =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        string_view s1 =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz";

        auto it0 = s0.data();
        auto it1 = s1.data();
        auto const end = it0 + s0.size();
        BOOST_TEST(s0.size() == s1.size());
        while(it0 != end)
            BOOST_TEST(bnf::ascii_tolower(
                *it0++) == *it1++);
    }
};

TEST_SUITE(
    ascii_test,
    "boost.url.ascii");

} // urls
} // boost

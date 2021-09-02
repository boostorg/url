//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/hexdig.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace rfc {

class hexdig_test
{
public:
    void
    run()
    {
        for(int i = 0;
            i < 256; ++i)
        {
            auto const c = static_cast<char>(i);
            switch(i)
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                BOOST_TEST(is_hexdig(c));
                BOOST_TEST(hex_digit(c) == i - '0');
                break;

            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                BOOST_TEST(is_hexdig(c));
                BOOST_TEST(hex_digit(c) == 10 + i - 'A');
                break;

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                BOOST_TEST(is_hexdig(c));
                BOOST_TEST(hex_digit(c) == 10 + i - 'a');
                break;

            default:
                BOOST_TEST(! is_hexdig(
                    static_cast<char>(i)));
                break;
            }
        }
    }
};

TEST_SUITE(
    hexdig_test,
    "boost.url.hexdig");

} // rfc
} // urls
} // boost

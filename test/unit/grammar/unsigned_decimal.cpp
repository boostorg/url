//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/unsigned_decimal.hpp>

#include <boost/url/grammar/parse.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct unsigned_decimal_test
{
    template<class U>
    void
    bad(string_view s)
    {
        error_code ec;
        unsigned_decimal<U> t;
        parse_string(s, ec, t);
        BOOST_TEST(ec.failed());
    }

    template<class U>
    void
    good(string_view s, U u)
    {
        unsigned_decimal<U> t;
        BOOST_TEST_NO_THROW(
            parse_string(s, t));
        BOOST_TEST_EQ(t.u, u);
        BOOST_TEST_EQ(t.s, s);
    }

    void
    testParse()
    {
        {
            using T = std::uint8_t;

            good("0", T(0));
            good("1", T(1));
            good("9", T(9));
            good("255", T(255));

            bad<T>("00");
            bad<T>("01");
            bad<T>("256");
            bad<T>("300");
        }
        {
            using T = std::uint16_t;

            good("0", T(0));
            good("1", T(1));
            good("99", T(99));
            good("65535", T(65535));

            bad<T>("00");
            bad<T>("01");
            bad<T>("65536");
            bad<T>("70000");
        }
        {
            using T = std::uint32_t;

            good("0", T(0));
            good("1", T(1));
            good("999", T(999));
            good("4294967295", T(4294967295));

            bad<T>("00");
            bad<T>("01");
            bad<T>("4294967296");
            bad<T>("5000000000");
        }
    }

    void
    run()
    {
        testParse();
    }
};

TEST_SUITE(
    unsigned_decimal_test,
    "boost.url.grammar.unsigned_decimal");

} // grammar
} // urls
} // boost

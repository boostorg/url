//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/unsigned_dec_rule.hpp>

#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct unsigned_dec_rule_test
{
    template<class U>
    void
    check(string_view s, U u)
    {
        auto rv = grammar::parse(
            s, unsigned_dec_rule<U>{});
        if(! BOOST_TEST(! rv.has_error()))
            return;
        auto const& t = *rv;
        BOOST_TEST_EQ(t.u, u);
        BOOST_TEST_EQ(t.s, s);
    }

    void
    run()
    {
        // test constexpr
        constexpr auto r =
            unsigned_dec_rule<unsigned short>{};

        {
            using T = std::uint8_t;
            constexpr auto t =
                unsigned_dec_rule<T>{};

            check("0", T(0));
            check("1", T(1));
            check("9", T(9));
            check("255", T(255));

            bad(t, "00");
            bad(t, "01");
            bad(t, "256");
            bad(t, "300");
            bad(t, "2555");
            bad(t, "25555");
        }
        {
            using T = std::uint16_t;
            constexpr auto t =
                unsigned_dec_rule<T>{};

            check("0", T(0));
            check("1", T(1));
            check("99", T(99));
            check("65535", T(65535));

            bad(t, "");
            bad(t, "a");
            bad(t, "00");
            bad(t, "01");
            bad(t, "65536");
            bad(t, "70000");
        }
        {
            using T = std::uint32_t;
            constexpr auto t =
                unsigned_dec_rule<T>{};

            check("0", T(0));
            check("1", T(1));
            check("999", T(999));
            check("4294967295", T(4294967295));

            bad(t, "00");
            bad(t, "01");
            bad(t, "4294967296");
            bad(t, "5000000000");
        }
    }
};

TEST_SUITE(
    unsigned_dec_rule_test,
    "boost.url.grammar.unsigned_dec_rule");

} // grammar
} // urls
} // boost

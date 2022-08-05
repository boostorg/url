//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/unsigned_rule.hpp>

#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token_rule.hpp>

#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct unsigned_rule_test
{
    template<class U>
    void
    check(string_view s, U u)
    {
        auto rv = parse(
            s, unsigned_rule<U>{});
        if(! BOOST_TEST(! rv.has_error()))
            return;
        BOOST_TEST_EQ(*rv, u);
    }

    void
    run()
    {
        // constexpr
        {
            constexpr auto r =
                unsigned_rule<unsigned short>{};
            (void)r;
        }

        // javadoc
        {
            auto rv = parse( "32767", unsigned_rule< unsigned short >{} );

            (void)rv;
        }

        {
            using T = std::uint8_t;
            constexpr auto t =
                unsigned_rule<T>{};

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
                unsigned_rule<T>{};

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
                unsigned_rule<T>{};

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
    unsigned_rule_test,
    "boost.url.grammar.unsigned_rule");

} // grammar
} // urls
} // boost

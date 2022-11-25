//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/experimental/router.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace experimental {

struct router_test
{
    void
    testPatterns()
    {
        auto good =[](
            string_view pattern,
            string_view request)
        {
            router<int> r;
            r.route(pattern, 1);
            using match_res = router<int>::match_results;
            result<match_res> rm = r.match(request);
            if (BOOST_TEST(rm))
            {
                match_res m = *rm;
                BOOST_TEST_EQ(*m, 1);
            }
        };

        auto bad =[](
            string_view pattern,
            string_view request)
        {
            router<int> r;
            r.route(pattern, 1);
            using match_res = router<int>::match_results;
            result<match_res> rm = r.match(request);
            BOOST_TEST_NOT(rm);
        };

        good("user", "user");

        // match all / no type
        good("user/{name}", "user/johndoe");
        good("user/{id}", "user/123");
        good("user/{name:}", "user/johndoe");
        good("user/{id:}", "user/123");

        // types
        // s: string / default type
        good("user/{name:s}", "user/johndoe");
        good("user/{id:s}", "user/123");
        // c: single char
        good("user/{opt:c}", "user/r");
        bad("user/{name:c}", "user/johndoe");
        bad("user/{id:c}", "user/123");
        // d: decimal integer
        good("user/{id:d}", "user/1111011");
        good("user/{id:d}", "user/123");
        bad("user/{name:d}", "user/johndoe");
        // d: octal integer
        good("user/{id:o}", "user/123");
        bad("user/{id:o}", "user/789");
        bad("user/{name:o}", "user/johndoe");
        // x: hex lowercase
        good("user/{id:x}", "user/7b");
        bad("user/{id:x}", "user/7B");
        good("user/{id:#x}", "user/0x7b");
        bad("user/{id:#x}", "user/0X7b");
        bad("user/{id:#x}", "user/0X7B");
        good("user/{id:x}", "user/123");
        bad("user/{name:x}", "user/johndoe");
        // x: hex uppercase
        good("user/{id:X}", "user/7B");
        bad("user/{id:X}", "user/7b");
        good("user/{id:#X}", "user/0X7B");
        bad("user/{id:#X}", "user/0x7B");
        bad("user/{id:#X}", "user/0x7b");
        good("user/{id:X}", "user/123");
        bad("user/{name:X}", "user/johndoe");
        // b: binary lowercase
        good("user/{id:b}", "user/1111011");
        good("user/{id:#b}", "user/0b1111011");
        bad("user/{id:#b}", "user/0B1111011");
        bad("user/{id:b}", "user/123");
        bad("user/{name:b}", "user/johndoe");
        // B: binary uppercase
        good("user/{id:B}", "user/1111011");
        good("user/{id:#B}", "user/0B1111011");
        bad("user/{id:#B}", "user/0b1111011");
        bad("user/{id:B}", "user/123");
        bad("user/{name:B}", "user/johndoe");

        // g: general format
        good("user/{id:g}", "user/123");
        good("user/{id:g}", "user/123.123");
        good("user/{id:g}", "user/123.123e+06");
        good("user/{id:g}", "user/123.123e-06");
        good("user/{id:g}", "user/inf");
        good("user/{id:g}", "user/nan");
        bad("user/{id:g}", "user/0x1a");
        bad("user/{id:g}", "user/0b10");
        bad("user/{name:g}", "user/johndoe");
        // f: fixed point
        good("user/{id:f}", "user/123");
        good("user/{id:f}", "user/123.123");
        bad("user/{id:f}", "user/123.123e+06");
        bad("user/{id:f}", "user/123.123e-06");
        good("user/{id:f}", "user/inf");
        good("user/{id:f}", "user/nan");
        bad("user/{id:f}", "user/0x1a");
        bad("user/{id:f}", "user/0b10");
        bad("user/{name:f}", "user/johndoe");
        // F: fixed point
        good("user/{id:F}", "user/123");
        good("user/{id:F}", "user/123.123");
        bad("user/{id:F}", "user/123.123E+06");
        bad("user/{id:F}", "user/123.123E-06");
        bad("user/{id:F}", "user/inf");
        bad("user/{id:F}", "user/nan");
        good("user/{id:F}", "user/INF");
        good("user/{id:F}", "user/NAN");
        bad("user/{id:F}", "user/0x1a");
        bad("user/{id:F}", "user/0b10");
        bad("user/{name:F}", "user/johndoe");
        // e: exponent notation
        good("user/{id:e}", "user/123");
        good("user/{id:e}", "user/123.123");
        good("user/{id:e}", "user/123.123e+06");
        good("user/{id:e}", "user/123.123e-06");
        bad("user/{id:e}", "user/123.123E+06");
        bad("user/{id:e}", "user/123.123E-06");
        good("user/{id:e}", "user/inf");
        good("user/{id:e}", "user/nan");
        bad("user/{id:e}", "user/0x1a");
        bad("user/{id:e}", "user/0b10");
        bad("user/{name:e}", "user/johndoe");
        // E: exponent notation
        good("user/{id:E}", "user/123");
        good("user/{id:E}", "user/123.123");
        good("user/{id:E}", "user/123.123E+06");
        good("user/{id:E}", "user/123.123E-06");
        bad("user/{id:E}", "user/123.123e+06");
        bad("user/{id:E}", "user/123.123e-06");
        bad("user/{id:E}", "user/inf");
        bad("user/{id:E}", "user/nan");
        good("user/{id:E}", "user/INF");
        good("user/{id:E}", "user/NAN");
        bad("user/{id:E}", "user/0x1a");
        bad("user/{id:E}", "user/0b10");
        bad("user/{name:E}", "user/johndoe");
        // a: hex floating point format
        good("user/{id:a}", "user/0xec7df");
        good("user/{id:a}", "user/0xec7df.ec7df");
        good("user/{id:a}", "user/0xec7df.ec7dfp+06");
        good("user/{id:a}", "user/0xec7df.ec7dfp-06");
        bad("user/{id:a}", "user/0xec7df.ec7dfP+06");
        bad("user/{id:a}", "user/0xec7df.ec7dfP-06");
        bad("user/{id:a}", "user/0xec7df.ec7dfe+06");
        bad("user/{id:a}", "user/0xec7df.ec7dfe-06");
        good("user/{id:a}", "user/inf");
        good("user/{id:a}", "user/nan");
        good("user/{id:a}", "user/0x1a");
        bad("user/{id:a}", "user/0b10");
        bad("user/{name:a}", "user/johndoe");
        // A: hex floating point format
        good("user/{id:A}", "user/0XEC7DF");
        good("user/{id:A}", "user/0XEC7DF.EC7DF");
        good("user/{id:A}", "user/0XEC7DF.EC7DFP+06");
        good("user/{id:A}", "user/0XEC7DF.EC7DFP-06");
        bad("user/{id:A}", "user/0XEC7DF.EC7DFp+06");
        bad("user/{id:A}", "user/0XEC7DF.EC7DFp-06");
        bad("user/{id:A}", "user/0XEC7DF.EC7DFE+06");
        bad("user/{id:A}", "user/0XEC7DF.EC7DFE-06");
        bad("user/{id:A}", "user/inf");
        bad("user/{id:A}", "user/nan");
        good("user/{id:A}", "user/INF");
        good("user/{id:A}", "user/NAN");
        bad("user/{id:A}", "user/0X1a");
        bad("user/{id:A}", "user/0b10");
        bad("user/{name:A}", "user/johndoe");

        // p: pointer
        good("user/{id:p}", "user/0x7ffd20800ad4");
        bad("user/{id:p}", "user/7ffd20800ad4");
        bad("user/{name:p}", "user/johndoe");

        // min-width
        good("user/{id:3.0f}", "user/123");
        bad("user/{id:3.0f}", "user/12");
        good("user/{id:3f}", "user/123.");
        bad("user/{id:7f}", "user/123.1");
        good("user/{id:7f}", "user/%20%20123.1");

        // precision
        good("user/{id:.0f}", "user/123");
        bad("user/{id:.0f}", "user/123.");
        bad("user/{id:.0f}", "user/123.1");
        good("user/{id:.2f}", "user/123.12");
        bad("user/{id:.2f}", "user/123");
        bad("user/{id:.2f}", "user/123.1");
        bad("user/{id:.2f}", "user/123.123");

        // use locale
        good("user/{id:Lf}", "user/123");
        char lpoint =
            std::use_facet<
                std::numpunct<char>>(std::locale()).decimal_point();
        good("user/{id:Lf}", std::string("user/123") + lpoint + "123");
        char nlpoint = lpoint == '.' ? ',' : '.';
        bad("user/{id:Lf}", std::string("user/123") + nlpoint + "123");

        // good("user/{name:Lg}", "user/123");
        // precision
        // good("user/{name:.3f}", "user/123");
        // min-width
        // good("user/{name:2}", "user/johndoe");
        // bad("user/{name:10}", "user/johndoe");
        // 0 padding
        // bad("user/{name:05}", "user/johndoe");
        // alternate
        // bad("user/{name:#5}", "user/johndoe");
        // sign
        // align
        // fill

        // static route with "." -> throw?
        // static route with ".." -> throw?
        // test routing with same name but different rules
        // test routing and matching equivalent pct-encoded segments
        // test empty segments
        // allow iterating the match results
        // optional dynamic segments
        // allow multiple {}s per segment?
        // {}* / {}+ for one or more segments
    }

    void
    run()
    {
        testPatterns();
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // experimental
} // urls
} // boost

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/pct_encoding.hpp>

#include <boost/url/static_pool.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

class pct_encoding_test
{
public:
    struct test_chars
    {
        constexpr
        bool
        operator()(char c) const noexcept
        {
            return c == 'A';
        }
    };

    struct test_chars_null
    {
        constexpr
        bool
        operator()(char c) const noexcept
        {
            return c == 'A' || c == '\0';
        }
    };

    //--------------------------------------------

    struct string_param : string_view
    {
        template<std::size_t N>
        string_param(
            char const(&ca)[N]) noexcept
            : string_view(ca, N - 1)
        {
        }
    };

    void
    testDecoding()
    {
        constexpr bnf::lut_chars CS1('A');
        constexpr bnf::lut_chars CS2 = CS1 + '\0';
        bnf::lut_chars const* pcs = &CS1;
        pct_decode_opts opt;

        auto const good = [&](
            string_param s0,
            string_param s1)
        {
            error_code ec;
            auto n = validate_pct_encoding(
                s0, ec, *pcs, opt);
            if(! BOOST_TEST(! ec.failed()))
                return;
            BOOST_TEST(n == s1.size());
            BOOST_TEST(n ==
                pct_decoded_bytes_unchecked(s0));
            auto s = pct_decode_unchecked(s0, opt);
            BOOST_TEST(s == s1);
        };

        auto const bad = [&](
            string_param s)
        {
            error_code ec;
            validate_pct_encoding(
                s, ec, test_chars{}, opt);
            BOOST_TEST(ec.failed());
        };

        {
            pcs = &CS1;
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            opt.plus_to_space = false;

            good("", "");
            good("%20", " ");
            good("A", "A");
            good("%41", "A");
            good("%42", "B");
            good("A%42", "AB");
            good("A%20%42", "A B");
            good("%00", "\0");
            bad("B");
            bad("+");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
            bad("A%00+");
        }

        {
            pcs = &CS1;
            opt.allow_null = false;
            opt.non_normal_is_error = false;
            opt.plus_to_space = false;

            good("", "");
            good("%20", " ");
            good("A", "A");
            good("%41", "A");
            good("%42", "B");
            good("A%42", "AB");
            good("A%20%42", "A B");
            bad("B");
            bad("%00");
            bad("+");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
            bad("A%00+");
        }

        {
            pcs = &CS1;
            opt.allow_null = true;
            opt.non_normal_is_error = true;
            opt.plus_to_space = false;

            good("", "");
            good("%20", " ");
            good("A", "A");
            bad("%41");
            good("%42", "B");
            good("A%42", "AB");
            good("A%20%42", "A B");
            good("%00", "\0");
            bad("B");
            bad("+");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
            bad("A%00+");
        }

        {
            pcs = &CS1;
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            opt.plus_to_space = false;

            good("", "");
            good("%20", " ");
            good("A", "A");
            good("%41", "A");
            good("%42", "B");
            good("A%42", "AB");
            good("A%20%42", "A B");
            good("%00", "\0");
            bad("B");
            bad("+");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
            bad("A%00+");
        }

        {
            pcs = &CS1;
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            opt.plus_to_space = true;

            good("", "");
            good("%20", " ");
            good("A", "A");
            good("%41", "A");
            good("%42", "B");
            good("A%42", "AB");
            good("A%20%42", "A B");
            good("%00", "\0");
            good("+", " ");
            bad("B");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
            good("A%00+", "A\0 ");
        }

        {
            pcs = &CS2;
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            opt.plus_to_space = true;

            good("\0", "\0");
            good("A\0", "A\0");
            good("%41\0", "A\0");
            good("%41%00", "A\0");
        }

        {
            pcs = &CS2;
            opt.allow_null = false;
            opt.non_normal_is_error = false;
            opt.plus_to_space = true;

            bad("\0");
            bad("A\0");
            bad("%41\0");
            bad("%41%00");
        }
    }

    //--------------------------------------------

    void
    testEncodeBytes()
    {
        auto const check =
            []( string_view s,
                std::size_t n,
                bool space_to_plus = false)
        {
            pct_encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            BOOST_TEST(pct_encode_bytes(
                s, test_chars{}, opt) == n);
        };

        check("", 0);
        check(" ", 3);
        check("A", 1);
        check("B", 3);
        check("AB", 4);
        check("A B", 7);

        check("", 0, true);
        check(" ", 1, true);
        check("A", 1, true);
        check("B", 3, true);
        check("AB", 4, true);
        check("A B", 5, true);
    }

    void
    testEncodeToBuffer()
    {
        auto const check =
            []( string_view s,
                string_view m0,
                bool space_to_plus = false)
        {
            pct_encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            auto const m = pct_encode(
                s, test_chars{}, opt);
            if(! BOOST_TEST(m == m0))
                return;
            char buf[64];
            BOOST_ASSERT(
                m.size() < sizeof(buf));
            for(std::size_t i = 0;
                i <= sizeof(buf); ++i)
            {
                char* dest = buf;
                char const* end = buf + i;
                bool const success =
                    pct_encode(dest, end,
                        s, test_chars{}, opt);
                string_view r(buf, dest - buf);
                if(success)
                {
                    BOOST_TEST(i == m.size());
                    BOOST_TEST(dest == end);
                    BOOST_TEST(r == m);
                    break;
                }
                std::size_t n = dest - buf;
                BOOST_TEST(
                    string_view(buf, n) ==
                    m.substr(0, n));
            }
        };

        check("", "");
        check(" ", "%20");
        check("A", "A");
        check("B", "%42");
        check("AB", "A%42");
        check("A B", "A%20%42");

        check("", "", true);
        check(" ", "+", true);
        check("A", "A", true);
        check("B", "%42", true);
        check("AB", "A%42", true);
        check("A B", "A+%42", true);
    }

    void
    testEncodeToString()
    {
        auto const check =
            []( string_view s,
                string_view m,
                bool space_to_plus = false)
        {
            {
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode(
                    s, test_chars{}, opt) == m);
            }
            {
                static_pool<256> pool;
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode(
                    s, test_chars{}, opt,
                        pool.allocator()) == m);
            }
        };

        check("", "");
        check(" ", "%20");
        check("A", "A");
        check("B", "%42");
        check("AB", "A%42");
        check("A B", "A%20%42");

        check("", "", true);
        check(" ", "+", true);
        check("A", "A", true);
        check("B", "%42", true);
        check("AB", "A%42", true);
        check("A B", "A+%42", true);
    }

    void
    testEncodeExtras()
    {
        // space_to_plus
        {
            BOOST_TEST(pct_encode(
                " ", test_chars{}) == "%20");
            pct_encode_opts opt;
            BOOST_TEST(opt.space_to_plus == false);
            BOOST_TEST(pct_encode(
                " ", test_chars{}, opt) == "%20");
            BOOST_TEST(pct_encode(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(pct_encode(
                " A+", test_chars{}, opt) == "%20A%2b");
            opt.space_to_plus = true;
            BOOST_TEST(pct_encode(
                " ", test_chars{}, opt) == "+");
            BOOST_TEST(pct_encode(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(pct_encode(
                " A+", test_chars{}, opt) == "+A%2b");
        }

        // allocator
        {
            static_pool<256> p;
            BOOST_TEST(pct_encode(
                "ABC", test_chars{}, {},
                p.allocator()) ==
                    "A%42%43");
        }
        {
            static_pool<4> p;
            string_view s =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            BOOST_TEST(s.size() >
                std::string().capacity());
        #ifndef _MSC_VER
            // VFALCO Because msvc's string allocates
            // one byte from a function marked noexcept
            BOOST_TEST_THROWS(pct_encode(s,
                test_chars(), {}, p.allocator()),
                    std::exception);
        #endif
        }
    }

    void
    run()
    {
        testDecoding();

        testEncodeBytes();
        testEncodeToBuffer();
        testEncodeToString();
        testEncodeExtras();
    }
};

TEST_SUITE(
    pct_encoding_test,
    "boost.url.pct_encoding");

} // urls
} // boost

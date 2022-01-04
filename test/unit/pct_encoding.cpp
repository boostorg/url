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
#include <boost/url/grammar/lut_chars.hpp>
#include "test_suite.hpp"
#include <memory>

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
        constexpr grammar::lut_chars CS1('A');
        constexpr grammar::lut_chars CS2 = CS1 + '\0';
        grammar::lut_chars const* pcs = &CS1;
        pct_decode_opts opt;

        auto const good = [&](
            string_param s0,
            string_param s1)
        {
            // validate_pct_encoding
            {
                error_code ec;
                auto n = validate_pct_encoding(
                    s0, ec, opt, *pcs);
                if(! BOOST_TEST(! ec.failed()))
                    return;
                BOOST_TEST(n == s1.size());
            }
            // pct_decode to buffer
            {
                char buf[16];
                for(std::size_t i = 0;
                    i < sizeof(buf); ++i)
                {
                    error_code ec;
                    auto const n = pct_decode(
                        buf, buf + i,
                            s0, ec, opt, *pcs);
                    if(i < s1.size())
                    {
                        BOOST_TEST(
                            ec == error::no_space);
                        BOOST_TEST(n < s1.size());
                        BOOST_TEST(n <= i);
                        continue;
                    }
                    BOOST_TEST(! ec.failed());
                    BOOST_TEST(n == s1.size());
                    BOOST_TEST(
                        string_view(buf, n) == s1);
                    break;
                }
            }
            // pct_decode() -> std::string
            {
                std::string s = pct_decode(
                    s0, opt, *pcs);
                BOOST_TEST(s == s1);
            }
            // pct_decode() -> std::basic_string
            {
                using A = basic_static_pool::
                    allocator_type<char>;
                static_pool<256> p;
                std::basic_string<char,
                    std::char_traits<char>, A> s =
                        pct_decode(s0, opt, *pcs,
                            p.allocator());
                BOOST_TEST(s == s1);
            }
            // pct_decode_to_value()
            {
                string_value s =
                    pct_decode_to_value(
                        s0, opt, *pcs);
                BOOST_TEST(s == s1);
            }
            // pct_decode_to_value(Allocator)
            {
                static_pool<256> p;
                string_value s =
                    pct_decode_to_value(
                        s0, opt, *pcs,
                            p.allocator());
                BOOST_TEST(s == s1);
            }
            // pct_decode_bytes_unchecked
            {
                auto const n =
                    pct_decode_bytes_unchecked(s0);
                BOOST_TEST(n == s1.size());
            }
            // pct_decode_unchecked
            {
                char buf[16];
                auto const n =
                    pct_decode_unchecked(
                        buf, buf + sizeof(buf),
                            s0, opt);
                BOOST_TEST(n == s1.size());
                BOOST_TEST(
                    string_view(buf, n) == s1);
            }
        };

        auto const bad = [&](
            string_param s)
        {
            // validate_pct_encoding
            {
                error_code ec;
                validate_pct_encoding(
                    s, ec, opt, test_chars{});
                BOOST_TEST(ec.failed());
            }
            // pct_decode to buffer
            {
                char buf[16];
                error_code ec;
                pct_decode(buf,
                    buf + sizeof(buf),
                        s, ec, opt, *pcs);
                BOOST_TEST(ec.failed());
            }
            // pct_decode_to_value()
            {
                BOOST_TEST_THROWS(
                    pct_decode_to_value(
                        s, opt, *pcs),
                    std::invalid_argument);
            }
            // pct_decode() -> std::string
            {
                BOOST_TEST_THROWS(
                    pct_decode(s, opt, *pcs),
                    std::invalid_argument);
            }
            // pct_decode_bytes_unchecked
            {
                // don't crash
                pct_decode_bytes_unchecked(s);
            }
            // pct_decode_unchecked
            {
                // don't crash
                char buf[16];
                pct_decode_unchecked(
                    buf, buf + sizeof(buf),
                        s, opt);
            }
            // well-defined behavior test
            {
                char buf[16];
                BOOST_ASSERT(
                    s.size() < sizeof(buf));
                for(std::size_t i = 0;
                    i < sizeof(buf); ++i)
                {
                    std::memset(
                        buf, 0xff, sizeof(buf));
                    pct_decode_unchecked(buf,
                        buf + i, s, opt);
                    string_view s1(buf, sizeof(buf));
                    BOOST_TEST(s1.find(' ') ==
                        string_view::npos);
                }
            }
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
    testEncode()
    {
        auto const check =
            []( string_view s,
                string_view m0,
                bool space_to_plus = false)
        {
            // pct_encode_bytes
            {
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode_bytes(
                    s, opt, test_chars{}) ==
                        m0.size());
            }
            // pct_encode
            {
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode(
                    s, opt, test_chars{}) == m0);
            }
            {
                static_pool<256> pool;
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode(
                    s, opt, test_chars{},
                        pool.allocator()) == m0);
            }
            // pct_encode_to_value
            {
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode_to_value(
                    s, opt, test_chars{}) == m0);
            }
            {
                static_pool<256> pool;
                pct_encode_opts opt;
                opt.space_to_plus =
                    space_to_plus;
                BOOST_TEST(pct_encode_to_value(
                    s, opt, test_chars{},
                        pool.allocator()) == m0);
            }
            pct_encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            auto const m = pct_encode_to_value(
                s, opt, test_chars{});
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
                std::size_t n = pct_encode(
                    dest, end, s, opt, test_chars{});
                string_view r(buf, n);
                if(n == m.size())
                {
                    BOOST_TEST(i == m.size());
                    BOOST_TEST(r == m);
                    break;
                }
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
    testEncodeExtras()
    {
        // space_to_plus
        {
            BOOST_TEST(pct_encode_to_value(
                " ", {}, test_chars{}) == "%20");
            pct_encode_opts opt;
            BOOST_TEST(opt.space_to_plus == false);
            BOOST_TEST(pct_encode_to_value(
                " ", opt, test_chars{}) == "%20");
            BOOST_TEST(pct_encode_to_value(
                "A", opt, test_chars{}) == "A");
            BOOST_TEST(pct_encode_to_value(
                " A+", opt, test_chars{}) == "%20A%2b");
            opt.space_to_plus = true;
            BOOST_TEST(pct_encode_to_value(
                " ", opt, test_chars{}) == "+");
            BOOST_TEST(pct_encode_to_value(
                "A", opt, test_chars{}) == "A");
            BOOST_TEST(pct_encode_to_value(
                " A+", opt, test_chars{}) == "+A%2b");
        }

        // allocator
        {
            static_pool<256> p;
            BOOST_TEST(pct_encode_to_value(
                "ABC", {}, test_chars{},
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
            BOOST_TEST_THROWS(pct_encode_to_value(s,
                {}, test_chars(), p.allocator()),
                    std::exception);
        #endif
        }
    }

    void
    run()
    {
        testDecoding();
        testEncode();
        testEncodeExtras();
    }
};

TEST_SUITE(
    pct_encoding_test,
    "boost.url.pct_encoding");

} // urls
} // boost

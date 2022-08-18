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
            return c == 'A' || c == '+';
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
        constexpr grammar::lut_chars CS1("A+");
        constexpr grammar::lut_chars CS2 = CS1 + '\0';
        grammar::lut_chars const* pcs = &CS1;
        pct_decode_opts opt;

        auto const good = [&](
            string_param s0,
            string_param s1)
        {
            // validate_pct_encoding
            {
                auto rn = validate_pct_encoding(
                    s0, *pcs, opt);
                if(! BOOST_TEST(! rn.has_error()))
                    return;
                BOOST_TEST_EQ(*rn, s1.size());
            }
            // pct_decode to buffer
            {
                char buf[16];
                for(std::size_t i = 0;
                    i < sizeof(buf); ++i)
                {
                    auto const rn = pct_decode(
                        buf, buf + i,
                            s0, *pcs, opt);
                    if(i < s1.size())
                    {
                        BOOST_TEST(
                            rn.error() == error::no_space);
                        // BOOST_TEST_LT(n, s1.size());
                        // BOOST_TEST_LE(n, i);
                        continue;
                    }
                    BOOST_TEST(! rn.has_error());
                    BOOST_TEST_EQ(*rn, s1.size());
                    BOOST_TEST_EQ(
                        string_view(buf, *rn), s1);
                    break;
                }
            }
            // pct_decode() -> std::string
            {
                std::string s;
                s.resize(pct_decode_bytes_unchecked(s0));
                auto rn =
                    pct_decode(&s[0], &s[0] + s.size(), s0, opt);
                BOOST_TEST(rn.has_value());
                s.resize(*rn);
                BOOST_TEST_EQ(s, s1);
            }
            // pct_decode() -> std::basic_string
            {
                using A = std::allocator<char>;
                std::basic_string<char,
                    std::char_traits<char>, A> s(A{});
                s.resize(pct_decode_bytes_unchecked(s0));
                auto rn = pct_decode(&s[0], &s[0] + s.size(), s0, opt);
                BOOST_TEST(rn.has_value());
                BOOST_TEST_EQ(s, s1);
            }
            // pct_decode_bytes_unchecked
            {
                auto const n =
                    pct_decode_bytes_unchecked(s0);
                BOOST_TEST_EQ(n, s1.size());
            }
            // pct_decode_unchecked
            {
                char buf[16];
                auto const n =
                    pct_decode_unchecked(
                        buf, buf + sizeof(buf),
                            s0, opt);
                BOOST_TEST_EQ(n, s1.size());
                BOOST_TEST_EQ(
                    string_view(buf, n), s1);
            }
        };

        auto const bad = [&](
            string_param s)
        {
            // validate_pct_encoding
            {
                auto rn = validate_pct_encoding(
                    s, test_chars{}, opt);
                BOOST_TEST(rn.has_error());
                if (!rn.has_error())
                    BOOST_TEST_EQ(s, "");
            }
            // pct_decode to buffer
            {
                char buf[16];
                auto rn = pct_decode(buf,
                    buf + sizeof(buf),
                        s, *pcs, opt);
                BOOST_TEST(rn.has_error());
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
            good("+", "+");
            good("A%00+", "A\0+");
            bad("B");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
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
            good("+", "+");
            bad("B");
            bad("%00");
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
            good("+", "+");
            good("A%00+", "A\0+");
            bad("B");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
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
            good("+", "+");
            good("A%00+", "A\0+");
            bad("B");
            bad("%");
            bad("%1");
            bad("%1x");
            bad("%%");
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
    check(
        string_view s,
        string_view m0,
        bool space_to_plus = false)
    {
        // pct_encode_bytes
        {
            pct_encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            BOOST_TEST(pct_encode_bytes(
                s, test_chars{}, opt) ==
                    m0.size());
        }
        // pct_encode
        {
            pct_encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            std::string t;
            t.resize(
                pct_encode_bytes(s, test_chars{}, opt));
            pct_encode(
                &t[0], &t[0] + t.size(), s, test_chars{}, opt);
            BOOST_TEST(t == m0);
        }
        pct_encode_opts opt;
        opt.space_to_plus =
            space_to_plus;
        auto const m = pct_encode_to_string(
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
            std::size_t n = pct_encode(
                dest, end, s, test_chars{}, opt);
            string_view r(buf, n);
            if(n == m.size())
            {
                BOOST_TEST_EQ(i, m.size());
                BOOST_TEST_EQ(r, m);
                break;
            }
            BOOST_TEST(
                string_view(buf, n) ==
                m.substr(0, n));
        }
    };

    void
    testEncode()
    {
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
            BOOST_TEST(pct_encode_to_string(
                " ", test_chars{}, {}) == "%20");
            pct_encode_opts opt;
            BOOST_TEST_EQ(opt.space_to_plus, false);
            BOOST_TEST(pct_encode_to_string(
                " ", test_chars{}, opt) == "%20");
            BOOST_TEST(pct_encode_to_string(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(pct_encode_to_string(
                " A+", test_chars{}, opt) == "%20A+");
            opt.space_to_plus = true;
            BOOST_TEST(pct_encode_to_string(
                " ", test_chars{}, opt) == "+");
            BOOST_TEST(pct_encode_to_string(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(pct_encode_to_string(
                " A+", test_chars{}, opt) == "+A+");
        }
    }

    void
    testValidate()
    {
        auto check = [](
            string_view s,
            error_code ec,
            pct_decode_opts opt)
        {
            auto r = validate_pct_encoding(s, opt);
            BOOST_TEST(r.has_error());
            BOOST_TEST(r.error() == ec);
        };

        pct_decode_opts opt;
        opt.allow_null = true;
        check("%a", error::missing_pct_hexdig, opt);
        check("%ar", error::bad_pct_hexdig, opt);

        opt.allow_null = false;
        check(string_view("\0", 1), error::illegal_null, opt);
        check("%00", error::illegal_null, opt);
        check("%a", error::missing_pct_hexdig, opt);
        check("%ar", error::bad_pct_hexdig, opt);

        {
            std::string dest;
            dest.resize(1);
            result<std::size_t> r = pct_decode(
                &dest[0], &dest[1], "%a", opt);
            BOOST_TEST_EQ(r.error(), error::missing_pct_hexdig);
        }
        {
            std::string dest;
            dest.resize(1);
            result<std::size_t> r = pct_decode(
                &dest[0], &dest[1], "%aa%aa", opt);
            BOOST_TEST_EQ(r.error(), error::no_space);
        }
    }

    void
    run()
    {
        testDecoding();
        testEncode();
        testEncodeExtras();
        testValidate();
    }
};

TEST_SUITE(
    pct_encoding_test,
    "boost.url.pct_encoding");

} // urls
} // boost

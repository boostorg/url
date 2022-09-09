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
#include <boost/url/decode.hpp>

#include <boost/url/grammar/lut_chars.hpp>
#include "test_suite.hpp"
#include <boost/url/string_view.hpp>
#include <boost/url/encode.hpp>
#include <memory>

namespace boost {
namespace urls {

class decode_test
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
        decode_opts opt;

        auto const good = [&](
            string_param s0,
            string_param s1)
        {
            // decode -> decode_view
            {
                auto const r = decode(s0, opt, *pcs);
                if (BOOST_TEST(r.has_value()))
                {
                    BOOST_TEST_EQ(r->size(), s1.size());
                    BOOST_TEST_EQ(*r, s1);
                }
            }
            // detail::validate_encoding
            {
                auto rn = detail::validate_encoding(
                    s0, opt, *pcs);
                if(! BOOST_TEST(! rn.has_error()))
                    return;
                BOOST_TEST_EQ(*rn, s1.size());
            }
            // decode to buffer
            {
                char buf[16];
                for(std::size_t i = 0;
                    i < sizeof(buf); ++i)
                {
                    auto const rn = detail::decode(
                        buf, buf + i,
                            s0, opt, *pcs);
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
            // decode() -> std::string
            {
                std::string s;
                s.resize(detail::decode_bytes_unchecked(s0));
                auto rn =
                    detail::decode(&s[0], &s[0] + s.size(), s0, opt);
                BOOST_TEST(rn.has_value());
                s.resize(*rn);
                BOOST_TEST_EQ(s, s1);
            }
            // decode() -> std::basic_string
            {
                using A = std::allocator<char>;
                std::basic_string<char,
                    std::char_traits<char>, A> s(A{});
                s.resize(detail::decode_bytes_unchecked(s0));
                auto rn = detail::decode(&s[0], &s[0] + s.size(), s0, opt);
                BOOST_TEST(rn.has_value());
                BOOST_TEST_EQ(s, s1);
            }
            // detail::decode_bytes_unchecked
            {
                auto const n =
                    detail::decode_bytes_unchecked(s0);
                BOOST_TEST_EQ(n, s1.size());
            }
            // decode_unchecked
            {
                char buf[16];
                auto const n =
                    detail::decode_unchecked(
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
            // detail::validate_encoding
            {
                auto rn = detail::validate_encoding(
                    s, opt, test_chars{});
                BOOST_TEST(rn.has_error());
                if (!rn.has_error())
                    BOOST_TEST_EQ(s, "");
            }
            // decode to buffer
            {
                char buf[16];
                auto rn = detail::decode(buf,
                    buf + sizeof(buf),
                        s, opt, *pcs);
                BOOST_TEST(rn.has_error());
            }
            // detail::decode_bytes_unchecked
            {
                // don't crash
                detail::decode_bytes_unchecked(s);
            }
            // detail::decode_unchecked
            {
                // don't crash
                char buf[16];
                detail::decode_unchecked(
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
                    detail::decode_unchecked(buf,
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
        // encoded_size
        {
            encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            BOOST_TEST(encoded_size(
                s, opt, test_chars{}) ==
                    m0.size());
        }
        // encode
        {
            encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            std::string t;
            t.resize(
                encoded_size(s, opt, test_chars{}));
            encode(
                &t[0], &t[0] + t.size(), s, opt, test_chars{});
            BOOST_TEST(t == m0);
        }
        encode_opts opt;
        opt.space_to_plus =
            space_to_plus;
        auto const m = encode_to_string(
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
            std::size_t n = encode(
                dest, end, s, opt, test_chars{});
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
    testValidate()
    {
        auto check = [](
            string_view s,
            error_code ec,
            decode_opts opt)
        {
            auto r = detail::validate_encoding(s, opt);
            BOOST_TEST(r.has_error());
            BOOST_TEST(r.error() == ec);
        };

        decode_opts opt;
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
            result<std::size_t> r = detail::decode(
                &dest[0], &dest[1], "%a", opt);
            BOOST_TEST_EQ(r.error(), error::missing_pct_hexdig);
        }
        {
            std::string dest;
            dest.resize(1);
            result<std::size_t> r = detail::decode(
                &dest[0], &dest[1], "%aa%aa", opt);
            BOOST_TEST_EQ(r.error(), error::no_space);
        }
    }

    void
    run()
    {
        testDecoding();
        testValidate();
    }
};

TEST_SUITE(
    decode_test,
    "boost.url.decode");

} // urls
} // boost

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

    template<
        class CharSet = test_chars>
    static
    void
    bad_decode_size(
        string_view s,
        pct_decode_opts const& opt = {},
        CharSet const& cs = {})
    {
        error_code ec;
        pct_decode_size(
            s, ec, cs, opt);
        BOOST_TEST(ec.failed());
    }

    template<
        class CharSet = test_chars>
    static
    void
    good_decode_size(
        std::size_t n,
        string_view s,
        pct_decode_opts const& opt = {},
        CharSet const& cs = {})
    {
        error_code ec;
        std::size_t n1 = pct_decode_size(
            s, ec, cs, opt);
        BOOST_TEST(! ec.failed());
        BOOST_TEST(n1 == n);
    }
    
    void
    testDecodeSize()
    {
        // check defaults
        {
            pct_decode_opts opt;
            BOOST_TEST(
                opt.allow_null == true);
            BOOST_TEST(
                opt.plus_to_space == true);
            BOOST_TEST(
                opt.non_normal_is_error == false);
        }

        good_decode_size(0, "");
        bad_decode_size("%");
        bad_decode_size("%%");
        bad_decode_size("%a");
        bad_decode_size("%g");
        bad_decode_size("%ag");
        bad_decode_size("%a%");

        // plus to space
        {
            pct_decode_opts opt;
            good_decode_size(1, "+", opt);
            opt.non_normal_is_error = true;
            good_decode_size(1, "+", opt);
            opt.plus_to_space = false;
            opt.non_normal_is_error = true;
            bad_decode_size("+", opt);
        }

        // allow null
        {
            pct_decode_opts opt;

            // null is unreserved
            opt.allow_null = true;
            good_decode_size(1,
                string_view("\0", 1), opt, test_chars_null{});
            good_decode_size(1, "%00", opt, test_chars_null{});
            opt.allow_null = false;
            bad_decode_size(
                string_view("\0", 1), opt, test_chars_null{});

            // null is reserved
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            bad_decode_size(string_view("\0", 1), opt);
            good_decode_size(1, string_view("\0", 1), opt, test_chars_null{});
            good_decode_size(1, "%00", opt);
            opt.allow_null = false;
            bad_decode_size(string_view("\0", 1), opt);
            bad_decode_size("%00", opt);
        }

        // non-normalized is error
        {
            pct_decode_opts opt;

            good_decode_size(1, "A", opt);
            good_decode_size(1, "%41", opt);
            bad_decode_size("ab%41", opt);
            bad_decode_size("aA", opt);

            opt.non_normal_is_error = true;
            good_decode_size(1, "A", opt);
            good_decode_size(2, "A%20", opt);
            bad_decode_size("%41", opt);
            opt.plus_to_space = true;
            good_decode_size(2, "A+", opt);
            opt.plus_to_space = false;
            bad_decode_size("%41", opt);
            bad_decode_size("ab%41", opt);
            bad_decode_size(" ", opt);
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
                    pct_encode(s, test_chars{},
                        dest, end, opt);
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
        testDecodeSize();

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

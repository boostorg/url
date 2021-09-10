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
            good_decode_size(1, string_view("\0", 1), opt);
            good_decode_size(1, "%00", opt);
            opt.allow_null = false;
            bad_decode_size(string_view("\0", 1), opt);
            bad_decode_size("%00", opt);
        }

        // non-normalized is error
        {
            pct_decode_opts opt;

            good_decode_size(1, "A", opt);
            good_decode_size(2, "aA", opt);
            good_decode_size(3, "ab%41", opt);

            opt.non_normal_is_error = true;
            good_decode_size(1, "A", opt);
            good_decode_size(2, "A%20", opt);
            opt.plus_to_space = true;
            good_decode_size(2, "A+", opt);
            opt.plus_to_space = false;
            bad_decode_size("%41", opt);
            bad_decode_size("ab%41", opt);
            bad_decode_size(" ", opt);
        }
    }

    //--------------------------------------------

    template<
        class CharSet = test_chars,
        class Allocator =
            std::allocator<char>>
    void
    bad_decode(
        string_view s,
        pct_decode_opts const& opt = {},
        CharSet const& cs = {},
        Allocator const& a = {})
    {
        BOOST_TEST_THROWS(pct_decode(
            s, cs, opt, a),
            std::exception);
    }

    template<
        class CharSet = test_chars,
        class Allocator =
        std::allocator<char>>
    void
    good_decode(
        string_view s,
        string_view s1,
        pct_decode_opts const& opt = {},
        CharSet const& cs = {},
        Allocator const& a = {})
    {
        string_type<Allocator> r;
        BOOST_TEST_NO_THROW(
            r = pct_decode(s, cs, opt, a));
        BOOST_TEST(r == s1);
    }

    void
    testDecode()
    {
        bad_decode("%");
        bad_decode("%%");
        bad_decode("%a");
        bad_decode("%g");
        bad_decode("%ag");
        bad_decode("%a%");

        // plus to space
        {
            pct_decode_opts opt;
            good_decode("+", " ", opt);
            opt.non_normal_is_error = true;
            good_decode("+", " ", opt);
            opt.plus_to_space = false;
            opt.non_normal_is_error = true;
            bad_decode("+", opt);
        }

        // allow null
        {
            pct_decode_opts opt;

            // null is unreserved
            opt.allow_null = true;
            good_decode(
                string_view("\0", 1),
                string_view("\0", 1),
                opt, test_chars_null{});
            good_decode(
                "%00", string_view("\0", 1),
                opt, test_chars_null{});
            opt.allow_null = false;
            bad_decode(string_view("\0", 1),
                opt, test_chars_null{});

            // null is reserved
            opt.allow_null = true;
            opt.non_normal_is_error = false;
            good_decode(
                string_view("\0", 1),
                string_view("\0", 1), opt);
            good_decode("%00",
                string_view("\0", 1), opt);
            opt.allow_null = false;
            bad_decode(string_view("\0", 1), opt);
        }

        // non-normalized is error
        {
            pct_decode_opts opt;

            good_decode("A", "A", opt);
            good_decode("aA", "aA", opt);
            good_decode("ab%41", "abA", opt);

            opt.non_normal_is_error = true;
            good_decode("A", "A", opt);
            good_decode("A%20", "A ", opt);
            opt.plus_to_space = true;
            good_decode("A+", "A ", opt);
            opt.plus_to_space = false;
            bad_decode("%41", opt);
            bad_decode("ab%41", opt);
            bad_decode(" ", opt);
        }
    }

    //--------------------------------------------

    void
    testEncodeSize()
    {
        BOOST_TEST(pct_encode_size(
            "A", test_chars{}) == 1);
        BOOST_TEST(pct_encode_size(
            "AAAA", test_chars{}) == 4);
        BOOST_TEST(pct_encode_size(
            "%32", test_chars{}) == 9);
        BOOST_TEST(pct_encode_size(
            " ", test_chars{}) == 3);

        // space_to_plus
        {
            BOOST_TEST(pct_encode_size(
                "  ", test_chars{}) == 6);
            pct_encode_opts opt;
            BOOST_TEST(pct_encode_size(
                "  ", test_chars{}, opt) == 6);
            opt.space_to_plus = true;
            BOOST_TEST(pct_encode_size(
                "  ", test_chars{}, opt) == 2);
        }
    }

    //--------------------------------------------

    void
    testEncode()
    {
        BOOST_TEST(pct_encode(
            "A", test_chars{}) == "A");
        BOOST_TEST(pct_encode(
            "B", test_chars{}) == "%42");
        BOOST_TEST(pct_encode(
            "AB", test_chars{}) == "A%42");

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
        #ifndef BOOST_MSVC
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
        testDecode();
        testEncodeSize();
        testEncode();
    }
};

TEST_SUITE(
    pct_encoding_test,
    "boost.url.pct_encoding");

} // urls
} // boost

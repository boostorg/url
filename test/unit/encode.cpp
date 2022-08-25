//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/lut_chars.hpp>
#include "test_suite.hpp"
#include <boost/url/string_view.hpp>
#include <boost/url/decode.hpp>
#include <boost/url/encode.hpp>
#include <memory>

namespace boost {
namespace urls {

class encode_test
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

    void
    check(
        string_view s,
        string_view m0,
        bool space_to_plus = false)
    {
        // encode_bytes
        {
            encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            BOOST_TEST(encode_bytes(
                s, test_chars{}, opt) ==
                    m0.size());
        }
        // encode
        {
            encode_opts opt;
            opt.space_to_plus =
                space_to_plus;
            std::string t;
            t.resize(
                encode_bytes(s, test_chars{}, opt));
            encode(
                &t[0], &t[0] + t.size(), s, test_chars{}, opt);
            BOOST_TEST(t == m0);
        }
        encode_opts opt;
        opt.space_to_plus =
            space_to_plus;
        auto const m = encode_to_string(
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
            std::size_t n = encode(
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
            BOOST_TEST(encode_to_string(
                " ", test_chars{}, {}) == "%20");
            encode_opts opt;
            BOOST_TEST_EQ(opt.space_to_plus, false);
            BOOST_TEST(encode_to_string(
                " ", test_chars{}, opt) == "%20");
            BOOST_TEST(encode_to_string(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(encode_to_string(
                " A+", test_chars{}, opt) == "%20A+");
            opt.space_to_plus = true;
            BOOST_TEST(encode_to_string(
                " ", test_chars{}, opt) == "+");
            BOOST_TEST(encode_to_string(
                "A", test_chars{}, opt) == "A");
            BOOST_TEST(encode_to_string(
                " A+", test_chars{}, opt) == "+A+");
        }
    }

    void
    run()
    {
        testEncode();
        testEncodeExtras();
    }
};

TEST_SUITE(
    encode_test,
    "boost.url.encode");

} // urls
} // boost

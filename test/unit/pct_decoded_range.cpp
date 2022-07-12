//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/pct_decoded_range.hpp>

#include <sstream>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct pct_decoded_range_test
{
    using A = std::allocator<char>;

    string_view str = "a%20uri+test";
    string_view dec_str = "a uri test";
    string_view no_plus_dec_str = "a uri+test";
    const std::size_t dn = 10;

    void
    testPctDecodedRange()
    {
        // pct_decoded_range()
        {
            pct_decoded_range s;
            BOOST_TEST_EQ(s, "");
            BOOST_TEST_EQ(s.size(), 0u);
            BOOST_TEST_EQ(s.encoded_size(), 0u);
        }

        // pct_decoded_range(bool plus_to_space)
        {
            pct_decoded_range s(false);
            BOOST_TEST_EQ(s, "");
            BOOST_TEST_EQ(s.size(), 0u);
            BOOST_TEST_EQ(s.encoded_size(), 0u);
        }

        // pct_decoded_range(char const*)
        {
            pct_decoded_range s(str.data());
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(char const*, bool plus_to_space)
        {
            pct_decoded_range s(str.data(), false);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(char const*, std::size_t)
        {
            pct_decoded_range s(str.data(), str.size());
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(char const*, std::size_t, bool plus_to_space)
        {
            pct_decoded_range s(str.data(), str.size(), false);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(char const*, char const*)
        {
            pct_decoded_range s(str.begin(), str.end());
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(char const*, char const*, bool plus_to_space)
        {
            pct_decoded_range s(str.begin(), str.end(), false);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(string_view)
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(string_view, bool plus_to_space)
        {
            pct_decoded_range s(str, false);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        // pct_decoded_range(string_view)
        {
            std::string_view std_str = str;
            pct_decoded_range s(std_str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // pct_decoded_range(string_view, bool plus_to_space)
        {
            std::string_view std_str = str;
            pct_decoded_range s(std_str, false);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }
#endif
    }

    void
    testIter()
    {
        // begin()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(*s.begin(), s.front());
        }

        // cbegin()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(*s.cbegin(), s.front());
        }

        // end()
        {
            pct_decoded_range s(str);
            auto l = s.end();
            --l;
            BOOST_TEST_EQ(*l, s.back());
        }

        // cend()
        {
            pct_decoded_range s(str);
            auto l = s.cend();
            --l;
            BOOST_TEST_EQ(*l, s.back());
        }
    }

    void
    testAccessors()
    {
        // front()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.front(), 'a');
        }

        // back()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.back(), 't');
        }

        // encoded_data()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.encoded_data(), str.data());
        }
    }

    void
    testObservers()
    {
        // size()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.size(), dn);
        }

        // length()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.length(), dn);
        }

        // encoded_size()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.encoded_size(), str.size());
        }

        // encoded_length()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.encoded_length(), str.size());
        }

        // max_size()
        {
            pct_decoded_range s(str);
            BOOST_TEST_GT(s.max_size(), 0u);
        }

        // empty()
        {
            pct_decoded_range s;
            BOOST_TEST(s.empty());

            pct_decoded_range s2(str);
            BOOST_TEST_NOT(s2.empty());
        }
    }

    void
    testCopy()
    {
        // copy()
        {
            pct_decoded_range s(str);
            std::string out(s.size(), ' ');
            s.copy(&out[0], s.size());
            BOOST_TEST_EQ(s, dec_str);
        }
    }

    void
    testCompare()
    {
        // compare()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.compare(dec_str), 0);
            BOOST_TEST_EQ(s.compare("a a"), 1);
            BOOST_TEST_EQ(s.compare("a z"), -1);
            std::string bs = "z";
            BOOST_TEST_EQ(s.compare(bs), -1);
        }

        // operators
        {
            pct_decoded_range s(str);

            pct_decoded_range s0(str);
            pct_decoded_range s1("a%20tri+test");
            pct_decoded_range s2("a%20vri+test");

            string_view str0(dec_str);
            string_view str1("a tri test");
            string_view str2("a vri test");

            std::string bstr0(dec_str);
            std::string bstr1("a tri test");
            std::string bstr2("a vri test");

            BOOST_TEST(s == s0);
            BOOST_TEST_NOT(s == s1);
            BOOST_TEST(s == str0);
            BOOST_TEST_NOT(s == str1);
            BOOST_TEST(s == bstr0);
            BOOST_TEST_NOT(s == bstr1);

            BOOST_TEST(s != s2);
            BOOST_TEST_NOT(s != s0);
            BOOST_TEST(s != str2);
            BOOST_TEST_NOT(s != str0);
            BOOST_TEST(s != bstr2);
            BOOST_TEST_NOT(s != bstr0);

            BOOST_TEST(s < s2);
            BOOST_TEST_NOT(s < s0);
            BOOST_TEST(s < str2);
            BOOST_TEST_NOT(s < str0);
            BOOST_TEST(s < bstr2);
            BOOST_TEST_NOT(s < bstr0);

            BOOST_TEST(s <= s2);
            BOOST_TEST(s <= s0);
            BOOST_TEST(s <= str2);
            BOOST_TEST(s <= str0);
            BOOST_TEST(s <= bstr2);
            BOOST_TEST(s <= bstr0);

            BOOST_TEST(s > s1);
            BOOST_TEST_NOT(s > s0);
            BOOST_TEST(s > str1);
            BOOST_TEST_NOT(s > str0);
            BOOST_TEST(s > bstr1);
            BOOST_TEST_NOT(s > bstr0);

            BOOST_TEST(s >= s1);
            BOOST_TEST(s >= s0);
            BOOST_TEST(s >= str1);
            BOOST_TEST(s >= str0);
            BOOST_TEST(s >= bstr1);
            BOOST_TEST(s >= bstr0);

        }
    }

    void
    testConversion()
    {
        // to_string()
        {
            pct_decoded_range s(str);
            std::string o = "to be recycled";
            BOOST_TEST_EQ(s.to_string(o), dec_str);
        }

        // to_string()
        {
            pct_decoded_range s(str);
            BOOST_TEST_EQ(s.to_string(), dec_str);
        }

        // explicit operator basic_string()
        {
            pct_decoded_range s(str);
            std::string o(s);
            BOOST_TEST_EQ(o, dec_str);
        }

        // pass it to a function taking a string_view
        {
            auto fn = [this](string_view sv)
            {
                BOOST_TEST(sv == dec_str);
            };
            pct_decoded_range s(str);
            fn(s.to_string());
        }

        // pass it to a function taking a char*
        {
            auto fn = [this](char const* sv)
            {
                BOOST_TEST(sv == dec_str);
            };
            pct_decoded_range s(str);
            fn(s.to_string().c_str());
        }
    }

    void
    testAppend()
    {
        // to_string()
        {
            pct_decoded_range s(str);
            std::string o = "init ";
            s.append_to(o);
            std::string exp = std::string("init ");
            exp += dec_str;
            BOOST_TEST_EQ(o, exp);
        }
    }

    void
    testStream()
    {
        // operator<<
        {
            std::stringstream ss;
            pct_decoded_range s(str);
            ss << s;
            BOOST_TEST_EQ(ss.str(), dec_str);
        }
    }

    void
    run()
    {
        testPctDecodedRange();
        testIter();
        testAccessors();
        testObservers();
        testCopy();
        testCompare();
        testConversion();
        testAppend();
        testStream();
    }
};

TEST_SUITE(
    pct_decoded_range_test,
    "boost.url.pct_decoded_range");

} // urls
} // boost

//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/decode_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include <sstream>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct decode_view_test
{
    string_view str = "a%20uri+test";
    string_view dec_str = "a uri test";
    string_view no_plus_dec_str = "a uri+test";
    const std::size_t dn = 10;
    decode_opts no_plus_opt;

    decode_view_test()
    {
        no_plus_opt.plus_to_space = false;
    }

    void
    testDecodedView()
    {
        // decode_view()
        {
            decode_view s;
            BOOST_TEST_EQ(s, "");
            BOOST_TEST_EQ(s.size(), 0u);
            BOOST_TEST_EQ(s.encoded().size(), 0u);
        }

        // decode_view(char const*)
        {
            decode_view s(str.data());
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // decode_view(char const*, bool plus_to_space)
        {
            decode_view
                s(str.data(), no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // decode_view(string_view)
        {
            decode_view s(str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // decode_view(string_view, bool plus_to_space)
        {
            decode_view s(str, no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        // decode_view(string_view)
        {
            std::string_view std_str = str;
            decode_view s(std_str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // decode_view(string_view, bool plus_to_space)
        {
            std::string_view std_str = str;
            decode_view s(std_str, no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }
#endif

        // decode_view(string_view)
        {
            std::string ss(str);
            decode_view s(ss);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // decode_view(string_view, bool plus_to_space)
        {
            std::string ss(str);
            decode_view s(ss, no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }
    }

    void
    testIter()
    {
        // begin()
        {
            decode_view s(str);
            BOOST_TEST_EQ(*s.begin(), s.front());
            BOOST_TEST_NE(s.begin(),
                decode_view::iterator{});
        }

        // end()
        {
            decode_view s(str);
            auto l = s.end();
            --l;
            BOOST_TEST_EQ(*l, s.back());
            BOOST_TEST_NE(l,
                decode_view::iterator{});
        }
    }

    void
    testAccessors()
    {
        // front()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.front(), 'a');
        }

        // back()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.back(), 't');
        }

        // encoded().data()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.encoded().data(), str.data());
        }
    }

    void
    testObservers()
    {
        // size()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.size(), dn);
        }

        // encoded().size()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // encoded().length()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.encoded().length(), str.size());
        }

        // encoded().max_size()
        {
            decode_view s(str);
            BOOST_TEST_GT(s.encoded().max_size(), 0u);
        }

        // empty()
        {
            decode_view s;
            BOOST_TEST(s.empty());

            decode_view s2(str);
            BOOST_TEST_NOT(s2.empty());
        }
    }

    void
    testCopy()
    {
        // copy()
        {
            decode_view s(str);
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
            decode_view s(str);
            BOOST_TEST_EQ(s.compare(dec_str), 0);
            BOOST_TEST_EQ(s.compare("a a"), 1);
            BOOST_TEST_EQ(s.compare("a z"), -1);
            std::string bs = "z";
            BOOST_TEST_EQ(s.compare(bs), -1);
        }

        // operators
        {
            decode_view s(str);

            // decode_view
            {
                decode_view s0(str);
                decode_view s1("a%20tri+test");
                decode_view s2("a%20vri+test");
                BOOST_TEST(s == s0);
                BOOST_TEST_NOT(s == s1);
                BOOST_TEST(s != s2);
                BOOST_TEST_NOT(s != s0);
                BOOST_TEST(s < s2);
                BOOST_TEST_NOT(s < s0);
                BOOST_TEST(s <= s2);
                BOOST_TEST(s <= s0);
                BOOST_TEST(s > s1);
                BOOST_TEST_NOT(s > s0);
                BOOST_TEST(s >= s1);
                BOOST_TEST(s >= s0);
            }

            // string_view
            {
                string_view str0(dec_str);
                string_view str1("a tri test");
                string_view str2("a vri test");
                BOOST_TEST(s == str0);
                BOOST_TEST_NOT(s == str1);
                BOOST_TEST(s != str2);
                BOOST_TEST_NOT(s != str0);
                BOOST_TEST(s < str2);
                BOOST_TEST_NOT(s < str0);
                BOOST_TEST(s <= str2);
                BOOST_TEST(s <= str0);
                BOOST_TEST(s > str1);
                BOOST_TEST_NOT(s > str0);
                BOOST_TEST(s >= str1);
                BOOST_TEST(s >= str0);
            }

            // string
            {
                std::string bstr0(dec_str);
                std::string bstr1("a tri test");
                std::string bstr2("a vri test");
                BOOST_TEST(s == bstr0);
                BOOST_TEST_NOT(s == bstr1);
                BOOST_TEST(s != bstr2);
                BOOST_TEST_NOT(s != bstr0);
                BOOST_TEST(s < bstr2);
                BOOST_TEST_NOT(s < bstr0);
                BOOST_TEST(s <= bstr2);
                BOOST_TEST(s <= bstr0);
                BOOST_TEST(s > bstr1);
                BOOST_TEST_NOT(s > bstr0);
                BOOST_TEST(s >= bstr1);
                BOOST_TEST(s >= bstr0);
            }


            // string literals
            {
                BOOST_TEST(s == "a uri test");
                BOOST_TEST_NOT(s == "a tri test");
                BOOST_TEST(s != "a vri test");
                BOOST_TEST_NOT(s != "a uri test");
                BOOST_TEST(s < "a vri test");
                BOOST_TEST_NOT(s < "a uri test");
                BOOST_TEST(s <= "a vri test");
                BOOST_TEST(s <= "a uri test");
                BOOST_TEST(s > "a tri test");
                BOOST_TEST_NOT(s > "a uri test");
                BOOST_TEST(s >= "a tri test");
                BOOST_TEST(s >= "a uri test");
            }

        }
    }

    void
    testConversion()
    {
        // to_string()
        {
            decode_view s(str);
            BOOST_TEST_EQ(s.to_string(), dec_str);
        }

        // append_to()
        {
            decode_view s(str);
            std::string o = "init ";
            s.append_to(o);

            std::string exp = std::string("init ");
            exp.append(dec_str.data(), dec_str.size());

            BOOST_TEST_EQ(o, exp);
        }

        // assign_to()
        {
            decode_view s(str);
            std::string o = "init ";
            s.assign_to(o);
            BOOST_TEST_EQ(o, dec_str);
        }

        // pass it to a function taking a string_view
        {
            auto f = [this](string_view sv)
            {
                BOOST_TEST(sv == dec_str);
            };
            decode_view s(str);
            f(s.to_string());
        }

        // pass it to a function taking a char*
        {
            auto f = [this](char const* sv)
            {
                BOOST_TEST(sv == dec_str);
            };
            decode_view s(str);
            f(s.to_string().c_str());
        }
    }

    void
    testStream()
    {
        // operator<<
        {
            std::stringstream ss;
            decode_view  s(str);
            ss << s;
            BOOST_TEST_EQ(ss.str(), dec_str);
        }
    }

    void
    testPR127Cases()
    {
        {
            std::stringstream ss;
            urls::decode_view ds("test+string");
            // no warning about object slicing
            ss << ds;
        }

        {
            auto break_stuff =
                [this](urls::string_view const& a) {
                urls::string_view b{};
                b = a;
                BOOST_TEST_EQ(b.size(), dn);
            };
            break_stuff(decode_view{str}.to_string());
        }

        {
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
            struct A
            {
                A( std::string_view s)
                {
                    boost::ignore_unused(s);
                }

                A( std::string_view s, std::size_t dn )
                {
                    BOOST_TEST_EQ(s.size(), dn);
                    boost::ignore_unused(s, dn);
                }
            };
            A a1(decode_view{str}.to_string(), dn);
            A a2(std::string_view(decode_view{str}.to_string()), dn);
            A a3 = std::string_view(decode_view{str}.to_string());
            // https://en.cppreference.com/w/cpp/language/copy_initialization#Notes
            // A a4 = decode_view{str}.to_string();
            boost::ignore_unused(a1, a2, a3);
#endif
            struct B
            {
                B( urls::string_view s)
                {
                    boost::ignore_unused(s);
                }

                B( urls::string_view s, std::size_t dn )
                {
                    BOOST_TEST_EQ(s.size(), dn);
                    boost::ignore_unused(s, dn);
                }
            };
            B b1(decode_view{str}.to_string(), dn);
            B b2(string_view(
                       decode_view{str}.to_string()), dn);
            B b3 = string_view(
                decode_view{str}.to_string());
            // https://en.cppreference.com/w/cpp/language/copy_initialization#Notes
            // B b4 = decode_view{str}.to_string();
            boost::ignore_unused(b1, b2, b3);
        }

        {
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
            auto f1 = []( std::string_view ) {};
            f1(decode_view{str}.to_string());
#endif
            auto f2 = []( urls::string_view ) {};
            f2(decode_view{str}.to_string());

            auto f3 = []( std::string const& ) {};
            f3(decode_view{str}.to_string());

            auto f4 = []( std::basic_string<char,
                         std::char_traits<char>,
                         std::allocator<char>> const&) {};
            f4(decode_view{str}.to_string());
        }
    }

    void
    run()
    {
        testDecodedView();
        testIter();
        testAccessors();
        testObservers();
        testCopy();
        testCompare();
        testConversion();
        testStream();
        testPR127Cases();
    }
};

TEST_SUITE(
    decode_view_test,
    "boost.url.decode_view");

} // urls
} // boost

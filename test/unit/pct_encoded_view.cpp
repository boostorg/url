//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/pct_encoded_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include <sstream>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct pct_encoded_view_test
{
    string_view str = "a%20uri+test";
    string_view dec_str = "a uri test";
    string_view no_plus_dec_str = "a uri+test";
    const std::size_t dn = 10;
    pct_decode_opts no_plus_opt;

    pct_encoded_view_test()
    {
        no_plus_opt.plus_to_space = false;
    }

    void
    testDecodedView()
    {
        // pct_encoded_view()
        {
            pct_encoded_view s;
            BOOST_TEST_EQ(s, "");
            BOOST_TEST_EQ(s.size(), 0u);
            BOOST_TEST_EQ(s.encoded().size(), 0u);
        }

        // pct_encoded_view(char const*)
        {
            pct_encoded_view s(str.data());
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // pct_encoded_view(char const*, bool plus_to_space)
        {
            pct_encoded_view
                s(str.data(), no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // pct_encoded_view(string_view)
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // pct_encoded_view(string_view, bool plus_to_space)
        {
            pct_encoded_view s(str, no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        // pct_encoded_view(string_view)
        {
            std::string_view std_str = str;
            pct_encoded_view s(std_str);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // pct_encoded_view(string_view, bool plus_to_space)
        {
            std::string_view std_str = str;
            pct_encoded_view s(std_str, no_plus_opt);
            BOOST_TEST_EQ(s, no_plus_dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }
#endif

        // pct_encoded_view(string_view)
        {
            std::string ss(str);
            pct_encoded_view s(ss);
            BOOST_TEST_EQ(s, dec_str);
            BOOST_TEST_EQ(s.size(), dn);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // pct_encoded_view(string_view, bool plus_to_space)
        {
            std::string ss(str);
            pct_encoded_view s(ss, no_plus_opt);
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
            pct_encoded_view s(str);
            BOOST_TEST_EQ(*s.begin(), s.front());
            BOOST_TEST_NE(s.begin(),
                pct_encoded_view::iterator{});
        }

        // end()
        {
            pct_encoded_view s(str);
            auto l = s.end();
            --l;
            BOOST_TEST_EQ(*l, s.back());
            BOOST_TEST_NE(l,
                pct_encoded_view::iterator{});
        }
    }

    void
    testAccessors()
    {
        // front()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.front(), 'a');
        }

        // back()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.back(), 't');
        }

        // encoded().data()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.encoded().data(), str.data());
        }
    }

    void
    testObservers()
    {
        // size()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.size(), dn);
        }

        // encoded().size()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.encoded().size(), str.size());
        }

        // encoded().length()
        {
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.encoded().length(), str.size());
        }

        // encoded().max_size()
        {
            pct_encoded_view s(str);
            BOOST_TEST_GT(s.encoded().max_size(), 0u);
        }

        // empty()
        {
            pct_encoded_view s;
            BOOST_TEST(s.empty());

            pct_encoded_view s2(str);
            BOOST_TEST_NOT(s2.empty());
        }
    }

    void
    testCopy()
    {
        // copy()
        {
            pct_encoded_view s(str);
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
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.compare(dec_str), 0);
            BOOST_TEST_EQ(s.compare("a a"), 1);
            BOOST_TEST_EQ(s.compare("a z"), -1);
            std::string bs = "z";
            BOOST_TEST_EQ(s.compare(bs), -1);
        }

        // operators
        {
            pct_encoded_view s(str);

            // pct_encoded_view
            {
                pct_encoded_view s0(str);
                pct_encoded_view s1("a%20tri+test");
                pct_encoded_view s2("a%20vri+test");
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
            pct_encoded_view s(str);
            BOOST_TEST_EQ(s.to_string(), dec_str);
        }

        // append_to()
        {
            pct_encoded_view s(str);
            std::string o = "init ";
            s.append_to(o);

            std::string exp = std::string("init ");
            exp.append(dec_str.data(), dec_str.size());

            BOOST_TEST_EQ(o, exp);
        }

        // assign_to()
        {
            pct_encoded_view s(str);
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
            pct_encoded_view s(str);
            f(s.to_string());
        }

        // pass it to a function taking a char*
        {
            auto f = [this](char const* sv)
            {
                BOOST_TEST(sv == dec_str);
            };
            pct_encoded_view s(str);
            f(s.to_string().c_str());
        }
    }

    void
    testStream()
    {
        // operator<<
        {
            std::stringstream ss;
            pct_encoded_view  s(str);
            ss << s;
            BOOST_TEST_EQ(ss.str(), dec_str);
        }
    }

    void
    testPR127Cases()
    {
        {
            std::stringstream ss;
            urls::pct_encoded_view ds("test+string");
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
            break_stuff(pct_encoded_view{str}.to_string());
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
            A a1(pct_encoded_view{str}.to_string(), dn);
            A a2(std::string_view(pct_encoded_view{str}.to_string()), dn);
            A a3 = std::string_view(pct_encoded_view{str}.to_string());
            // https://en.cppreference.com/w/cpp/language/copy_initialization#Notes
            // A a4 = pct_encoded_view{str}.to_string();
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
            B b1(pct_encoded_view{str}.to_string(), dn);
            B b2(string_view(
                       pct_encoded_view{str}.to_string()), dn);
            B b3 = string_view(
                pct_encoded_view{str}.to_string());
            // https://en.cppreference.com/w/cpp/language/copy_initialization#Notes
            // B b4 = pct_encoded_view{str}.to_string();
            boost::ignore_unused(b1, b2, b3);
        }

        {
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
            auto f1 = []( std::string_view ) {};
            f1(pct_encoded_view{str}.to_string());
#endif
            auto f2 = []( urls::string_view ) {};
            f2(pct_encoded_view{str}.to_string());

            auto f3 = []( std::string const& ) {};
            f3(pct_encoded_view{str}.to_string());

            auto f4 = []( std::basic_string<char,
                         std::char_traits<char>,
                         std::allocator<char>> const&) {};
            f4(pct_encoded_view{str}.to_string());
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
    pct_encoded_view_test,
    "boost.url.pct_encoded_view");

} // urls
} // boost

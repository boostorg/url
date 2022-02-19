//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/const_string.hpp>

#include <boost/url/static_pool.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct const_string_test
{
    using A = std::allocator<char>;

    string_view big_;
    string_view big2_;
    string_view small_;
    static constexpr std::size_t C = 32;

    const_string_test() noexcept
    {
        string_view cs =
            "*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        BOOST_TEST(cs.size() > C);
        big_ = cs.substr(1);
        big2_ = cs;
        small_ = big_.substr(0, C);
        BOOST_TEST(big_.size() > C);
        BOOST_TEST(small_.size() <= C);
        BOOST_TEST(C > 3);
    }

    void
    testConstString()
    {
        // const_string()
        // ~const_string()
        {
            const_string s;
            BOOST_TEST(s == "");
            BOOST_TEST(s.empty());

            // same buffer
            BOOST_TEST(s.size() <= C);
            BOOST_TEST(
                const_string().data() ==
                const_string().data());
        }

        // const_string(string_view, InitFn)
        // ~const_string()
        {
            const_string s(small_.size(), A{},
                [&](std::size_t n, char* dest)
                {
                    small_.copy(dest, n, 0);
                });
            BOOST_TEST(s == small_);
        }

        // const_string(string_view, Allocator)
        // ~const_string()
        {
            char const* const x = "x";
            const_string s(x, A{});
            BOOST_TEST(s == "x");
            BOOST_TEST(! s.empty());

            // different buffer
            BOOST_TEST(s.size() <= C);
            BOOST_TEST(s.data() != x);
        }

        // const_string(const_string const&)
        {
            // small buffer
            {
                auto c = small_;
                const_string s1(c, A{});
                const_string s2(s1);
                BOOST_TEST(s1 == c);
                BOOST_TEST(s2 == c);
                BOOST_TEST(s2 == s1);

                // different buffer
                BOOST_TEST(s1.size() <= C);
                BOOST_TEST(s1.data() != s2.data());
            }

            // big buffer
            {
                const_string s1(big_, A{});
                const_string s2(s1);
                BOOST_TEST(s1 == big_);
                BOOST_TEST(s2 == big_);
                BOOST_TEST(s2 == s1);

                // same buffer
                BOOST_TEST(s1.size() > C);
                BOOST_TEST(s1.data() == s2.data());
            }
        }

        // operator=(const_string const&)
        {
            // small
            {
                const_string s1("abc", A{});
                const_string s2;
                BOOST_TEST(s1 == "abc");
                BOOST_TEST(s2.empty());
                s2 = s1;
                BOOST_TEST(s1 == "abc");
                BOOST_TEST(s2 == "abc");
                BOOST_TEST(s2 == s1);

                // different buffer
                BOOST_TEST(s1.size() <= C);
                BOOST_TEST(s1.data() != s2.data());
            }

            // big
            {
                const_string s1(big_, A{});
                const_string s2;
                BOOST_TEST(s1 == big_);
                BOOST_TEST(s2.empty());
                s2 = s1;
                BOOST_TEST(s1 == big_);
                BOOST_TEST(s2 == big_);
                BOOST_TEST(s2 == s1);

                // same buffer
                BOOST_TEST(s1.size() > C);
                BOOST_TEST(s1.data() == s2.data());
            }

            // big
            {
                const_string s1(small_, A{});
                const_string s2(big_, A{});
                BOOST_TEST(s1 == small_);
                BOOST_TEST(s2 == big_);
                s1 = s2;
                BOOST_TEST(s1 == big_);
                BOOST_TEST(s2 == big_);
                BOOST_TEST(s2 == s1);

                // same buffer
                BOOST_TEST(s1.size() > C);
                BOOST_TEST(s1.data() == s2.data());
            }

            // big
            {
                const_string s1(big_, A{});
                const_string s2(big2_, A{});
                BOOST_TEST(s1 == big_);
                BOOST_TEST(s2 == big2_);
                s1 = s2;
                BOOST_TEST(s1 == big2_);
                BOOST_TEST(s2 == big2_);
                BOOST_TEST(s2 == s1);

                // same buffer
                BOOST_TEST(s1.size() > C);
                BOOST_TEST(s1.data() == s2.data());
            }

            // operator= (coverage)
            {
                const_string s1(big_, A{});
                const_string s2(small_, A{});
                s1 = s2;
                BOOST_TEST(s1 == s2);
            }

            // operator= (self-assignment)
            {
                const_string s(small_, A{});
                const_string const& cs(s);
                s = cs;
                BOOST_TEST(s == small_);
            }
        }
    }

    void
    testFactory()
    {
        using factory = const_string::factory;

        // factory()
        // ~factory()
        {
            factory a;
        }

        // factory(std::allocator)
        // ~factory()
        {
            factory a(A{});
        }

        // factory(Allocator)
        // ~factory()
        {
            static_pool<100> sp;
            factory a(sp.allocator());
        }

        // operator()(size_t, InitFn)
        {
            {
                const_string s;
                {
                    factory a;
                    s = a(C,
                        [this](
                            std::size_t n,
                            char* dest)
                        {
                            small_.copy(dest, n, 0);
                        });
                }
                BOOST_TEST(s == small_);
                BOOST_TEST(! s.empty());
            }
            {
                const_string s;
                {
                    factory a;
                    s = a(big_.size(),
                        [this](std::size_t n, char* dest)
                        {
                            big_.copy(dest, n, 0);
                        });
                }
                BOOST_TEST(s == big_);
                BOOST_TEST(! s.empty());
            }
            {
                static_pool<100> sp;
                const_string s;
                {
                    factory a(sp.allocator());
                    s = a(C,
                        [this](
                            std::size_t n,
                            char* dest)
                        {
                            small_.copy(dest, n, 0);
                        });
                }
                BOOST_TEST(s == small_);
                BOOST_TEST(! s.empty());
            }
            {
                static_pool<1000> sp;
                const_string s;
                {
                    factory a(sp.allocator());
                    s = a(big_.size(),
                        [this](
                            std::size_t n,
                            char* dest)
                        {
                            big_.copy(dest, n, 0);
                        });
                }
                BOOST_TEST(s == big_);
                BOOST_TEST(! s.empty());
            }
        }

        // operator()(string_view)
        {
            const char* const abc = "abc";

            {
                const_string s;
                {
                    factory a;
                    s = a(abc);
                }
                BOOST_TEST(s == "abc");
                BOOST_TEST(! s.empty());

                // different buffer
                BOOST_TEST(s.data() != abc);
            }
            {
                static_pool<100> sp;
                const_string s;
                {
                    factory a(sp.allocator());
                    s = a(abc);
                }
                BOOST_TEST(s == "abc");
                BOOST_TEST(! s.empty());

                // different buffer
                BOOST_TEST(s.data() != abc);
            }
        }
    }

    template <class SV>
    void
    testSvFunctions()
    {
        std::allocator<char> a;
        const_string s("test string", a);

        // iterators
        BOOST_TEST(*s.begin() == 't');
        BOOST_TEST(*s.cbegin() == 't');
        BOOST_TEST(*std::prev(s.end()) == 'g');
        BOOST_TEST(*std::prev(s.cend()) == 'g');
        BOOST_TEST(*s.rbegin() == 'g');
        BOOST_TEST(*s.crbegin() == 'g');
        BOOST_TEST(*std::prev(s.rend()) == 't');
        BOOST_TEST(*std::prev(s.crend()) == 't');

        // element access
        BOOST_TEST(s[0] == 't');
        BOOST_TEST(s.at(0) == 't');
        BOOST_TEST(s.front() == 't');
        BOOST_TEST(s.back() == 'g');
        BOOST_TEST(*s.data() == 't');

        // capacity
        BOOST_TEST(s.size() == 11);
        BOOST_TEST(s.length() == 11);
        BOOST_TEST(s.max_size() == std::size_t(-1));
        BOOST_TEST(!s.empty());

        // operations
        char dest[20];
        s.copy(dest, 7);
        BOOST_TEST(string_view(dest, 5) == "test ");
        s.copy(dest, 7, 1);
        BOOST_TEST(string_view(dest, 5) == "est s");
        BOOST_TEST(s.substr() == "test string");
        BOOST_TEST(s.substr(2) == "st string");
        BOOST_TEST(s.substr(2, 2) == "st");
        BOOST_TEST(s.compare(string_view("test string")) == 0);
        BOOST_TEST(s.compare(string_view("a")) > 0);
        BOOST_TEST(s.compare(string_view("a")) > 0);
        BOOST_TEST(s.compare(string_view("z")) < 0);
        BOOST_TEST(s.compare(0, 4, string_view("test")) == 0);
        BOOST_TEST(s.compare(0, 4, string_view("a")) > 0);
        BOOST_TEST(s.compare(0, 4, string_view("z")) < 0);
        BOOST_TEST(s.compare(0, 4, string_view("test other"), 0, 4) == 0);
        BOOST_TEST(s.compare(0, 4, string_view("a other"), 0, 1) > 0);
        BOOST_TEST(s.compare(0, 4, string_view("z other"), 0, 1) < 0);
        BOOST_TEST(s.compare(0, 4, string_view("z other"), 0, 1) < 0);
        BOOST_TEST(s.compare(0, 4, string_view("the test other"), 4, 4) == 0);
        BOOST_TEST(s.compare(0, 4, string_view("the a other"), 4, 1) > 0);
        BOOST_TEST(s.compare(0, 4, string_view("the z other"), 4, 1) < 0);
        BOOST_TEST(s.compare(0, 4, string_view("the z other"), 4, 1) < 0);
        BOOST_TEST(s.compare("test string") == 0);
        BOOST_TEST(s.compare("a") > 0);
        BOOST_TEST(s.compare("z") < 0);
        BOOST_TEST(s.compare(0, 4, "test") == 0);
        BOOST_TEST(s.compare(0, 4, "a") > 0);
        BOOST_TEST(s.compare(0, 4, "z") < 0);
        BOOST_TEST(s.compare(0, 4, "test other", 4) == 0);
        BOOST_TEST(s.compare(0, 4, "a other", 1) > 0);
        BOOST_TEST(s.compare(0, 4, "z other", 1) < 0);
        BOOST_TEST(s.compare(0, 4, "z other", 1) < 0);

        // conversions
        // const_string -> string_view
        auto sv_fn = [](SV sv) {
            return sv.size();
        };
        BOOST_TEST(sv_fn(s) == 11);

        // const_string -> std::string
        auto str_fn = [](std::string const& str) {
            return str.size();
        };
        BOOST_TEST(str_fn(s) == 11);

        // string <- const_string
        {
            std::string str(s);
            (void) str;
        }
        {
            std::string str = s;
            (void) str;
        }

        // string_view <- const_string
        {
            SV sv(s);
            (void) sv;
        }
        {
            SV sv = s;
            (void) sv;
        }

        // A(string_view) <- const_string
        struct A
        {
            A(SV) {}
        };
        {
            A aa(s);
            (void) aa;
        }

        // A(std::string const&) <- const_string
        struct B
        {
            B(std::string const&) {}
        };
        {
            B bb(s);
            (void) s;
        }

        // const_string <=> const_string
        {
            const_string eq_s("test string", a);
            const_string l_s("a", a);
            const_string g_s("z", a);
            BOOST_TEST(s == eq_s);
            BOOST_TEST(!(s == l_s));
            BOOST_TEST(!(s == g_s));
            BOOST_TEST(!(s != eq_s));
            BOOST_TEST(s != l_s);
            BOOST_TEST(s != g_s);
            BOOST_TEST(!(s < eq_s));
            BOOST_TEST(s < g_s);
            BOOST_TEST(!(s < l_s));
            BOOST_TEST(s <= eq_s);
            BOOST_TEST(s <= g_s);
            BOOST_TEST(!(s <= l_s));
            BOOST_TEST(!(s > eq_s));
            BOOST_TEST(!(s > g_s));
            BOOST_TEST(s > l_s);
            BOOST_TEST(s >= eq_s);
            BOOST_TEST(!(s >= g_s));
            BOOST_TEST(s >= l_s);
        }

        // const_string <=> string_view
        {
            SV eq_s = "test string";
            SV l_s = "a";
            SV g_s = "z";
            BOOST_TEST(s == eq_s);
            BOOST_TEST(!(s == l_s));
            BOOST_TEST(!(s == g_s));
            BOOST_TEST(!(s != eq_s));
            BOOST_TEST(s != l_s);
            BOOST_TEST(s != g_s);
            BOOST_TEST(!(s < eq_s));
            BOOST_TEST(s < g_s);
            BOOST_TEST(!(s < l_s));
            BOOST_TEST(s <= eq_s);
            BOOST_TEST(s <= g_s);
            BOOST_TEST(!(s <= l_s));
            BOOST_TEST(!(s > eq_s));
            BOOST_TEST(!(s > g_s));
            BOOST_TEST(s > l_s);
            BOOST_TEST(s >= eq_s);
            BOOST_TEST(!(s >= g_s));
            BOOST_TEST(s >= l_s);
        }

        // string_view <=> const_string
        {
            SV eq_s = "test string";
            SV l_s = "a";
            SV g_s = "z";
            BOOST_TEST(eq_s == s);
            BOOST_TEST(!(l_s == s));
            BOOST_TEST(!(g_s == s));
            BOOST_TEST(!(eq_s != s));
            BOOST_TEST(l_s != s);
            BOOST_TEST(g_s != s);
            BOOST_TEST(!(eq_s < s));
            BOOST_TEST(!(g_s < s));
            BOOST_TEST(l_s < s);
            BOOST_TEST(eq_s <= s);
            BOOST_TEST(!(g_s <= s));
            BOOST_TEST(l_s <= s);
            BOOST_TEST(!(eq_s > s));
            BOOST_TEST(g_s > s);
            BOOST_TEST(!(l_s > s));
            BOOST_TEST(eq_s >= s);
            BOOST_TEST(g_s >= s);
            BOOST_TEST(!(l_s >= s));
        }
    }

    void
    run()
    {
        testConstString();
        testFactory();
        testSvFunctions<boost::core::string_view>();
#ifndef BOOST_URL_NO_CXX17_HDR_STRING_VIEW
        testSvFunctions<std::string_view>();
#endif
    }
};

TEST_SUITE(
    const_string_test,
    "boost.url.const_string");

} // urls
} // boost

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
    static constexpr std::size_t C =
        const_string::builtin_capacity;

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

    void
    run()
    {
        testConstString();
        testFactory();
    }
};

TEST_SUITE(
    const_string_test,
    "boost.url.const_string");

} // urls
} // boost

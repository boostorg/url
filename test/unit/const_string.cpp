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
            BOOST_TEST(
                const_string().data() ==
                const_string().data());
        }

        // const_string(string_view, InitFn)
        // ~const_string()
        {
            const_string s(3, A{},
                [](std::size_t, char* dest)
                {
                    dest[0] = 'a';
                    dest[1] = 'b';
                    dest[2] = 'c';
                });
            BOOST_TEST(s == "abc");
            BOOST_TEST(! s.empty());
        }

        // const_string(string_view, Allocator)
        // ~const_string()
        {
            char const* const x = "x";
            const_string s(x, A{});
            BOOST_TEST(s == "x");
            BOOST_TEST(! s.empty());

            // different buffer
            BOOST_TEST(s.data() != x);
        }

        // const_string(const_string const&)
        {
            const_string s1("abc", A{});
            const_string s2(s1);
            BOOST_TEST(s1 == "abc");
            BOOST_TEST(s2 == "abc");
            BOOST_TEST(s2 == s1);

            // same buffer
            BOOST_TEST(s1.data() == s2.data());
        }

        // operator=(const_string const&)
        {
            const_string s1("abc", A{});
            const_string s2;
            BOOST_TEST(s1 == "abc");
            BOOST_TEST(s2.empty());
            s2 = s1;
            BOOST_TEST(s1 == "abc");
            BOOST_TEST(s2 == "abc");
            BOOST_TEST(s2 == s1);

            // same buffer
            BOOST_TEST(s1.data() == s2.data());
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
                    s = a(3,
                        [](std::size_t, char* dest)
                        {
                            dest[0] = 'a';
                            dest[1] = 'b';
                            dest[2] = 'c';
                        });
                }
                BOOST_TEST(s == "abc");
                BOOST_TEST(! s.empty());
            }
            {
                static_pool<100> sp;
                const_string s;
                {
                    factory a(sp.allocator());
                    s = a(3,
                        [](std::size_t, char* dest)
                        {
                            dest[0] = 'a';
                            dest[1] = 'b';
                            dest[2] = 'c';
                        });
                }
                BOOST_TEST(s == "abc");
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

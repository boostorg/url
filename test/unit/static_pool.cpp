//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/static_pool.hpp>

#include <boost/url/string_view.hpp>
#include "test_suite.hpp"

#include <iostream>

namespace boost {
namespace urls {

class static_pool_test
{
public:
    template<class Allocator =
        std::allocator<char>>
    static
    std::basic_string<char, std::char_traits<char>, Allocator>
    make_string(
        string_view s,
        Allocator const& a = {})
    {
        return {s.begin(), s.end(), a};
    }

    void
    run()
    {
        string_view s;
        s = "abcdefghijklmnopqrstuvwxyzabcdefghijk";
        {
            std::string s0;
            BOOST_TEST(s0.capacity() <
                s.size());
        }
        BOOST_TEST(
            make_string("xyz") == "xyz");
        {
            static_pool<4096> sp;
            BOOST_TEST(make_string(
                "xyz", sp.allocator()) == "xyz");
        }
        {
            static_pool<4> sp;
            BOOST_TEST_THROWS(
                make_string(s, sp.allocator()),
                std::exception);
        }
        {
            static_pool<1024> sp;
            static_pool_allocator<int> a =
                sp.allocator();
            BOOST_TEST_EQ(sp.allocator(), a);
            static_pool<1024> sp2;
            BOOST_TEST(sp.allocator() !=
                sp2.allocator());
        }
        {
            static_pool<1024> sp;
            static_pool_allocator<char> a =
                sp.allocator();
            auto p1 = a.allocate(1);
            auto p2 = a.allocate(2);
            auto p3 = a.allocate(3);
            auto p4 = a.allocate(4);
            a.deallocate(p3, 3);
            a.deallocate(p1, 1);
            a.deallocate(p4, 4);
            a.deallocate(p2, 2);
        }
    }
};

TEST_SUITE(static_pool_test, "boost.url.static_pool");

} // urls
} // boost

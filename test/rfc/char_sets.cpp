//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/string.hpp>

#include "test_suite.hpp"
#include <array>
#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class char_sets_test
{
public:
    using table_type = std::array<
        std::uint8_t, 256>;

    // This is used to generate the
    // initializer for the char table
    void
    build_table(table_type& v)
    {
        string_view unreserved =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~";
        string_view sub_delims = "!$&'()*+,;=";
        string_view gen_delims = ":/?#[]@";
        string_view question = "?";
        string_view colon = ":";
        string_view slash = "/";
        string_view at = "@";

        v = {};
        for(std::uint8_t c : unreserved) v[c] |= 0x01;
        for(std::uint8_t c : sub_delims) v[c] |= 0x02;
        for(std::uint8_t c : gen_delims) v[c] |= 0x04;
        for(std::uint8_t c : question)   v[c] |= 0x08;
        for(std::uint8_t c : colon)      v[c] |= 0x10;
        for(std::uint8_t c : slash)      v[c] |= 0x20;
        for(std::uint8_t c : at)         v[c] |= 0x40;
    }

    void
    print_table()
    {
        test_suite::debug_stream dout(std::cout);

        table_type tab;
        build_table(tab);
        for(int i = 0; i < 256; i += 16)
        {
            dout <<
                std::hex <<
                std::setfill('0') <<
                std::setw(2) <<
  "0x" << std::setw(2) << int(tab[i+ 0]) << ", 0x" << std::setw(2) << int(tab[i+ 1]) <<
", 0x" << std::setw(2) << int(tab[i+ 2]) << ", 0x" << std::setw(2) << int(tab[i+ 3]) <<
", 0x" << std::setw(2) << int(tab[i+ 4]) << ", 0x" << std::setw(2) << int(tab[i+ 5]) <<
", 0x" << std::setw(2) << int(tab[i+ 6]) << ", 0x" << std::setw(2) << int(tab[i+ 7]) <<
", 0x" << std::setw(2) << int(tab[i+ 8]) << ", 0x" << std::setw(2) << int(tab[i+ 9]) <<
", 0x" << std::setw(2) << int(tab[i+10]) << ", 0x" << std::setw(2) << int(tab[i+11]) <<
", 0x" << std::setw(2) << int(tab[i+12]) << ", 0x" << std::setw(2) << int(tab[i+13]) <<
", 0x" << std::setw(2) << int(tab[i+14]) << ", 0x" << std::setw(2) << int(tab[i+15]);
            if(i < 240)
                dout << ",";
            dout << std::endl;
        }
        dout.flush();
    }

    // Verify that a char_set contains
    // exactly the specified characters
    template<class T>
    void
    check(T cs, string_view s)
    {
        int n = 0;
        unsigned char u = 0;
        do
        {
            char const c =
                static_cast<char>(u);
            if(cs.contains(c))
                ++n;
        }
        while(++u != 0);
        BOOST_TEST(n == s.size());
        for(char c : s)
            BOOST_TEST(cs.contains(c));
    }

    void
    run()
    {
        //print_table();

        check(masked_char_set<
            unreserved_char_mask>(),
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~");

        check(masked_char_set<
            sub_delims_char_mask>(),
            "!$&'()*+,;=");

        check(masked_char_set<
            gen_delims_char_mask>(),
            ":/?#[]@");

        check(masked_char_set<
            colon_char_mask>(),
            ":");

        check(masked_char_set<
            at_char_mask>(),
            "@");

        check(masked_char_set<
            slash_char_mask>(),
            "/");

        check(masked_char_set<
            question_char_mask>(),
            "?");
    }
};

TEST_SUITE(
    char_sets_test,
    "boost.url.char_sets");

} // rfc
} // urls
} // boost

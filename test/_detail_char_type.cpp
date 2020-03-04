//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/url/detail/char_type.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <string>

namespace boost {
namespace urls {
namespace detail {

class char_type_test
{
public:
    static
    std::string
    sort(string_view s0) noexcept
    {
        std::string s(
            s0.data(), s0.size());
        std::sort(
            s.begin(), s.end());
        return s;
    }

    static
    void
    check_encoding(
        string_view special,
        pct_encoding e)
    {
        auto p = &special[0];
        for(int i = 0; i < 256; ++i)
        {
            auto const c =
                static_cast<char>(i);
            if(c == *p)
            {
                ++p;
                BOOST_TEST(! e.is_special(c));
            }
            else
            {
                BOOST_TEST(e.is_special(c));
            }
        }
    }

    void
    testEncodings()
    {
        // reg_name
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;="
            );
            check_encoding(
                special, reg_name_pct_set());
            check_encoding(special,
                userinfo_nc_pct_set());
        }

        // userinfo
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;="
                // ':'
                ":"
            );
            check_encoding(special,
                userinfo_pct_set());
        }

        // pchar
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;="
                // ':' / '@'
                ":@"
            );
            check_encoding(
                special, pchar_pct_set());
        }

        // pchar_nc
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;="
                // '@'
                "@"
            );
            check_encoding(
                special, pchar_nc_pct_set());
        }

        // query
        // fragment
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;="
                // ':' / '@' / '/' / '?'
                ":@/?"
            );
            check_encoding(
                special, frag_pct_set());
            check_encoding(
                special, query_pct_set());
        }

        // qkey
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$&'()*+,;" // minus '='
                // ':' / '@' / '/' / '?'
                ":@/?"
            );
            check_encoding(
                special, qkey_pct_set());
        }

        // qval
        {
            auto const special = sort(
                // unreserved
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "-._~"
                // sub-delims
                "!$'()*+,;=" // minus '&'
                // ':' / '@' / '/' / '?'
                ":@/?"
            );
            check_encoding(
                special, qval_pct_set());
        }
    }

    void
    run()
    {
        testEncodings();
    }
};

TEST_SUITE(char_type_test, "boost.url.char_type");

} // detail
} // urls
} // boost

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/url/detail/char_type.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <algorithm>
#include <string>

namespace boost {
namespace url {
namespace detail {

class char_type_test : public beast::unit_test::suite
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
                BEAST_EXPECT(! e.is_special(c));
            }
            else
            {
                BEAST_EXPECT(e.is_special(c));
            }
        }
    }

    void
    testEncodings()
    {
        // reg-name
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

        // query / fragment
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
    }

    void
    run() override
    {
        testEncodings();
    }
};

BEAST_DEFINE_TESTSUITE(boost,url,char_type);

} // detail
} // url
} // boost

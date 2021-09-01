//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/sequence.hpp>

#include <boost/url/string.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace bnf {

class sequence_test
{
public:
    template<char C>
    struct CharLiteral
    {
        char const*
        operator->() const noexcept
        {
            return &c_;
        }

        char
        operator*() const noexcept
        {
            return c_;
        }

        char const*
        parse(
            char const* const start,
            char const* const end,
            error_code& ec)
        {
            if(start == end)
            {
                ec = error::need_more;
                return start;
            }
            if(*start != C)
            {
                ec = error::syntax;
                return start;
            }
            c_ = C;
            return start + 1;
        }

    private:
        char c_ = {};
    };

    void
    run()
    {
        error_code ec;
        string_view s = "12";
        sequence<
            CharLiteral<'1'>,
            CharLiteral<'2'>> p;
        p.parse(
            s.data(),
            s.data() + s.size(),
            ec);
        BOOST_TEST(*get<0>(p)=='1');
        BOOST_TEST(*get<1>(p)=='2');
        BOOST_TEST(p[0] == "1");
        BOOST_TEST(p[1] == "2");
    }
};

TEST_SUITE(
    sequence_test,
    "boost.url.sequence");

} // bnf
} // urls
} // boost

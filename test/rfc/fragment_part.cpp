//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/fragment_part.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class fragment_part_test
{
public:
    struct T
    {
        optional<
            pct_encoded_str> v;

        friend
        bool
        parse(
            char const*& it,
            char const* const end,
            error_code& ec,
            T& t)
        {
            using bnf::parse;
            return parse(it, end, ec,
                fragment_part{t.v});
        }
    };

    void
    run()
    {
        bad<T>("#/");
        bad<T>("#?");
        bad<T>("##");
        bad<T>("#[");
        bad<T>("#]");
        bad<T>("#%F");

        good<T>("");
        good<T>("#");
        good<T>("#@");
        good<T>("#.%ff");
    }
};

TEST_SUITE(
    fragment_part_test,
    "boost.url.fragment_part");

} // rfc
} // urls
} // boost

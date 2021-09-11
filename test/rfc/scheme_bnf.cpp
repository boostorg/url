//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/scheme_bnf.hpp>

#include <boost/url/bnf/parse.hpp>
#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class scheme_bnf_test
{
public:
    void
    check(
        string_view s,
        scheme id)
    {
        error_code ec;
        scheme_bnf p;
        using bnf::parse_string;
        if(! BOOST_TEST(
            parse_string(
                s, ec, p)))
            return;
        BOOST_TEST(
            ! ec.failed());
        BOOST_TEST(
            p.id == id);
    }

    void
    run()
    {
        using T = scheme_bnf;

        bad<T>("");
        bad<T>("1");
        bad<T>(" ");
        bad<T>(" http");
        bad<T>("http ");
        bad<T>("nope:");

        check ("http", scheme::http);
        check ("HTTP", scheme::http);
        check ("HtTp", scheme::http);
        check ("a1steak", scheme::unknown);
    }
};

TEST_SUITE(
    scheme_bnf_test,
    "boost.url.scheme_bnf");

} // urls
} // boost

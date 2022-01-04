//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/scheme_rule.hpp>

#include <boost/url/grammar/parse.hpp>
#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {

class scheme_rule_test
{
public:
    void
    check(
        string_view s,
        scheme id)
    {
        error_code ec;
        scheme_rule p;
        if(! BOOST_TEST(
            grammar::parse_string(
                s, ec, p)))
            return;
        BOOST_TEST(
            ! ec.failed());
        BOOST_TEST(
            p.scheme_id == id);
    }

    void
    run()
    {
        using T = scheme_rule;

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
    scheme_rule_test,
    "boost.url.scheme_rule");

} // urls
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/query_bnf.hpp>

#include <boost/url/bnf/range.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <algorithm>

namespace boost {
namespace urls {

class query_bnf_test
{
public:
    void
    check(
        string_view s,
        std::initializer_list<
            params_value_type> i)
    {
        query_bnf t;
        if(! BOOST_TEST_NO_THROW(
            bnf::parse_string(s, t)))
            return;
        if(! BOOST_TEST(t.v.size() == i.size()))
            return;
        BOOST_TEST(std::equal(
            i.begin(),
            i.end(),
            t.v.begin()));
    }

    void
    testParse()
    {
        check("", {{}});
        check("&", {{},{}});
        check("x", {{"x"}});
        check("x&", {{"x"},{}});
        check("x=", {{"x","",true}});
        check("x=y", {{"x","y",true}});
        check("a=b&c=d", {{"a","b",true},{"c","d",true}});
    }

    void
    run()
    {
        testParse();

        using T = query_bnf;

        bad <T>("%");

        good<T>("");
        good<T>("x");
        good<T>("x=");
        good<T>("x=y");
        good<T>("x=y&");
        good<T>("x=y&a");
        good<T>("x=y&a=b&");

        // some gen-delims
        for(auto c :
            "#[]"
            )
        {
            string_view s( &c, 1 );
            bad<T>(s);
        }

        // pchar / "/" / "?"
        good<T>(
            // unreserved
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~"
            // sub-delims
            "!$&'()*+,;="
            // ":" / "@"
            ":@"
            // "/" / "?"
            "/?"
            );
    }
};

TEST_SUITE(
    query_bnf_test,
    "boost.url.query_bnf");

} // urls
} // boost

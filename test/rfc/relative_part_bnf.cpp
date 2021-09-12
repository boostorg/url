//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/relative_part_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class relative_part_bnf_test
{
public:
    void
    run()
    {       
        using T = relative_part_bnf;

        bad<T>(":/"); // colon not ok in relative-part

        good<T>("");
        good<T>("/");
        good<T>("//");
        good<T>("//user:pass@");
        good<T>("//boost.org");
        good<T>("//1.2.3.4:8080");
        good<T>("//1.2.3.4:8080/");
        good<T>("//1.2.3.4:8080/x");
        good<T>("//1.2.3.4:8080/x/");
        good<T>("//1.2.3.4:8080////");
        good<T>("/x");
        good<T>("/x/");
        good<T>("/x/y");
        good<T>("/x/y//");
        good<T>("x");
        good<T>("x/");
        good<T>("x//");
        good<T>("x/y/z");
        good<T>("x//y///z///");

        error_code ec;
        relative_part_bnf t;
        bnf::parse_string(
            "/1/2/3/4/5", ec, t);
        BOOST_TEST(t.path == "/1/2/3/4/5");
        BOOST_TEST(t.path_count == 5);
    }
};

TEST_SUITE(
    relative_part_bnf_test,
    "boost.url.relative_part_bnf");

} // urls
} // boost

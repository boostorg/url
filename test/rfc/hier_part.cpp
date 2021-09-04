//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/hier_part.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

#include <iostream>

namespace boost {
namespace urls {
namespace rfc {

class hier_part_test
{
public:
    void
    run()
    {
        using T = hier_part;
        
        bad_ <T>("/");

        good_<T>("//");
        good_<T>("//user:pass@");
        good_<T>("//boost.org");
        good_<T>("//1.2.3.4:8080");
        good_<T>("//1.2.3.4:8080/");
        good_<T>("//1.2.3.4:8080/x");
        good_<T>("//1.2.3.4:8080/x/");
        good_<T>("//1.2.3.4:8080////");
        good_<T>("/x");
        good_<T>("/x/");
        good_<T>("/x/y");
        good_<T>("/x/y//");
        good_<T>("x");
        good_<T>("x/");
        good_<T>("x//");
        good_<T>("x/y/z");
        good_<T>("x//y///z///");
        good_<T>("");

        hier_part p;
        error_code ec;
        using bnf::parse;
        parse("/1/2/3/4/5", ec, p);
        for(auto const& t : p.path())
            std::cout << t.str() << std::endl;
    }
};

TEST_SUITE(
    hier_part_test,
    "boost.url.hier_part");

} // rfc
} // urls
} // boost

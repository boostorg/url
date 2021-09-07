//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/hier_part_bnf.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {

class hier_part_bnf_test
{
public:
    void
    run()
    {
        using T = hier_part_bnf;
        
        bad <T>("/");

        good<T>({
            ""
            "//",
            "//user:pass@",
            "//boost.org",
            "//1.2.3.4:8080",
            "//1.2.3.4:8080/",
            "//1.2.3.4:8080/x",
            "//1.2.3.4:8080/x/",
            "//1.2.3.4:8080////",
            "/x",
            "/x/",
            "/x/y",
            "/x/y//",
            "x",
            "x/",
            "x//",
            "x/y/z",
            "x//y///z///"
            ":/" // colon ok in hier-part
            });

        hier_part_bnf p;
        error_code ec;
        using bnf::parse;
        parse("/1/2/3/4/5", ec, p);
        std::string s;
        for(auto const& t : p.path)
        {
            s.push_back('/');
            s.append(
                t.str.data(),
                t.str.size());
        }
        BOOST_TEST(s == "/1/2/3/4/5");
    }
};

TEST_SUITE(
    hier_part_bnf_test,
    "boost.url.hier_part_bnf");

} // urls
} // boost

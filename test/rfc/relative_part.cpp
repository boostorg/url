//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/relative_part.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class relative_part_test
{
public:
    void
    run()
    {       
        bad<relative_part>({
            "/",
            ":/" // colon not ok in relative-part
            });

        good<relative_part>({
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
            });

        relative_part p;
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
    relative_part_test,
    "boost.url.relative_part");

} // rfc
} // urls
} // boost

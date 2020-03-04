//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/url/detail/parse.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <string>

namespace boost {
namespace urls {
namespace detail {

class parse_test
{
public:
    void
    good(
        string_view s)
    {
        error_code ec;
        parts pt;
        parse_url(pt, s, ec);
        BOOST_TEST(! ec);
    }

    void
    bad(
        string_view s)
    {
        error_code ec;
        parts pt;
        parse_url(pt, s, ec);
        BOOST_TEST(!! ec);
    }

    void
    testParse()
    {
        good("http:");
        good("http://");
        good("http://:");
        good("http://example.com");
        good("http://example.com:");
        good("http://example.com:443");
        good("http://:443");
        good("http://example.com/");
        good("example://a/.//b/%2E%2E%2F/b/c/");
        good("http://example.com:443/path");
        good("http://example.com:443/path/to/file.txt");
        good("http://example.com:443/path/to/file.txt?query");
        good("http://example.com:443/path/to/file.txt");
        good("http://example.com:443/path/to/file.txt?query");

        good("http://example.com:443/path/to/file.txt?query");

        good("/path/to/file.txt?query#frag");

        bad("ws://%X9");
        bad("1badscheme://");
    }

    void
    run()
    {
        testParse();
    }
};

TEST_SUITE(parse_test, "boost.url.parse");

} // detail
} // urls
} // boost

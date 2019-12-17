//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/url/detail/parser.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <string>

namespace boost {
namespace url {
namespace detail {

class parse_test
{
public:
    void
    good_uri_ref(
        string_view s)
    {
        error_code ec;
        parser pr(s);
        pr.parse_uri_reference(ec);
        BOOST_TEST(! ec);
    }

    void
    bad_uri_ref(
        string_view s)
    {
        error_code ec;
        parser pr(s);
        pr.parse_uri_reference(ec);
        BOOST_TEST(!! ec);
    }

    void
    testParse()
    {
        good_uri_ref("http:");
        good_uri_ref("http://");
        good_uri_ref("http://:");
        good_uri_ref("http://example.com");
        good_uri_ref("http://example.com:");
        good_uri_ref("http://example.com:443");
        good_uri_ref("http://:443");
        good_uri_ref("http://example.com/");
        good_uri_ref("example://a/.//b/%2E%2E%2F/b/c/");
        good_uri_ref("http://example.com:443/path");
        good_uri_ref("http://example.com:443/path/to/file.txt");
        good_uri_ref("http://example.com:443/path/to/file.txt?query");
        good_uri_ref("http://example.com:443/path/to/file.txt#frag");
        good_uri_ref("http://example.com:443/path/to/file.txt?query#frag");

        bad_uri_ref("1badscheme://");
    }

    void
    run()
    {
        testParse();
    }
};

TEST_SUITE(parse_test, "boost.url.parser");

} // detail
} // url
} // boost

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#include <boost/url.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class limits_test
{
public:
    void
    test_url_view()
    {
        string_view s = "http://www.example.com/path/to/file.txt";

        BOOST_TEST_THROWS(
            parse_absolute_uri(s),
            std::length_error);

        BOOST_TEST_THROWS(
            parse_uri(s),
            std::length_error);

        BOOST_TEST_THROWS(
            parse_relative_ref(s),
            std::length_error);

        BOOST_TEST_THROWS(
            parse_uri_reference(s),
            std::length_error);
    }

    void
    run()
    {
        test_url_view();
    }
};

TEST_SUITE(
    limits_test,
    "boost.url.limits");

} // urls
} // boost

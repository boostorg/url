//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/params_const_view.hpp>

#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct params_const_view_test
{
    void
    testJavadocs()
    {
        // class
        {
    url_view u( "?first=John&last=Doe" );

    params_const_view p = u.params();

    ignore_unused(p);
        }
    }

    void
    run()
    {
        testJavadocs();
    }
};

TEST_SUITE(
    params_const_view_test,
    "boost.url.params_const_view");

} // urls
} // boost

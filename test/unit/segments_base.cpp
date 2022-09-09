//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/segments_base.hpp>

#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct segments_base_test
{
    void
    testJavadoc()
    {
        // value_type
        {
        segments_base::value_type ps( url_view( "/path/to/file.txt" ).segments().back() );

        ignore_unused(ps);
        }

        // buffer()
        {
        assert( url_view( "/path/to/file.txt" ).segments().buffer() == "/path/to/file.txt" );
        }

        // is_absolute()
        {
        assert( url_view( "/path/to/file.txt" ).segments().is_absolute() == true );
        }

        // empty()
        {
        assert( ! url_view( "/index.htm" ).segments().empty() );
        }

        // size()
        {
        assert( url_view( "/path/to/file.txt" ).segments().size() == 3 );
        }

        // front()
        {
        assert( url_view( "/path/to/file.txt" ).segments().front() == "path" );
        }

        // back()
        {
        assert( url_view( "/path/to/file.txt" ).segments().back() == "file.txt" );
        }
    }

    void
    run()
    {
        testJavadoc();
    }
};

TEST_SUITE(
    segments_base_test,
    "boost.url.segments_base");

} // urls
} // boost

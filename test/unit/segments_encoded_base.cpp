//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_encoded_base.hpp>

#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct segments_encoded_base_test
{
    void
    testJavadocs()
    {
        // value_type
        {
        segments_encoded_base::value_type ps( *url_view( "/path/to/file.txt" ).encoded_segments().back() );

        ignore_unused(ps);
        }

        // buffer()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().buffer() == "/path/to/file.txt" );
        }

        // is_absolute()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().is_absolute() == true );
        }

        // empty()
        {
        assert( ! url_view( "/index.htm" ).encoded_segments().empty() );
        }

        // size()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().size() == 3 );
        }

        // front()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().front() == "path" );
        }

        // back()
        {
        assert( url_view( "/path/to/file.txt" ).encoded_segments().back() == "file.txt" );
        }
    }

    void
    run()
    {
        testJavadocs();
    }
};

TEST_SUITE(
    segments_encoded_base_test,
    "boost.url.segments_encoded_base");

} // urls
} // boost

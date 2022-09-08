//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/segments_view.hpp>

#include <boost/url/parse_path.hpp>
#include <boost/url/url_view.hpp>
#include <boost/static_assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(
    ! std::is_default_constructible<
        segments_encoded_view>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        segments_encoded_view>::value);

BOOST_STATIC_ASSERT(
    ! std::is_copy_assignable<
        segments_encoded_view>::value);

struct segments_view_test
{
    void
    testMembers()
    {
        // segments_view(
        //      segments_view const&)
        {
            segments_view ps0 =
                parse_path("/path/to/file.txt").value();
            segments_view ps1(ps0);
            BOOST_TEST_EQ(
                ps0.buffer().data(), ps1.buffer().data());
        }

        // ostream
        {
            segments_view ps = parse_path(
                "/path/to/file.txt").value();
            std::stringstream ss;
            ss << ps;
            BOOST_TEST_EQ(ss.str(),
                "/path/to/file.txt");
        }
    }

    void
    testJavadocs()
    {
        // {class}
        {
    url_view u( "/path/to/file.txt" );

    segments_view ps = u.segments();

    assert( ps.buffer().data() == u.string().data() );

    ignore_unused(ps);
        }
    }

    void
    run()
    {
        testMembers();
        testJavadocs();
    }
};

TEST_SUITE(
    segments_view_test,
    "boost.url.segments_view");

} // urls
} // boost

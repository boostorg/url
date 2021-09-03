//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATH_ABEMPTY_HPP
#define BOOST_URL_RFC_PATH_ABEMPTY_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for path-abempty

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class path_abempty
{
public:
    /*
    range<segment_> const&
    path() const noexcept
    {
        return r_;
    }
    */

    string_view const&
    operator*() const noexcept
    {
        return v_;
    }

    string_view const*
    operator->() const noexcept
    {
        return &v_;
    }

    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec)
    {
        return increment(
            start, end, ec);
    }

    BOOST_URL_DECL
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    string_view v_;
};

} // rfc
} // urls
} // boost

#endif

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATH_NOSCHEME_HPP
#define BOOST_URL_RFC_PATH_NOSCHEME_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for path-noscheme

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )

    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                  ; non-zero-length segment without any colon ":"
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class path_noscheme
{
public:
    using value_type = string_view;

    value_type
    value() const noexcept
    {
        return v_;
    }

    BOOST_URL_DECL
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec);

    BOOST_URL_DECL
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    value_type v_;
};

} // rfc
} // urls
} // boost

#endif

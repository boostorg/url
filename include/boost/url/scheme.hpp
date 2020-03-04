//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_SCHEME_HPP
#define BOOST_URL_SCHEME_HPP

#include <boost/url/config.hpp>

namespace boost {
namespace urls {

/** Identifies a special URL scheme.
*/
enum class scheme : unsigned char
{
    unknown = 0,

    ftp,
    file,
    http,
    https,
    ws,
    wss
};

/** Return the scheme for a non-normalized string, if known
*/
BOOST_URL_DECL
scheme
string_to_scheme(string_view s) noexcept;

/** Return the normalized string for a known scheme
*/
BOOST_URL_DECL
string_view
to_string(scheme s) noexcept;

/** Return `true` if the scheme string is a special scheme.

    The list of special schemes is as follows:
    ftp, file, http, https, ws, wss.

    @param s The string constant to check

    @return `true` if the scheme is special
*/
inline
bool
is_special(string_view s) noexcept
{
    return string_to_scheme(s) != scheme::unknown;
}

} // urls
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/scheme.ipp>
#endif

#endif

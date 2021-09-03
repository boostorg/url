//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_USERINFO_HPP
#define BOOST_URL_RFC_USERINFO_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/optional.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for userinfo

    @par BNF
    @code
    userinfo    = user [ ":" [ password ] ]

    user        = *( unreserved / pct-encoded / sub-delims )
    password    = *( unreserved / pct-encoded / sub-delims / ":" )
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class userinfo
{
    string_view str_;
    pct_encoded_value user_;
    optional<
        pct_encoded_value> pass_;

public:
    string_view
    str() const noexcept
    {
        return str_;
    }

    pct_encoded_value const&
    user() const noexcept
    {
        return user_;
    }

    optional<
        pct_encoded_value> const&
    password() const noexcept
    {
        return pass_;
    }

    BOOST_URL_DECL
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        userinfo& t);
};

} // rfc
} // urls
} // boost

#endif

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_HPP
#define BOOST_URL_RFC_PCT_ENCODED_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pct_encoding.hpp>
#include <memory>

namespace boost {
namespace urls {
namespace rfc {

/** Base type for a percent-encoded BNF element
*/
class pct_encoded_value
{
    template<std::uint8_t>
    friend class pct_encoded;

    string_view s_;
    std::size_t n_ = 0;

public:
    /** Return the percent-encoded string
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    /** Return the number of bytes in the decoded representation of the string
    */
    std::size_t
    decoded_size() const noexcept
    {
        return n_;
    }
};

/** BNF for a string of percent-encoded, reserved characters

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.1
*/
template<std::uint8_t CharMask>
class pct_encoded
{
    pct_encoded_value v_;

public:
    using value_type =
        pct_encoded_value;

    pct_encoded_value const&
    operator*() const noexcept
    {
        return v_;
    }

    pct_encoded_value const*
    operator->() const noexcept
    {
        return &v_;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec) noexcept;
};
    
} // rfc
} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded.hpp>

#endif

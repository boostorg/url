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

/** BNF for pct-encoded

    @par BNF
    @code
    pct-encoded   = "%" HEXDIG HEXDIG
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.1
*/
class pct_encoded0
{
public:
    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);
};

/** Base type for a percent-encoded BNF element
*/
class pct_encoded_base
{
public:
    class value_type
    {
        template<std::uint8_t>
        friend class pct_encoded;

        string_view s_;

    public:
        string_view
        encoded_str() const noexcept
        {
            return s_;
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        str(
            Allocator const& a = {}) const
        {
            return pct_decode_unchecked(
                s_, a);
        }
    };
};

/** BNF for a string of percent-encoded, reserved characters

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.1
*/
template<std::uint8_t CharMask>
class pct_encoded
    : public pct_encoded_base
{
    value_type v_;

public:
    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
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

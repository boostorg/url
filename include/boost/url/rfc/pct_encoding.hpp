//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODING_HPP
#define BOOST_URL_RFC_PCT_ENCODING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace rfc {

template<class CharSet>
char const*
parse_pct_encoded(
    CharSet const& cs,
    char const* const start,
    char const* const end,
    error_code& ec) noexcept;

/** Returns the size of string s if it was percent-decoded, with error checking
*/
template<class CharSet>
std::size_t
pct_decoded_size(
    CharSet const& cs,
    string_view s,
    error_code& ec) noexcept;

/** Returns the size of string s if it was percent-decoded, without error checking

    @par Preconditions
    `s` is a valid encoded string
*/
BOOST_URL_DECL
std::size_t
pct_decoded_size_unchecked(
    string_view s) noexcept;

/** Write string s with percent-decoding applied, to dest

    @par Preconditions
    @li `s` is a valid encoded string
    @li `dest` has sufficient space for the decoded string

    @return A pointer to one past the last character written.

    @see
        @ref pct_decoded_size
*/
BOOST_URL_DECL
char*
pct_decode_unchecked(
    char *dest,
    string_view s) noexcept;

/** Return a percent-decoded string, without error checking

    @par Preconditions
    `es` is a valid encoded string
*/
template<class Allocator =
    std::allocator<char>>
string_type<Allocator>
pct_decode_unchecked(
    string_view es,
    Allocator const& a = {}) noexcept
{
    string_type<Allocator> s(a);
    s.resize(
        pct_decoded_size_unchecked(es));
    pct_decode_unchecked(
        &s[0], es);
    return s;
}

} // rfc
} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoding.hpp>

#endif

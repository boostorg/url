//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PCT_ENCODING_HPP
#define BOOST_URL_PCT_ENCODING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {

#if 0
template<class CharSet>
bool
parse_pct_encoded(
    char const*& start,
    char const* const end,
    error_code& ec,
    CharSet const& cs) noexcept;

/** Returns the size of string s if it was percent-decoded, with error checking
*/
template<class CharSet>
std::size_t
pct_decoded_size(
    string_view s,
    error_code& ec,
    CharSet const& cs) noexcept;

#endif

/** Returns the size of string s if it was percent-decoded, without error checking

    @par Preconditions
    `s` is a valid encoded string
*/
BOOST_URL_DECL
std::size_t
pct_decoded_size_unchecked(
    string_view s) noexcept;

/** Write string s with percent-decoding applied, to the range first, last

    @par Preconditions
    @li `s` is a valid encoded string
    @li The output range has space for the decoded string

    @see
        @ref pct_decoded_size
*/
BOOST_URL_DECL
void
pct_decode_unchecked(
    char* first,
    char* last,
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
    std::size_t decoded_size,
    Allocator const& a = {})
{
// VFALCO FIX ME
decoded_size = pct_decoded_size_unchecked(es);
    string_type<Allocator> s(a);
    if(decoded_size == 0)
        return s;
    s.resize(decoded_size);
    pct_decode_unchecked(&s[0],
        &s[0] + s.size(), es);
    return s;
}

/** Return true if plain equals a deecoded percent-encoded string
*/
BOOST_URL_DECL
bool
key_equal_encoded(
    string_view plain_key,
    pct_encoded_str encoded) noexcept;

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoding.hpp>

#endif

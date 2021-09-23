//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_PCT_ENCODING_HPP
#define BOOST_URL_PCT_ENCODING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_set.hpp>
#include <memory>

namespace boost {
namespace urls {

/** Return the number of bytes needed to store a string with percent-encoding removed

    This function examines the characters in
    the string to determine the number of bytes
    necessary if the string were to be percent
    decoded using the given options. No encoding
    is actually performed. Since not all encoded
    strings are valid, this function also performs
    checking to ensure that the encoding is valid
    for the character set, setting the error if
    the string is invalid.

    @par Exception Safety
    Throws nothing.

    @throw system_error if the encoded string
        is invalid.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The string to analyze.

    @param ec Set to the error, if any occurred.

    @param cs An optional character set to use.
    This type must satisfy the requirements
    of _CharSet_. If this parameter is omitted,
    then no characters are considered special.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref bnf::all_chars,
        @ref pct_decode_opts,
        @ref pct_encode.
*/
template<
    class CharSet /*= bnf::all_chars*/>
std::size_t
pct_decode_size(
    string_view s,
    error_code& ec,
    CharSet const& cs = {},
    pct_decode_opts const& opt = {}) noexcept;

/** Return a string with percent-decoding applied

    This function applies percent-decoding to
    the given string, using the specified allowed
    character set. Characters in the encoded string
    are treated thusly:

    @li Unescaped characters are copied as-is.

    @li Percent-escaped hex sequences are
        copied by inserting the corresponding
        ASCII character. The hex sequence
        must be syntactically valid or else
        an exception is thrown.

    @li Special characters; that is, characters
        that are not in the set `cs`, cause an
        exception to be thrown.

    @par Exception Safety
    Exceptions thrown on invalid input.
    Calls to allocate may throw.

    @throw system_error if the encoded string
        is invalid.

    @return The decoded string.

    @param s The string to decode.

    @param cs An optional character set to use.
    This type must satisfy the requirements
    of _CharSet_. If this parameter is omitted,
    then no characters are considered special.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @param a An optional allocator the returned
    string will use. If this parameter is omitted,
    the default allocator is used, and the return
    type of the function becomes `std::string`.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref bnf::all_chars,
        @ref pct_decode_opts,
        @ref pct_encode.
*/
template<
    class CharSet = bnf::all_chars,
    class Allocator = std::allocator<char>>
string_type<Allocator>
pct_decode(
    string_view s,
    CharSet const& cs = {},
    pct_decode_opts const& opt = {},
    Allocator const& a = {});

//------------------------------------------------

/** Return the number of bytes needed to store a string with percent-encoding applied

    This function examines the characters in
    the string to determine the number of bytes
    necessary if the string were to be percent
    encoded using the given options and character
    set. No encoding is actually performed.

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The string to encode.

    @param cs The character set to use.

    @param opt The options for encoding. If this
    parameter is omitted, the default options
    will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_encode_opts.
*/
template<class CharSet>
std::size_t
pct_encode_size(
    string_view s,
    CharSet const& cs,
    pct_encode_opts const& opt = {}) noexcept;

#ifndef BOOST_URL_DOCS
// VFALCO This would make no sense
std::size_t
pct_encode_size(
    string_view,
    bnf::all_chars const&,
    ...) = delete;
#endif

/** Return a string with percent-encoding applied

    This function returns a percent encoded
    string formed by escaping all characters
    that are not in the specified _CharSet_.

    @par Exception Safety
    Calls to allocate may throw.

    @param s The string to encode.

    @param opt The options for encoding. If this
    parameter is omitted, the default options
    will be used.

    @param cs The character set to use.

    @param a An optional allocator the returned
    string will use. If this parameter is omitted,
    the default allocator is used, and the return
    type of the function becomes `std::string`.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_encode_opts.
*/
template<
    class CharSet,
    class Allocator = std::allocator<char>
>
string_type<Allocator>
pct_encode(
    string_view s,
    CharSet const& cs,
    pct_encode_opts const& opt = {},
    Allocator const& a = {});

} // urls
} // boost

#include <boost/url/impl/pct_encoding.hpp>

#endif

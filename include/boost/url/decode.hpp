//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DECODE_HPP
#define BOOST_URL_DECODE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/decode_opts.hpp>
#include <boost/url/error_types.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/all_chars.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/decode_view.hpp>

namespace boost {
namespace urls {

namespace detail {
/** Validate a percent encoded string and return the number of decoded bytes

    This function examines the characters in
    the string to determine the number of bytes
    necessary if the string were to be percent
    decoded using the given options. No encoding
    is actually performed. Since not all encoded
    strings are valid, this function also performs
    checking to ensure that the encoding is valid
    for the character set, setting the error if
    the string is invalid.

    @par Example 1
    This validates and calculates the decoded length of a valid percent-encoded string.
    @code
    error_code ec;
    std::size_t decoded_size = validate_encoding( "Program%20Files",
            ec, pchars, decode_opts{} );
    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    @endcode

    @par Example 2
    This shows how validation can fail using an error code.
    @code
    error_code ec;
    std::size_t decoded_size = validate_encoding( "bad%escape",
            ec, pchars, decode_opts{});
    assert( ec.failed() );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The percent-encoded string to analyze.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @param allowed The set of characters
    allowed to appear unescaped.
    This type must satisfy the requirements
    of <em>CharSet</em>. If this parameter is
    omitted, then no characters are considered
    special. The character set is ignored if
    `opt.non_normal_is_error == false`.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode,
        @ref decode_opts,
*/
template<
    class CharSet>
result<std::size_t>
validate_encoding(
    string_view s,
    CharSet const& allowed,
    decode_opts const& opt = {}) noexcept;

/** Validate a percent encoded string and return the number of decoded bytes

    This function examines the characters in
    the string to determine the number of bytes
    necessary if the string were to be percent
    decoded using the given options. No encoding
    is actually performed. This function does
    not perform checking to ensure that the
    unencoded characters belong to specified
    character set.

    @par Example 1
    This validates and calculates the decoded length of a valid percent-encoded string.
    @code
    error_code ec;
    std::size_t decoded_size = validate_encoding( "Program%20Files",
            ec, decode_opts{} );
    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    @endcode

    @par Example 2
    This shows how validation can fail using an error code.
    @code
    error_code ec;
    std::size_t decoded_size = validate_encoding( "bad%escape",
            ec, decode_opts{} );
    assert( ec.failed() );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The percent-encoded string to analyze.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode,
        @ref decode_bytes_unchecked,
        @ref decode_opts,
        @ref decode_unchecked,
*/
BOOST_URL_DECL
result<std::size_t>
validate_encoding(
    string_view s,
    decode_opts const& opt = {}) noexcept;

/** Write a string with percent-decoding into a buffer.

    This function applies percent-decoding to
    the given percent-encoded string, by
    converting escape sequences into their
    character equivalent.
    The function returns the number of bytes
    written to the destination buffer, which
    may be less than the size of the output
    area.

    @par Example
    @code
    char *dest = new char[MAX_LENGTH];
    error_code ec;
    std::size_t decoded_size = decode( dest, dest + MAX_LENGTH,
            "Program%20Files", ec, pchars, decode_opts{});

    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    assert( strncmp( "Program Files", dest, decoded_size ) == 0 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes written to
    the destination buffer, which does not
    include any null termination. If the
    destination buffer is too small to hold
    the result, the result is set to
    @ref error::no_space.

    @param dest A pointer to the
    beginning of the output buffer.

    @param end A pointer to one past the end
    of the output buffer.

    @param s The string to decode.

    @param opt The options for decoding. If
    this parameter is omitted, the default
    options will be used.

    @param allowed The set of characters
    allowed to appear unescaped.
    This type must satisfy the requirements
    of <em>CharSet</em>. If this parameter is
    omitted, then no characters are considered
    special. The character set is ignored if
    `opt.non_normal_is_error == false`.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode_opts.
*/
template<
    class CharSet>
result<std::size_t>
decode(
    char* dest,
    char const* end,
    string_view s,
    CharSet const& allowed,
    decode_opts const& opt = {}) noexcept;

/** Write a string with percent-decoding into a buffer.

    This function applies percent-decoding to
    the given percent-encoded string, by
    converting escape sequences into their
    character equivalent.
    The function returns the number of bytes
    written to the destination buffer, which
    may be less than the size of the output
    area.

    @par Example
    @code
    char *dest = new char[MAX_LENGTH];
    error_code ec;
    std::size_t decoded_size = decode( dest, dest + MAX_LENGTH,
            "Program%20Files", ec, decode_opts{});

    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    assert( strncmp( "Program Files", dest, decoded_size ) == 0 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes written to
    the destination buffer, which does not
    include any null termination.

    @param dest A pointer to the
    beginning of the output buffer.

    @param end A pointer to one past the end
    of the output buffer.

    @param s The string to decode.

    @param opt The options for decoding. If
    this parameter is omitted, the default
    options will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode_bytes_unchecked,
        @ref decode_opts,
        @ref decode_unchecked.
        @ref validate_encoding.
*/
BOOST_URL_DECL
result<std::size_t>
decode(
    char* dest,
    char const* end,
    string_view s,
    decode_opts const& opt = {}) noexcept;

/** Return the number of bytes needed to hold the string with percent-decoding applied.

    This function calculates the number of
    bytes needed to apply percent-decoding to
    the encoded string `s`, excluding any
    null terminator. The caller is responsible
    for validating the input string before
    calling this function.

    @par Example
    @code
    std::size_t size = decode_bytes_unchecked( "Program%20Files" );
    assert( size == 13 );
    @endcode

    @par Preconditions
    The string `s` must contain a valid
    percent-encoding.

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed to
    apply percent-decoding, excluding any
    null terminator.

    @param s The string to inspect.
*/
BOOST_URL_DECL
std::size_t
decode_bytes_unchecked(
    string_view s) noexcept;

/** Apply percent-decoding to a string

    This function applies percent-decoding to
    the input string, without performing any
    checking to ensure that the input string
    is valid. The contents of the output
    buffer will never be left undefined,
    regardless of input.

    @par Example
    @code
    char *dest = new char[MAX_LENGTH];
    std::size_t decoded_size = decode_unchecked( dest, dest + MAX_LENGTH,
            "Program%20Files" );

    assert( decoded_size == 13 );
    assert( strncmp("Program Files", dest, decoded_size) == 0 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes written to
    the destination.

    @param dest A pointer to the beginning of
    the output buffer to hold the percent-decoded
    characters.

    @param end A pointer to one past the end
    of the output buffer.

    @param s The string to apply percent-decoding to.

    @param opt Optional settings for applying
    the decoding. If this parameter is omitted,
    default settings are used.
*/
BOOST_URL_DECL
std::size_t
decode_unchecked(
    char* dest,
    char const* end,
    string_view s,
    decode_opts const& opt = {}) noexcept;
}

/** Validate and decode a string view

    This function returns a view that applies
    percent-decoding to the given percent-encoded
    string, by converting escape sequences into
    their character equivalent.

    The function validates the input string
    and returns a view of the decoded bytes.
    If the input string is invalid, the result
    contains an error.

    @par Example
    @code
    result< decode_view > r = decode( "Program%20Files" );
    assert( r.has_value() );

    decode_view v = *r;
    assert( v == "Program Files" );
    assert( v.size() == 13 );
    assert( v.encoded().size() == 15 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes written to
    the destination buffer, which does not
    include any null termination.

    @param s The string to decode.

    @param allowed The set of characters
    allowed to appear unescaped.
    This type must satisfy the requirements
    of <em>CharSet</em>. If this parameter is
    omitted, then no characters are considered
    special. The character set is ignored if
    `opt.non_normal_is_error == false`.

    @param opt The options for decoding. If
    this parameter is omitted, the default
    options will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode_opts,
        @ref validate_encoding.
*/
template< class CharSet = grammar::all_chars_t >
result< decode_view >
decode(
    string_view s,
    decode_opts const& opt = {},
    CharSet const& allowed = {});

} // urls
} // boost

#include <boost/url/impl/decode.hpp>

#endif

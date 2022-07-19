//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
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
#include <boost/url/const_string.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <memory>

namespace boost {
namespace urls {

/** Options for removing percent-encoding from strings

    @see
        @ref pct_decode,
        @ref pct_decode_bytes_unchecked,
        @ref pct_decode_unchecked,
        @ref validate_pct_encoding.
*/
struct pct_decode_opts
{
    /** True if null characters are allowed in decoded output
    */
    bool allow_null = true;

    /** True if PLUS ('+') decodes into SP (space, ' ')

        @par Specification
        @li <a href="https://www.w3.org/TR/html401/interact/forms.html#h-17.13.4.1">
            application/x-www-form-urlencoded (w3.org)</a>
    */
    bool plus_to_space = true;

    /** True if decoding a non-normal string is an error
    */
    bool non_normal_is_error = false;
};

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
    std::size_t decoded_size = validate_pct_encoding( "Program%20Files",
            ec, pchars, pct_decode_opts{} );
    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    @endcode

    @par Example 2
    This shows how validation can fail using an error code.
    @code
    error_code ec;
    std::size_t decoded_size = validate_pct_encoding( "bad%escape",
            ec, pchars, pct_decode_opts{});
    assert( ec.failed() );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The percent-encoded string to analyze.

    @param ec Set to the error, if any occurred.

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
        @ref pct_decode,
        @ref pct_decode_bytes_unchecked,
        @ref pct_decode_opts,
        @ref pct_decode_to_value,
        @ref pct_decode_unchecked,
        @ref reserved_chars_t.
*/
template<
    class CharSet>
std::size_t
validate_pct_encoding(
    string_view s,
    error_code& ec,
    CharSet const& allowed,
    pct_decode_opts const& opt = {}) noexcept;

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
    std::size_t decoded_size = validate_pct_encoding( "Program%20Files",
            ec, pct_decode_opts{} );
    assert( ! ec.failed() );
    assert( decoded_size == 13 );
    @endcode

    @par Example 2
    This shows how validation can fail using an error code.
    @code
    error_code ec;
    std::size_t decoded_size = validate_pct_encoding( "bad%escape",
            ec, pct_decode_opts{} );
    assert( ec.failed() );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The percent-encoded string to analyze.

    @param ec Set to the error, if any occurred.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_decode_bytes_unchecked,
        @ref pct_decode_opts,
        @ref pct_decode_to_value,
        @ref pct_decode_unchecked,
        @ref reserved_chars_t.
*/
BOOST_URL_DECL
std::size_t
validate_pct_encoding(
    string_view s,
    error_code& ec,
    pct_decode_opts const& opt = {}) noexcept;

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
    std::size_t decoded_size = pct_decode( dest, dest + MAX_LENGTH,
            "Program%20Files", ec, pchars, pct_decode_opts{});

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

    @param ec Set to the error, if any
    occurred. If the destination buffer
    is too small to hold the result, `ec`
    is set to @ref error::no_space.

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
        @ref pct_decode_bytes_unchecked,
        @ref pct_decode_opts,
        @ref pct_decode_to_value.
        @ref pct_decode_unchecked.
        @ref validate_pct_encoding.
*/
template<
    class CharSet>
std::size_t
pct_decode(
    char* dest,
    char const* end,
    string_view s,
    error_code& ec,
    CharSet const& allowed,
    pct_decode_opts const& opt = {}) noexcept;

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
    std::size_t decoded_size = pct_decode( dest, dest + MAX_LENGTH,
            "Program%20Files", ec, pct_decode_opts{});

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

    @param ec Set to the error, if any occurred.

    @param opt The options for decoding. If
    this parameter is omitted, the default
    options will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode_bytes_unchecked,
        @ref pct_decode_opts,
        @ref pct_decode_to_value.
        @ref pct_decode_unchecked.
        @ref validate_pct_encoding.
*/
BOOST_URL_DECL
std::size_t
pct_decode(
    char* dest,
    char const* end,
    string_view s,
    error_code& ec,
    pct_decode_opts const& opt = {}) noexcept;

/** Return the number of bytes needed to hold the string with percent-decoding applied.

    This function calculates the number of
    bytes needed to apply percent-decoding to
    the encoded string `s`, excluding any
    null terminator. The caller is responsible
    for validating the input string before
    calling this function.

    @par Example
    @code
    std::size_t size = pct_decode_bytes_unchecked( "Program%20Files" );
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

    @see
        @ref validate_pct_encoding.
*/
BOOST_URL_DECL
std::size_t
pct_decode_bytes_unchecked(
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
    std::size_t decoded_size = pct_decode_unchecked( dest, dest + MAX_LENGTH,
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
pct_decode_unchecked(
    char* dest,
    char const* end,
    string_view s,
    pct_decode_opts const& opt = {}) noexcept;

//------------------------------------------------

/** Options for applying percent-encoding to strings.

    Instances of this type may be provided
    to percent-encoding algorithms to
    customize their behavior.

    @see
        @ref pct_encode,
        @ref pct_encode_bytes,
        @ref pct_encode_to_value.
*/
struct pct_encode_opts
{
    /** True if space (SP, ' ') encodes into PLUS ('+').

        @par Specification
        @li <a href="https://www.w3.org/TR/html401/interact/forms.html#h-17.13.4.1">
            application/x-www-form-urlencoded (w3.org)</a>
    */
    bool space_to_plus = false;
};

/** Return the number of bytes needed to store a string with percent-encoding applied

    This function examines the characters in
    the string to determine the number of bytes
    necessary if the string were to be percent
    encoded using the given options and character
    set. No encoding is actually performed.

    @par Example 1
    Find the number of bytes needed to encode a string without transforming
    ' ' to '+'.
    @code
    pct_encode_opts opt;
    opt.space_to_plus = false;
    std::size_t n = pct_encode_bytes( "My Stuff", pchars, opt );

    assert( n == 10 );
    @endcode

    @par Example 2
    Find the number of bytes needed to encode a string when transforming
    ' ' to '+'.
    @code
    pct_encode_opts opt;
    opt.space_to_plus = true;
    std::size_t n = pct_encode_bytes( "My Stuff", opt, pchars );

    assert( n == 8 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The number of bytes needed,
    excluding any null terminator.

    @param s The string to encode.

    @param opt The options for encoding. If
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
        @ref pct_decode,
        @ref pct_encode,
        @ref pct_encode_opts,
        @ref pct_encode_to_value.
*/
template <class CharSet>
std::size_t
pct_encode_bytes(
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt = {}) noexcept;

/** Write a string with percent-encoding into a buffer.

    This function applies percent-encoding to
    the given plain string, by escaping all
    characters that are not in the specified
    <em>CharSet</em>.
    The output is written to the destination,
    and will be truncated if there is
    insufficient space.

    @par Example
    @code
    char *dest = new char[MAX_LENGTH];
    std::size_t encoded_size = pct_encode( dest, dest + MAX_LENGTH,
            "Program Files", pct_encode_opts{}, pchars );

    assert( encoded_size == 15 );
    assert( strncmp( "Program%20Files", dest, encoded_size ) == 0 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return `true` if the output was large
    enough to hold the entire result.

    @param[in, out] dest A pointer to the
    beginning of the output buffer. Upon
    return, the argument will be changed
    to one past the last character written.

    @param end A pointer to one past the end
    of the output buffer.

    @param s The string to encode.

    @param opt The options for encoding. If
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
        @ref pct_decode,
        @ref pct_encode,
        @ref pct_encode_bytes.
*/
template <class CharSet>
std::size_t
pct_encode(
    char* dest,
    char const* end,
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt = {});

/** Return a string with percent-encoding applied

    This function applies percent-encoding to
    the given plain string, by escaping all
    characters that are not in the specified
    <em>CharSet</em>.
    The result is returned as a
    `std::basic_string`, using the optionally
    specified allocator.

    @par Example
    @code
    pct_encode_opts opt;
    opt.space_to_plus = true;
    std::string s = pct_encode( "My Stuff", opt, pchars );

    assert( s == "My+Stuff" );
    @endcode

    @par Exception Safety
    Calls to allocate may throw.

    @return A `std::basic_string` holding the
    encoded string, using the specified
    allocator.

    @param s The string to encode.

    @param opt The options for encoding. If
    this parameter is omitted, the default
    options will be used.

    @param allowed The set of characters
    allowed to appear unescaped.
    This type must satisfy the requirements
    of <em>CharSet</em>. If this parameter is
    omitted, then no characters are considered
    special. The character set is ignored if
    `opt.non_normal_is_error == false`.

    @param a An optional allocator the returned
    string will use. If this parameter is omitted,
    the default allocator is used. In this case
    the return type will be `std::string`.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_encode,
        @ref pct_encode_bytes,
        @ref pct_encode_opts,
        @ref pct_encode_to_value,
        @ref reserved_chars_t.
*/
template<
    class CharSet,
    class Allocator =
        std::allocator<char> >
std::basic_string<char,
    std::char_traits<char>,
        Allocator>
pct_encode_to_string(
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt = {},
    Allocator const& a = {});

} // urls
} // boost

#include <boost/url/impl/pct_encoding.hpp>

#endif

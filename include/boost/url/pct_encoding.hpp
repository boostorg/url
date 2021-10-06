//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
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
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/string.hpp>
#include <memory>

namespace boost {
namespace urls {

/** Options for removing percent-encoding from strings

    @see
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

    @par Exception Safety
    Throws nothing.

    @throw invalid_argument if the encoded string
        is invalid.

    @return The number of bytes needed, excluding
    any null terminator.

    @param s The percent-encoded string to analyze.

    @param ec Set to the error, if any occurred.

    @param cs An optional character set to use.
    This type must satisfy the requirements
    of <em>CharSet</em>. If this parameter is
    omitted, then no characters are considered
    special. The character set is ignored if
    `opt.non_normal_is_error == false`.

    @param opt The options for decoding. If this
    parameter is omitted, the default options
    will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode_opts,
        @ref pct_encode.
*/
template<class CharSet>
std::size_t
validate_pct_encoding(
    string_view s,
    error_code& ec,
    CharSet const& cs,
    pct_decode_opts const& opt = {}) noexcept;

/** Return the number of bytes needed to hold the string with percent-decoding applied.

    This function calculates the number of
    bytes needed to apply percent-decoding to
    the encoded string `s`, excluding any
    null terminator. The caller is responsible
    for validating the input string before
    calling this function.

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
pct_decoded_bytes_unchecked(
    string_view s) noexcept;

/** Apply percent-decoding to a string

    This function applies percent-decoding to
    the input string, without performing any
    checking to ensure that the input string
    is valid. The contents of the output
    buffer will never be left undefined,
    regardless of input.

    @par Exception Safety
    Throws nothing.

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
void
pct_decode_unchecked(
    char* dest,
    char const* end,
    string_view s,
    pct_decode_opts const& opt = {}) noexcept;

/** Return a string with percent-decoding applied.

    This function applies percent-decoding
    to the specified string and returns a
    newly allocated string containing the
    result, using the optionally specified
    allocator. No checking is performed to
    ensure that the input is valid; however,
    the returned string is never undefined.

    @par Exception Safety
    Calls to allocate may throw.

    @return A string containing the decoded
    result.

    @param s The string to decode.

    @param opt Optional settings for applying
    the decoding. If this parameter is omitted,
    default settings are used.

    @param a An optional allocator the returned
    string will use. If this parameter is omitted,
    the default allocator is used.

    @param decoded_size An optional hint to
    the algorithm indicating the correct
    decoded size. If this parameter is omitted,
    the value will be calculated as-if by
    calling @ref pct_decoded_bytes_unchecked.
*/
template<class Allocator =
    std::allocator<char> >
string_value
pct_decode_unchecked(
    string_view s,
    pct_decode_opts const& opt = {},
    Allocator const& a = {},
    std::size_t decoded_size =
        std::size_t(-1) );

// VFALCO Users will probably want these eventually
#if 0
template<class CharSet = bnf::all_chars>
std::size_t
pct_decode(
    char* dest,
    char const* end,
    error_code& ec,
    string_view s,
    pct_decode_opts const& opt = {},
    CharSet const& cs = {}) noexcept;

template<class CharSet = bnf::all_chars>
string_value
pct_decode(
    string_view s,
    pct_decode_opts const& opt = {},
    CharSet const& cs = {},
    Allocator const& a) noexcept;

template<
    class CharSet = bnf::all_chars>
std::string
pct_decode(
    string_view s,
    pct_decode_opts const& opt = {},
    CharSet const& cs = {}) noexcept;
#endif

//------------------------------------------------

/** Options for applying percent-encoding to strings
*/
struct pct_encode_opts
{
    /** True if SP (space, ' ') encodes into PLUS ('+')

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
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_encode,
        @ref pct_encode_opts.
*/
template<
    class String,
    class CharSet>
std::size_t
pct_encode_bytes(
    String const& s,
    CharSet const& cs,
    pct_encode_opts const& opt = {}) noexcept;

#ifndef BOOST_URL_DOCS
namespace bnf {
struct all_chars;
} // bnf
// VFALCO This would make no sense
std::size_t
pct_encode_bytes(
    string_view,
    bnf::all_chars const&,
    ...) = delete;
#endif

/** Apply percent-encoding to a string

    This function applies percent-encoding to
    the given plain string, by escaping all
    characters that are not in the specified
    <em>CharSet</em>.
    The output is written to the destination,
    which will be truncated if there is
    insufficient space.

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

    @param cs The character set to use.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_encode,
        @ref pct_encode_bytes.
*/
template<
    class String,
    class CharSet>
bool
pct_encode(
    char*& dest,
    char const* end,
    String const& s,
    CharSet const& cs,
    pct_encode_opts const& opt = {});

/** Return a string with percent-encoding applied

    This function applies percent-encoding to
    the given plain string, by escaping all
    characters that are not in the specified
    <em>CharSet</em>.
    The result is returned as a @ref string_value
    using the optionally specified allocator.

    @par Exception Safety
    Calls to allocate may throw.

    @return A @ref string_value holding the
    encoded string.

    @param s The string to encode.

    @param cs The character set to use.

    @param a An optional allocator the returned
    string will use. If this parameter is omitted,
    the default allocator is used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref pct_decode,
        @ref pct_encode,
        @ref pct_encode_bytes.
*/
template<
    class String,
    class CharSet,
    class Allocator = std::allocator<char> >
string_value
pct_encode(
    String const& s,
    CharSet const& cs,
    pct_encode_opts const& opt = {},
    Allocator const& a = {});

} // urls
} // boost

#include <boost/url/impl/pct_encoding.hpp>

#endif

//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_HEXDIG_CHARS_HPP
#define BOOST_URL_GRAMMAR_HEXDIG_CHARS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/detail/charset.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** The set of hexadecimal digits

    This function object is invocable with
    the following equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    It returns `true` when `ch` is a member
    of the character set, and `false` otherwise.

    @par BNF
    @code
    HEXDIG      = DIGIT
                / "A" / "B" / "C" / "D" / "E" / "F"
                / "a" / "b" / "c" / "d" / "e" / "f"
    @endcode

    @note The RFCs are inconsistent on the case
    sensitivity of hexadecimal digits. Existing
    uses suggest case-insensitivity is a de-facto
    standard.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc7230#section-1.2"
        >1.2. Syntax Notation (rfc7230)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5952#section-2.3"
        >2.3. Uppercase or Lowercase (rfc5952)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5952#section-4.3"
        >4.3. Lowercase (rfc5952)</a>

    @see
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ hexdig_chars;
#else
struct hexdig_chars_t
{
    /** Return true if c is in the character set.
    */
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f');
    }

#ifdef BOOST_URL_USE_SSE2
    char const*
    find_if(
        char const* first,
        char const* last) const noexcept
    {
        return detail::find_if_pred(
            *this, first, last);
    }

    char const*
    find_if_not(
        char const* first,
        char const* last) const noexcept
    {
        return detail::find_if_not_pred(
            *this, first, last);
    }
#endif
};

constexpr hexdig_chars_t hexdig_chars{};
#endif

// VFALCO We can declare
// these later if needed
//
//struct hexdig_upper_chars;
//struct hexdig_lower_chars;

/** Return the numeric value of a HEXDIG, or -1 if invalid

    This function returns the numeric value
    of a single hexadecimal digit, or -1 if
    `ch` is not a hexadecimal digit.
*/
inline
bool
hexdig_value(
    char c, char& res) noexcept
{
    if (c >= '0' && c <= '9')
    {
        res = c - '0';
        return true;
    }
    if (c >= 'A' && c <= 'F')
    {
        res = 10 + c - 'A';
        return true;
    }
    if (c >= 'a' && c <= 'f')
    {
        res = 10 + c - 'a';
        return true;
    }
    res = 0;
    return false;
}

} // grammar
} // urls
} // boost

#endif

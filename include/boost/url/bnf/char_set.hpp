//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_CHAR_SET_HPP
#define BOOST_URL_BNF_CHAR_SET_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/bnf/detail/char_set.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** Set of alphanumeric characters

    @par BNF
    @code
    ALNUM       = ALPHA / DIGIT

    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z

    DIGIT       =  %x30-39
                ; 0-9
    @endcode

    @see
        find_if, find_if_not
        @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">[RFC5234] B.1. Core Rules</a>
*/
struct alnum_chars
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z');
    }
};

/** Set of alpha characters

    @par BNF
    @code
    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z
    @endcode

    @see
        find_if, find_if_not
        @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">[RFC5234] B.1. Core Rules</a>
*/
struct alpha_chars
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z');
    }
};

/** Set of digit characters

    @par BNF
    @code
    DIGIT       =  %x30-39
                ; 0-9
    @endcode

    @see
        find_if, find_if_not
        @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">[RFC5234] B.1. Core Rules</a>
*/
struct digit_chars
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return c >= '0' && c <= '9';
    }
};

/** Set of hexadecimal digit characters

    @par BNF
    @code
    HEXDIG  =  DIGIT
            / "A" / "B" / "C" / "D" / "E" / "F"
            / "a" / "b" / "c" / "d" / "e" / "f"
    @endcode

    @note The RFCs are inconsistent on the case
    sensitivity of hexadecimal digits. Existing
    uses suggest case-insensitivity is a de-facto
    standard.

    @see
        find_if, find_if_not
        @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">[RFC5234] B.1. Core Rules</a>
        @li https://datatracker.ietf.org/doc/html/rfc7230#section-1.2
        @li https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1
        @li https://datatracker.ietf.org/doc/html/rfc5952#section-2.3
        @li https://datatracker.ietf.org/doc/html/rfc5952#section-4.3
*/
struct hexdig_chars
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f');
    }
};

// VFALCO We can declare
// these later if needed
//
//struct hexdig_upper_chars;
//struct hexdig_lower_chars;

/** Return the numeric value of a HEXDIG, or -1 if invalid
*/
constexpr
char
hexdig_value(char c) noexcept
{
    return
        (c >= '0' && c <= '9') ?
        c - '0' : (
            (c >= 'A' && c <= 'F') ?
            10 + c - 'A' : (
                (c >= 'a' && c <= 'f') ?
                10 + c - 'a' : - 1) );
}

//------------------------------------------------

/** Find the first characer in the string that is in CharSet
*/
template<class CharSet>
char const*
find_if(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        detail::is_char_set_pred<CharSet>::value);
    return detail::find_if(first, last, cs,
        detail::has_find_if<CharSet>{});
}

/** Find the first characer in the string that is not in CharSet
*/
template<class CharSet>
char const*
find_if_not(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        detail::is_char_set_pred<CharSet>::value);
    return detail::find_if_not(first, last, cs,
        detail::has_find_if_not<CharSet>{});
}

} // bnf
} // urls
} // boost

#endif

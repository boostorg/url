//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_CHARSET_HPP
#define BOOST_URL_BNF_CHARSET_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/bnf/detail/charset.hpp>
#include <boost/type_traits/make_void.hpp>
#include <boost/static_assert.hpp>
#include <cstdint>
#include <type_traits>

// Credit to Peter Dimov for ideas regarding
// SIMD constexpr, and character set masks.

namespace boost {
namespace urls {
namespace bnf {

/** Alias for `std::true_type` if T satisfies <em>CharSet</em>.

    This metafunction determines if the
    type `T` meets these requirements of
    <em>CharSet</em>:

    @li An instance of `T` is invocable
    with this equivalent function signature:
    @code
    bool T::operator()( char ) const;
    @endcode

    @par Example
    Use with `enable_if` on the return value:
    @code
    template< class CharSet >
    typename std::enable_if< is_charset<T>::value >::type
    func( CharSet const& cs );
    @endcode

    @tparam T the type to check.
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_charset = __see_below__;
#else
template<class T, class = void>
struct is_charset : std::false_type {};

template<class T>
struct is_charset<T, boost::void_t<
    decltype(
    std::declval<bool&>() =
        std::declval<T const&>().operator()(
            std::declval<char>())
            ) > > : std::true_type
{
};
#endif

//------------------------------------------------

/** A character set containing all characters.

    Objects of this type are invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of the
    function satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    ALL         = %x00-FF
                ; all ASCII and high-ASCII
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>

    @see
        @ref all_chars,
        @ref is_charset,
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
using all_chars_t = __see_below__;
#else
struct all_chars_t
{
    constexpr
    bool
    operator()(char) const noexcept
    {
        return true;
    }

    constexpr
    char const*
    find_if(
        char const* first,
        char const*) const noexcept
    {
        return first;
    }

    constexpr
    char const*
    find_if_not(
        char const*,
        char const* last) const noexcept
    {
        return last;
    }
};
#endif

/** A character set constant representing all characters

    @see
        @ref all_chars_t
*/
constexpr all_chars_t all_chars{};

//------------------------------------------------

/** A character set containing all letters and digits.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of the
    function satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    ALNUM       = ALPHA / DIGIT

    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z

    DIGIT       =  %x30-39
                ; 0-9
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>

    @see
        @ref is_charset,
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
using alnum_chars_t = __see_below__;
#else
struct alnum_chars_t
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
#endif

/** A character set containing the alphanumeric characters.

    @see
        @ref alnum_chars_t
*/
constexpr alnum_chars_t alnum_chars{};

//------------------------------------------------

/** A character set containing the alphabetical characters.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of the
    function satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>

    @see
        @ref alpha_chars,
        @ref is_charset,
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
using alpha_chars_t = __see_below__;
#else
struct alpha_chars_t
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z');
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
#endif

/** A character set containing the alphabetical characters.

    @see
        @ref alpha_chars_t
*/
constexpr alpha_chars_t alpha_chars{};

//------------------------------------------------

/** A character set containing the decimal digits.

    This is an object invocable with
    the equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of the
    function satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    DIGIT       =  %x30-39
                ; 0-9
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>

    @see
        @ref digit_chars,
        @ref is_charset,
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
using digit_chars_t = __see_below__;
#else
struct digit_chars_t
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return c >= '0' && c <= '9';
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
#endif

/** A character set containing the decimal digits

    @see
        @ref digit_chars_t
*/
constexpr digit_chars_t digit_chars{};

//------------------------------------------------

/** A character set containing the hexadecimal digits.

    This is an object invocable with
    the equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of the
    function satisfies the <em>CharSet</em>
    requirements.

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
        @ref hexdig_chars,
        @ref is_charset,
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
using hexdig_chars_t = __see_below__;
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
#endif

/** A character set containing the hexadecimal digits

    @see
        @ref hexdig_chars_t
*/
constexpr hexdig_chars_t hexdig_chars{};

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

/** Find the first character in the string that is in the set.

    @par Exception Safety
    Throws nothing.

    @return A pointer to the character if found,
    otherwise a pointer to `last`.

    @param first A pointer to the first character
    in the string to search.

    @param last A pointer to one past the last
    character in the string to search.

    @param cs The character set to use.

    @see
        @ref find_if_not.
*/
template<class CharSet>
char const*
find_if(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        is_charset<CharSet>::value);
    return detail::find_if(first, last, cs,
        detail::has_find_if<CharSet>{});
}

/** Find the first character in the string that is not in CharSet

    @par Exception Safety
    Throws nothing.

    @return A pointer to the character if found,
    otherwise a pointer to `last`.

    @param first A pointer to the first character
    in the string to search.

    @param last A pointer to one past the last
    character in the string to search.

    @param cs The character set to use.

    @see
        @ref find_if_not.
*/
template<class CharSet>
char const*
find_if_not(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        is_charset<CharSet>::value);
    return detail::find_if_not(first, last, cs,
        detail::has_find_if_not<CharSet>{});
}

} // bnf
} // urls
} // boost

#endif

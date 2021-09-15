//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
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
#include <cstdint>

//#include <emmintrin.h>

// Credit to Peter Dimov for ideas regarding
// SIMD constexpr, and character set masks.

namespace boost {
namespace urls {
namespace bnf {

/** Alias for `std::true_type` if T satisfies __CharSet__
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_char_set = __see_below__;
#else
template<class T, class = void>
struct is_char_set : std::false_type {};

template<class T>
struct is_char_set<T, boost::void_t<
    decltype(
    std::declval<bool&>() =
        std::declval<T const&>().operator()(
            std::declval<char>())
            ) > > : std::true_type
{
};
#endif

//------------------------------------------------

/** A character set based on a unary predicate
*/
template<class Pred>
struct pred_chars
{
    constexpr
    bool
    operator()(unsigned char ch) const noexcept
    {
        return Pred()(ch);
    }

    #if 0
    constexpr
    char const*
    find_if(
        char const* first,
        char const* last ) const noexcept
    {
        while( last - first >= 16 )
        {
            unsigned char r[ 16 ] = {};
            for( int i = 0; i < 16; ++i )
                r[ i ] = Pr()( first[ i ] )? 0xFF: 0x00;
            __m128i r2 = _mm_loadu_si128( (__m128i const*)r );
            unsigned r3 = _mm_movemask_epi8( r2 );
            if( r3 )
                return first + __builtin_ctz( r3 );
            first += 16;
        }
        while(
            first != last &&
            ! Pred()(*first))
        {
            ++first;
        }
        return first;
    }
    #endif
};

//------------------------------------------------

/** A character set based on a constexpr lookup table
*/
class lut_chars
{
    std::uint64_t mask_[4] = {};

    constexpr
    static
    std::uint64_t
    lo(char c) noexcept
    {
        return static_cast<
            unsigned char>(c) & 3;
    }

    constexpr
    static
    std::uint64_t
    hi(char c) noexcept
    {
        return 1ULL << (static_cast<
            unsigned char>(c) >> 2);
    }

    constexpr
    static
    lut_chars
    construct(
        char const* s) noexcept
    {
        return *s
            ? lut_chars(*s) +
                construct(s+1)
            : lut_chars();
    }

    template<class Pred>
    constexpr
    static
    lut_chars
    construct(Pred pred) noexcept
    {
        return pred(255) ?
            lut_chars(255) :
            lut_chars();
    }

    template<class Pred>
    constexpr
    static
    lut_chars
    construct(Pred pred, unsigned char ch)
    {
        return ch == 255
            ? construct(pred)
            : construct(pred, ch) +
                construct(pred, ch + 1);
    }

    constexpr
    lut_chars() = default;

    constexpr
    lut_chars(
        std::uint64_t m0,
        std::uint64_t m1,
        std::uint64_t m2,
        std::uint64_t m3) noexcept
        : mask_{ m0, m1, m2, m3 }
    {
    }

public:
    constexpr
    explicit
    lut_chars(char ch)
        : mask_ {
            lo(ch) == 0 ? hi(ch) : 0,
            lo(ch) == 1 ? hi(ch) : 0,
            lo(ch) == 2 ? hi(ch) : 0,
            lo(ch) == 3 ? hi(ch) : 0 }
    {
    }

    constexpr
    explicit
    lut_chars(char const* s) noexcept
        : lut_chars(construct(s))
    {
    }

    template<class Pred>
    constexpr
    explicit
    lut_chars(Pred pred) noexcept
        : lut_chars(pred, 0)
    {
    }

    constexpr
    bool
    operator()(
        unsigned char ch) const noexcept
    {
        return mask_[lo(ch)] & hi(ch);
    }

    constexpr
    lut_chars
    operator+(char ch) const noexcept
    {
        return *this + lut_chars(ch);
    }

    constexpr
    lut_chars
    operator+(
        lut_chars const& cs) const noexcept
    {
        return lut_chars(
            mask_[0] | cs.mask_[0],
            mask_[1] | cs.mask_[1],
            mask_[2] | cs.mask_[2],
            mask_[3] | cs.mask_[3]);
    }

    constexpr
    lut_chars
    operator-(char ch) const noexcept
    {
        return *this - lut_chars(ch);
    }

    constexpr
    lut_chars
    operator-(
        lut_chars const& cs) const noexcept
    {
        return lut_chars(
            mask_[0] & ~cs.mask_[0],
            mask_[1] & ~cs.mask_[1],
            mask_[2] & ~cs.mask_[2],
            mask_[3] & ~cs.mask_[3]);
    }

    constexpr
    lut_chars
    operator~() const noexcept
    {
        return lut_chars(
            ~mask_[0],
            ~mask_[1],
            ~mask_[2],
            ~mask_[3]
        );
    }
};

//------------------------------------------------

/** A CharSet containing all characters

    @par BNF
    @code
    ALL         = %x00-FF
                ; all ASCII and high-ASCII
    @endcode

    @see @ref find_if, @ref find_if_not.
*/
struct all_chars
{
    constexpr
    bool
    operator()(char) const noexcept
    {
        return true;
    }
};

/** A CharSet containing the alphanumeric characters

    @par BNF
    @code
    ALNUM       = ALPHA / DIGIT

    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z

    xDIGIT       =  %x30-39
                ; 0-9
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">
        B.1. Core Rules (rfc5234)</a>

    @see @ref find_if, @ref find_if_not.
*/
struct alnum_chars
{
    /** Return true if c is in the character set.
    */
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

/** A CharSet containing the alpha characters

    @par BNF
    @code
    ALPHA       =  %x41-5A / %x61-7A
                ; A-Z / a-z
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">
        B.1. Core Rules (rfc5234)</a>

    @see @ref find_if, @ref find_if_not.
*/
struct alpha_chars
{
    /** Return true if c is in the character set.
    */
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z');
    }
};

/** A CharSet containing the digit characters

    @par BNF
    @code
    DIGIT       =  %x30-39
                ; 0-9
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">
        B.1. Core Rules (rfc5234)</a>

    @see @ref find_if, @ref find_if_not.
*/
struct digit_chars
{
    /** Return true if c is in the character set.
    */
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return c >= '0' && c <= '9';
    }
};

/** A CharSet containing the hexadecimal digit characters

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

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1">
        B.1. Core Rules (rfc5234)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc7230#section-1.2">
        1.2. Syntax Notation (rfc7230)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5952#section-2.3">
        2.3. Uppercase or Lowercase (rfc5952)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5952#section-4.3">
        4.3. Lowercase (rfc5952)</a>

    @see @ref find_if, @ref find_if_not.
*/
struct hexdig_chars
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

/** Find the first character in the string that is in CharSet
*/
template<class CharSet>
char const*
find_if(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        is_char_set<CharSet>::value);
    return detail::find_if(first, last, cs,
        detail::has_find_if<CharSet>{});
}

/** Find the first character in the string that is not in CharSet
*/
template<class CharSet>
char const*
find_if_not(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    BOOST_STATIC_ASSERT(
        is_char_set<CharSet>::value);
    return detail::find_if_not(first, last, cs,
        detail::has_find_if_not<CharSet>{});
}

} // bnf
} // urls
} // boost

#endif

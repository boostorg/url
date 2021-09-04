//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_CHAR_SETS_HPP
#define BOOST_URL_RFC_CHAR_SETS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/bnf/char_set.hpp>
#include <boost/url/rfc/detail/char_table.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace rfc {

/** Return true if c is a low-ASCII letter

    @par BNF
    @code
    ALPHA   =  %x41-5A / %x61-7A
            ; A-Z / a-z
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc2234#section-6.1
*/
inline
bool
is_alpha(char c) noexcept;

/** Return true if c is a decimal digit

    @par BNF
    @code
    DIGIT   = %x30-39
            ; 0-9
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc2234#section-6.1
*/
inline
bool
is_digit(char c) noexcept;

/** Return true if c is a hexadecimal digit

    @par BNF
    @code
    HEXDIG  =  DIGIT
            / "A" / "B" / "C" / "D" / "E" / "F"
            / "a" / "b" / "c" / "d" / "e" / "f"
    @endcode

    @note The RFCs are inconsistent on the case
    sensitivity of hexadecimal digits.

    @see
        https://datatracker.ietf.org/doc/html/rfc7230#section-1.2
        https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1
        https://datatracker.ietf.org/doc/html/rfc5952#section-2.3
        https://datatracker.ietf.org/doc/html/rfc5952#section-4.3
*/
inline
bool
is_hexdig(char c) noexcept;

/** Return true if c is in sub-delims

    @par BNF
    @code
    sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
*/
inline
bool
is_sub_delims(char c) noexcept;

/** Return true if c is unreserved

    @par BNF
    @code
    unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.3
*/
inline
bool
is_unreserved(char c) noexcept;

//------------------------------------------------

template<std::uint8_t Mask>
class masked_char_set
    : public bnf::char_set<
        masked_char_set<Mask>>
{
    std::uint8_t const* tab_ =
        detail::char_table;

public:
    bool
    contains(char c) const noexcept
    {
        return (tab_[static_cast<
            std::uint8_t>(c)] & Mask) != 0;
    }
};

//------------------------------------------------

constexpr std::uint8_t
    unreserved_char_mask = 0x01;

constexpr std::uint8_t
    sub_delims_char_mask = 0x02;

constexpr std::uint8_t
    gen_delims_char_mask = 0x04;

constexpr std::uint8_t
    question_char_mask = 0x08;

constexpr std::uint8_t
    colon_char_mask = 0x10;

constexpr std::uint8_t
    slash_char_mask = 0x20;

constexpr std::uint8_t
    alnum_char_mask = 0x40;

constexpr std::uint8_t
    at_char_mask = 0x80;

} // rfc
} // urls
} // boost

#include <boost/url/rfc/impl/char_sets.hpp>

#endif

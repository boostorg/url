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
#include <cstdint>

namespace boost {
namespace urls {
namespace rfc {

#ifndef BOOST_URL_DOCS
namespace detail {
BOOST_URL_DECL
extern
std::uint8_t
char_set_flags[256];
} // detail
#endif

/** Character set using bitmasks for membership
*/
template<std::uint8_t Mask>
class masked_char_set
{
    std::uint8_t const* tab_ =
        detail::char_set_flags;

public:
    bool
    operator()(char c) const noexcept
    {
        return (tab_[static_cast<
            std::uint8_t>(c)] & Mask) != 0;
    }
};

//------------------------------------------------

/** Mask for the unreserved character set

    @par BNF
    @code
    unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.3
*/
constexpr std::uint8_t
    unreserved_char_mask = 0x01;

/** Mask for character set of sub-delims

    @par BNF
    @code
    sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        @ref masked_char_set
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
*/
constexpr std::uint8_t
    sub_delims_char_mask = 0x02;

/** Mask for character set of gen-delims

    @par BNF
    @code
    gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    @endcode

    @see
        @ref masked_char_set
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
*/
constexpr std::uint8_t
    gen_delims_char_mask = 0x04;

/** Mask for a character set containing '?'

    @par BNF
    @code
    QUESTION    = "?"
    @endcode

    @see
        @ref masked_char_set
*/
constexpr std::uint8_t
    question_char_mask = 0x08;

/** Mask for a character set containing '='

    @par BNF
    @code
    EQUALS    = "="
    @endcode

    @see
        @ref masked_char_set
*/
constexpr std::uint8_t
    equals_char_mask = 0x10;

/** Mask for a character set containing ':'

    @par BNF
    @code
    COLON       = ":"
    @endcode

    @see
        @ref masked_char_set
*/
constexpr std::uint8_t
    colon_char_mask = 0x20;

/** Mask for a character set containing '/'

    @par BNF
    @code
    SLASH       = "/"
    @endcode

    @see
        @ref masked_char_set
*/
constexpr std::uint8_t
    slash_char_mask = 0x40;

/** Mask for a character set containing '@'

    @par BNF
    @code
    AT        = "Q"
    @endcode

    @see
        @ref masked_char_set
*/
constexpr std::uint8_t
    at_char_mask = 0x80;

/** Mask for a character set containing pchars

    @par BNF
    @code
    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    @endcode

    @see
        @ref masked_char_set
        @ref pct_encoded
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
constexpr std::uint8_t pchar_mask =
    unreserved_char_mask |
    sub_delims_char_mask |
    colon_char_mask |
    at_char_mask;

} // rfc
} // urls
} // boost

#endif

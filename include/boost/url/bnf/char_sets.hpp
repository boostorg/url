//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_CHAR_SETS_HPP
#define BOOST_URL_BNF_CHAR_SETS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_set.hpp>

namespace boost {
namespace urls {
namespace bnf {

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

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/char_sets.hpp>

#endif

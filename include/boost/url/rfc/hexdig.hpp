//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HEXDIG_HPP
#define BOOST_URL_RFC_HEXDIG_HPP

#include <boost/url/detail/config.hpp>

namespace boost {
namespace urls {
namespace rfc {

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

/** Return the numeric value of a hex char, or -1 if invalid
*/
inline
char
hex_digit(char c) noexcept;

} // rfc
} // urls
} // boost

#include <boost/url/rfc/impl/hexdig.hpp>

#endif

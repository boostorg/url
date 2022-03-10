//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_ASCII_HPP
#define BOOST_URL_GRAMMAR_ASCII_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/detail/charset.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {
namespace grammar {

// Algorithms for interacting with ASCII
// characters and strings, for implementing
// semantics in RFCs. These routines do
// not use locale.

/** Return c converted to lower case

    This function returns the lowercase equivalent
    if a character is an upper-case letter, otherwise
    returns the same character.

    @par Exception Safety
    Throws nothing.

    @return The converted character

    @param c The ascii character to convert
*/
constexpr
char
ascii_tolower(char c) noexcept
{
    return
      (c >= 'A' &&
       c <= 'Z')
        ? c + 'a' - 'A'
        : c;
}

/** Return c converted to upper case

    This function returns the lowercase equivalent
    if a character is an upper-case letter, otherwise
    returns the same character.

    @par Exception Safety
    Throws nothing.

    @return The converted character

    @param c The ascii character to convert
*/
constexpr
char
ascii_toupper(char c) noexcept
{
    return
      (c >= 'a' &&
       c <= 'z')
        ? c - ('a' - 'A')
        : c;
}

} // grammar
} // urls
} // boost

#endif

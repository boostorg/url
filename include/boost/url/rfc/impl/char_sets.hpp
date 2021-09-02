//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/urls
//

#ifndef BOOST_URL_RFC_IMPL_CHAR_SETS_HPP
#define BOOST_URL_RFC_IMPL_CHAR_SETS_HPP

namespace boost {
namespace urls {
namespace rfc {

bool
is_alpha(char c) noexcept
{
    return
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z');
}

bool
is_digit(char c) noexcept
{
    return static_cast<
        unsigned char>(c-'0') < 10;
}

bool
is_hexdig(char c) noexcept
{
    if(is_digit(c))
        return true;
    switch(c)
    {
    case 'A': case 'B': case 'C':
    case 'D': case 'E': case 'F':
    case 'a': case 'b': case 'c':
    case 'd': case 'e': case 'f':
        return true;
    default:
        break;
    }
    return false;
}

bool
is_sub_delims(char c) noexcept
{
    switch(c)
    {
    case  '!': case '$': case '&':
    case '\'': case '(': case ')':
    case  '*': case '+': case ',':
    case  ';': case '=':
        return true;
    default:
        break;
    }
    return false;
}

bool
is_unreserved(char c) noexcept
{
    if(is_alpha(c))
        return true;
    if(is_digit(c))
        return true;
    switch(c)
    {
    case '-':
    case '.':
    case '_':
    case '~':
        return true;
    default:
        break;
    }
    return false;
}

} // bnf
} // urls
} // boost

#endif

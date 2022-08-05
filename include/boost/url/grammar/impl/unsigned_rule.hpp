//
// Copyright (c) 2022 Alan de Freitas (alandefreitas at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_UNSIGNED_RULE_HPP
#define BOOST_URL_GRAMMAR_IMPL_UNSIGNED_RULE_HPP

#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/digit_chars.hpp>
#include <algorithm> // VFALCO grr..

namespace boost {
namespace urls {
namespace grammar {

template<class U>
auto
unsigned_rule<U>::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    if(it == end)
        return error::syntax;
    if(*it == '0')
    {
        ++it;
        return U(0);
    }
    if(! digit_chars(*it))
        return error::syntax;

    static constexpr U Digits10 =
        std::numeric_limits<
            U>::digits10;
    static constexpr U ten = 10;
    char const* safe_end;
    if(end - it >= Digits10)
        safe_end = it + Digits10;
    else
        safe_end = end;
    U u = *it - '0';
    ++it;

    while( it != safe_end &&
           digit_chars(*it))
    {
        char const dig = *it - '0';
        u = u * ten + dig;
        ++it;
    }

    if( it != end &&
        digit_chars(*it))
    {
        static constexpr U Max = (
            std::numeric_limits<
                U>::max)();
        static constexpr auto div = (Max / ten);
        static constexpr char rem = (Max % ten);
        char const dig = *it - '0';
        if( u > div || (
                u == div && dig > rem))
            return grammar::error::overflow;
        u = u * ten + dig;
        ++it;

        if( it < end &&
                digit_chars(*it))
            return grammar::error::overflow;
    }

    return u;
}

} // grammar
} // urls
} // boost

#endif

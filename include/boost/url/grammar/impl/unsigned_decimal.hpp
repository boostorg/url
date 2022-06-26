//
// Copyright (c) 2022 Alan de Freitas (alandefreitas at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_UNSIGNED_DECIMAL_HPP
#define BOOST_URL_GRAMMAR_IMPL_UNSIGNED_DECIMAL_HPP

namespace boost {
namespace urls {
namespace grammar {

template<class Unsigned>
void
unsigned_decimal<Unsigned>::parse(
    char const*& it,
    char const* end,
    error_code& ec,
    unsigned_decimal& t) noexcept
{
    if(it == end)
    {
        ec = grammar::error::syntax;
        return;
    }
    if(*it == '0')
    {
        t.u = 0;
        t.s = string_view(it, 1);
        ++it;
        return;
    }
    if(! digit_chars(*it))
    {
        ec = grammar::error::syntax;
        return;
    }

    auto const start = it;
    static constexpr Unsigned Digits10 =
        std::numeric_limits<
            Unsigned>::digits10;
    static constexpr Unsigned ten = 10;
    auto const safe_end = (
        std::min)(it + Digits10, end);
    Unsigned u = *it - '0';
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
        static constexpr Unsigned Max = (
            std::numeric_limits<
                Unsigned>::max)();
        static constexpr auto div = (Max / ten);
        static constexpr char rem = (Max % ten);
        char const dig = *it - '0';
        if( u > div || (
            u == div && dig > rem))
        {
            ec = grammar::error::overflow;
            return;
        }
        u = u * ten + dig;
        ++it;

        if( it < end &&
            digit_chars(*it))
        {
            ec = grammar::error::overflow;
            return;
        }
    }

    t.u = u;
    t.s = string_view(start, it);
}

} // grammar
} // urls
} // boost

#endif

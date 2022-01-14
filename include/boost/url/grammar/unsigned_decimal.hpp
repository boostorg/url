//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_UNSIGNED_DECIMAL_HPP
#define BOOST_URL_GRAMMAR_UNSIGNED_DECIMAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse_tag.hpp>
#include <boost/url/string_view.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an unsigned decimal

    @par BNF
    @code
    unsigned      = "0" / ( ["1"..."9"] *DIGIT )
    @endcode
*/
template<class T>
struct unsigned_decimal
{
    BOOST_STATIC_ASSERT(
        std::is_unsigned<T>::value);

    T u;
    string_view s;

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        unsigned_decimal& t) noexcept
    {
        parse(it, end, ec, t);
    }

private:
    static
    void
    parse(
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
            t.s = string_view(
                it, 1);
            ++it;
            t.u = 0;
            return;
        }
        T u = 0;
        auto const start = it;
        constexpr auto div = (T(-1) / 10);
        constexpr auto rem = (T(-1) % 10);
        while(it != end)
        {
            if(! digit_chars(*it))
                break;
            T const dig = *it - '0';
            if( u > div ||
                (u == div && dig > rem))
            {
                ec = grammar::error::overflow;
                return;
            }
            u = static_cast<T>(10 * u + dig);
            ++it;
        }
        t.u = u;
        t.s = string_view(start, it);
    }
};

//------------------------------------------------

} // grammar
} // urls
} // boost

#endif

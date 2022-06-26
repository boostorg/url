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
#include <boost/static_assert.hpp>
#include <limits>
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
template<class Unsigned>
struct unsigned_decimal
{
    Unsigned u;
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
    BOOST_STATIC_ASSERT(
        std::numeric_limits<
            Unsigned>::is_integer &&
        ! std::numeric_limits<
            Unsigned>::is_signed);

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
        auto const safe_end = (std::min)(it + Digits10, end);
        Unsigned u = *it - '0';
        ++it;

        while(
            it != safe_end &&
            digit_chars(*it))
        {
            char const dig = *it - '0';
            u = u * ten + dig;
            ++it;
        }

        if(
            it != end &&
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

            if(
                it < end &&
                digit_chars(*it))
            {
                ec = grammar::error::overflow;
                return;
            }
        }

        t.u = u;
        t.s = string_view(start, it);
    }
};

} // grammar
} // urls
} // boost

#endif

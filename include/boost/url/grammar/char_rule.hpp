//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_CHAR_RULE_HPP
#define BOOST_URL_GRAMMAR_CHAR_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/error.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Match a single character literal

    This example matches a period, and returns
    a view to the matching part of character
    buffer upon success.

    @par Example
    @code
    result< string_view > = parse( s, char_rule('.') );
    @endcode

    @par Value Type
    @code
    using value_type = string_view;
    @endcode

    @par BNF
    @code
    char        = %00-FF
    @endcode

    @param ch The character to match

    @see
        @ref parse.
*/
#ifdef BOOST_URL_DOCS
constexpr
__implementation_defined__
char_rule( char ch ) noexcept;
#else
struct char_rule
{
    using value_type = void;

    /** Constructor

        @param ch The character to match
    */
    constexpr
    char_rule(char ch) noexcept
        : ch_(ch)
    {
    }

    result<value_type>
    parse(
        char const*& it,
        char const* end) const noexcept
    {
        if(it != end)
        {
            if(*it == ch_)
            {
                ++it;
                return {};
            }
            return error::syntax;
        }
        return error::incomplete;
    }

private:
    char ch_;
};
#endif

} // grammar
} // urls
} // boost

#endif

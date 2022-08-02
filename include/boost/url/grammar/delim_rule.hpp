//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_DELIM_RULE_HPP
#define BOOST_URL_GRAMMAR_DELIM_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/error.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Match a character literal

    This matches the specified character.
    The value is a reference to the character
    in the underlying buffer, expressed as a
    @ref string_view. The function @ref squelch
    may be used to turn this into `void` instead.

    @par Value Type
    @code
    using value_type = string_view;
    @endcode

    @par Example
    Rules are used with the function @ref parse.
    @code
    result< string_view > rv = parse( ".", delim_rule('.') );
    @endcode

    @par BNF
    @code
    char        = %00-FF
    @endcode

    @param ch The character to match

    @see
        @ref parse,
        @ref squelch.
*/
#ifdef BOOST_URL_DOCS
constexpr
__implementation_defined__
delim_rule( char ch ) noexcept;
#else
struct delim_rule
{
    using value_type = string_view;

    /** Constructor

        @param ch The character to match
    */
    constexpr
    delim_rule(char ch) noexcept
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
                return string_view{
                    it++, 1 };
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

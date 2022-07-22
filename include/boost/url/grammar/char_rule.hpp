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

/** Rule to match a single character literal.
*/
struct char_rule
{
    using value_type = string_view;

    /** Constructor

        @param c The character to match
    */
    constexpr
    char_rule(
        char c) noexcept
        : c_(c)
    {
    }

    result<string_view>
    parse(
        char const*& it,
        char const* end) const noexcept
    {
        if( it != end &&
            *it == c_)
            return string_view(
                it++, 1);
        return error::syntax;
    }

private:
    char c_;
};

} // grammar
} // urls
} // boost

#endif

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PORT_RULE_IPP
#define BOOST_URL_IMPL_PORT_RULE_IPP

#include <boost/url/rfc/port_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/static_assert.hpp>
#include <type_traits>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    port_rule& t) noexcept
{
    port_rule::number_type u = 0;
    auto const start = it;
    while(it != end)
    {
        if(! grammar::digit_chars(*it))
            break;
        auto u0 = u;
        u = 10 * u + *it - '0';
        if(u < u0)
        {
            // overflow
            it = grammar::find_if_not(
                it, end, grammar::digit_chars);
            t.str = string_view(
                start, it - start);
            t.has_number = false;
            ec = {};
            return;
        }
        ++it;
    }
    t.str = string_view(
        start, it - start);
    if(! t.str.empty())
    {
        t.has_number = true;
        t.number = u;
    }
    else
    {
        t.has_number = false;
    }
}

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    port_part_rule& t)
{
    if( it == end ||
        *it != ':')
    {
        ec = {};
        t.has_port = false;
        return;
    }
    ++it;
    port_rule t0;
    if(! grammar::parse(
            it, end, ec, t0))
        return;
    t.has_port = true;
    t.port = t0.str;
    t.has_number = t0.has_number;
    t.port_number = t0.number;
}

} // urls
} // boost

#endif

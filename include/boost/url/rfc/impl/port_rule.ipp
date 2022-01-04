//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PORT_BNF_IPP
#define BOOST_URL_IMPL_PORT_BNF_IPP

#include <boost/url/rfc/port_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token.hpp>
#include <boost/static_assert.hpp>
#include <type_traits>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    port_rule& t)
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
            return true;
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
    return true;
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    port_part_rule& t)
{
    using grammar::parse;
    if( it == end ||
        *it != ':')
    {
        ec = {};
        t.has_port = false;
        return true;
    }
    ++it;
    port_rule t0;
    if(! parse(it, end, ec, t0))
        return false;
    t.has_port = true;
    t.port = t0.str;
    t.has_number = t0.has_number;
    t.port_number = t0.number;
    return true;
}

} // urls
} // boost

#endif

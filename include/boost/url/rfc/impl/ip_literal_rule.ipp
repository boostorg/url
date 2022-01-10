//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_IP_LITERAL_RULE_IPP
#define BOOST_URL_IMPL_IP_LITERAL_RULE_IPP

#include <boost/url/rfc/ip_literal_rule.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/ipv_future_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    ip_literal_rule& t) noexcept
{
    // '['
    if(! grammar::parse(
            it, end, ec, '['))
        return;
    if(it == end)
    {
        // expected address
        ec = grammar::error::incomplete;
        return;
    }
    if(*it != 'v')
    {
        // IPv6address
        if(! grammar::parse(
                it, end, ec,
                t.ipv6, ']'))
            return;
        t.is_ipv6 = true;
    }
    else
    {
        // IPvFuture
        ipv_future_rule p;
        if(! grammar::parse(
                it, end, ec,
                p, ']'))
            return;
        t.is_ipv6 = false;
        t.ipvfuture = p.str;
    }
}

} // urls
} // boost

#endif

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_HOST_RULE_IPP
#define BOOST_URL_IMPL_HOST_RULE_IPP

#include <boost/url/rfc/host_rule.hpp>
#include <boost/url/rfc/ip_literal_rule.hpp>
#include <boost/url/rfc/reg_name_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    host_rule& t) noexcept
{
    if(it == end)
    {
        t.host_type =
            host_type::name;
        t.name = {};
        t.host_part = {};
        return;
    }
    auto const start = it;
    if(*it == '[')
    {
        // IP-literal
        ip_literal_rule v;
        if(! grammar::parse(
                it, end, ec, v))
            return;
        if(v.is_ipv6)
        {
            // IPv6address
            t.ipv6 = v.ipv6;
            t.host_type =
                host_type::ipv6;
            t.host_part = string_view(
                start, it - start);
            return;
        }

        // IPvFuture
        t.ipvfuture = v.ipvfuture;
        t.host_type =
            host_type::ipvfuture;
        t.host_part = string_view(
            start, it - start);
        return;
    }
    // IPv4address
    {
        if(grammar::parse(
            it, end, ec, t.ipv4))
        {
            t.host_type =
                host_type::ipv4;
            t.host_part = string_view(
                start, it - start);
            return;
        }
        // rewind
        it = start;
    }
    // reg-name
    if(! grammar::parse(it, end, ec,
        reg_name_rule{t.name}))
    {
        // bad reg-name
        return;
    }

    t.host_type =
        host_type::name;
    t.host_part = string_view(
        start, it - start);
}

} // urls
} // boost

#endif

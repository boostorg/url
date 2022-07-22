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

auto
host_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    value_type t;

    if(it == end)
    {
        t.host_type =
            urls::host_type::name;
        t.name = {};
        t.host_part = {};
        return t;
    }
    auto const start = it;
    if(*it == '[')
    {
        // IP-literal
        auto rv = grammar::parse(
            it, end, ip_literal_rule);
        if(! rv)
            return rv.error();
        auto v = *rv;
        if(v.is_ipv6)
        {
            // IPv6address
            t.ipv6 = v.ipv6;
            t.host_type =
                urls::host_type::ipv6;
            t.host_part = string_view(
                start, it - start);
            return t;
        }

        // IPvFuture
        t.ipvfuture = v.ipvfuture;
        t.host_type =
            urls::host_type::ipvfuture;
        t.host_part = string_view(
            start, it - start);
        return t;
    }
    // IPv4address
    {
        auto rv = grammar::parse(
            it, end, ipv4_address_rule);
        if(rv.has_value())
        {
            t.ipv4 = *rv;
            t.host_type =
                urls::host_type::ipv4;
            t.host_part = string_view(
                start, it - start);
            return t;
        }
        // rewind
        it = start;
    }
    // reg-name
    {
        auto rv = grammar::parse(
            it, end, reg_name_rule);
        if(! rv)
            return rv.error();
        t.name = *rv;
        t.host_type =
            urls::host_type::name;
        t.host_part = string_view(
            start, it - start);
        return t;
    }
}

} // urls
} // boost

#endif

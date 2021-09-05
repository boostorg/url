//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_HOST_IPP
#define BOOST_URL_RFC_IMPL_HOST_IPP

#include <boost/url/rfc/host.hpp>
#include <boost/url/rfc/ip_literal.hpp>
#include <new>

namespace boost {
namespace urls {
namespace rfc {

void
host::
destroy()
{
    switch(kind_)
    {
    default:
    case host_kind::none:
        break;
    case host_kind::ipv4:
        ipv4_.~ipv4_address();
        break;
    case host_kind::ipv6:
        ipv6_.~ipv6_address();
        break;
    case host_kind::ipv_future:
        fut_.~string_view();
        break;
    case host_kind::named:
        name_.~pct_encoded_str();
        break;
    }
}

host::
~host()
{
    destroy();
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    host& t)
{
    auto const start = it;
    t.destroy();
    t.kind_ = host_kind::none;
    if(*it == '[')
    {
        // IP-literal
        ip_literal v;
        if(! parse(it, end, ec, v))
        {
            // need more or
            // bad ip-literal
            return false;
        }
        if(v.is_ipv6)
        {
            // IPv6address
            ::new(&t.ipv6_)
                ipv6_address(v.ipv6);
            t.kind_ = host_kind::ipv6;
            goto finish;
        }
        // VFALCO TODO
        // IPvFuture
        ::new(&t.fut_) string_view(
            v.fut_str);
        t.kind_ = host_kind::ipv_future;
        goto finish;
    }
    // IPv4address
    {
        ipv4_address v;
        if(parse(it, end, ec, v))
        {
            ::new(&t.ipv4_)
                ipv4_address(v);
            t.kind_ = host_kind::ipv4;
            goto finish;
        }
        ec = {};
    }
    // reg-name
    {
        pct_encoded_str ns;
        if(! parse(it, end, ec,
            pct_encoded<
                unsub_char_mask>{ns}))
        {
            // bad reg-name
            return false;
        }
        ::new(&t.name_)
            pct_encoded_str(ns);
        t.kind_ = host_kind::named;
    }
finish:
    t.s_ = string_view(
        start, it - start);
    return true;
}

} // rfc
} // urls
} // boost

#endif

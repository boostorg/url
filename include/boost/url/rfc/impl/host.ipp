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
        break;
    case host_kind::named:
        name_.~pct_encoded_value();
        break;
    }
}

host::
~host()
{
    destroy();
}

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    host& t)
{
    t.destroy();
    t.kind_ = host_kind::none;
    auto it = start;
    if(*it == '[')
    {
        // IP-literal
        ip_literal v;
        it = parse(it, end, ec, v);
        if(ec)
        {
            // need more or
            // bad ip-literal
            return start;
        }
        if(v.is_ipv6())
        {
            // IPv6address
            ::new(&t.ipv6_) ipv6_address(
                v.get_ipv6_address());
            t.kind_ = host_kind::ipv6;
            return it;
        }
        // VFALCO TODO
        // IPvFuture
        ec = error::syntax;
        return start;
    }
    // IPv4address
    {
        ipv4_address v;
        it = parse(it, end, ec, v);
        if(! ec)
        {
            ::new(&t.ipv4_)
                ipv4_address(v);
            return it;
        }
        ec = {};
    }
    // reg-name
    pct_encoded<
        unreserved_char_mask |
        sub_delims_char_mask> v;
    it = parse(it, end, ec, v);
    if(ec)
    {
        // bad reg-name
        return start;
    }
    ::new(&t.name_) pct_encoded_value(
        v.value());
    t.kind_ = host_kind::named;
    return it;
}

} // rfc
} // urls
} // boost

#endif

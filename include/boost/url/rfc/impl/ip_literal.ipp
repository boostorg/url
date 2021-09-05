//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_IP_LITERAL_IPP
#define BOOST_URL_RFC_IMPL_IP_LITERAL_IPP

#include <boost/url/rfc/host.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/ipv_future.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    ip_literal& t)
{
    using bnf::parse;
    // '['
    if(! parse(
        it, end, ec, '['))
    {
        // expected '['
        return false;
    }
    if(it == end)
    {
        // expected address
        ec = error::syntax;
        return false;
    }
    if(*it != 'v')
    {
        // IPv6address
        if(! parse(it, end, ec,
            t.ipv6, ']'))
            return false;
        t.is_ipv6 = true;
        return true;
    }
    // IPvFuture
    ipv_future v;
    if(! parse(
        it, end, ec, v, ']'))
        return false;
    t.fut_str = v.str();
    t.is_ipv6 = false;
    return true;
}

} // rfc
} // urls
} // boost

#endif

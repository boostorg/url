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

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    ip_literal& t)
{
    if(start == end)
    {
        ec = error::need_more;
        return start;
    }
    auto it = start;
    if(*it != '[')
    {
        // expected '['
        ec = error::syntax;
        return start;
    }
    ++it;
    if(it == end)
    {
        // expected address
        ec = error::need_more;
        return start;
    }
    if(*it != 'v')
    {
        // IPv6address
        using bnf::parse;
        it = parse(it,
            end, ec, t.ipv6_, ']');
        if(ec)
            return start;
        t.is_ipv6_ = true;
        return it;
    }
    // IPvFuture
    ipv_future v;
    it = parse(it, end, ec, v);
    if(ec)
        return start;
    t.is_ipv6_ = false;
    return it;
}

} // rfc
} // urls
} // boost

#endif

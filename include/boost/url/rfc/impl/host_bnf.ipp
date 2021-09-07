//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_HOST_BNF_IPP
#define BOOST_URL_RFC_IMPL_HOST_BNF_IPP

#include <boost/url/rfc/host_bnf.hpp>
#include <boost/url/rfc/ip_literal_bnf.hpp>
#include <new>

namespace boost {
namespace urls {
namespace rfc {

host_bnf::
~host_bnf() noexcept = default;

host_bnf::
host_bnf() noexcept = default;

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    host_bnf& t)
{
    auto const start = it;
    if(*it == '[')
    {
        // IP-literal
        ip_literal_bnf v;
        if(! parse(it, end, ec, v))
        {
            // need more or
            // bad ip-literal
            return false;
        }
        if(v.is_ipv6)
        {
            // IPv6address
            t.ipv6_ = v.ipv6;
            t.host_type_ = host_type::ipv6;
            goto finish;
        }
        // VFALCO TODO
        // IPvFuture
        t.ipvfuture_ = v.fut_str;
        t.host_type_ =
            host_type::ipvfuture;
        goto finish;
    }
    // IPv4address
    {
        ipv4_address_bnf v;
        auto it0 = it;
        if(parse(it, end, ec, v))
        {
            t.ipv4_ = v.addr;
            t.host_type_ = host_type::ipv4;
            goto finish;
        }
        it = it0;
        ec = {};
    }
    // reg-name
    {
        pct_encoded_str ns;
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                unsub_char_mask>{
                    t.name_}))
        {
            // bad reg-name
            return false;
        }
        t.host_type_ =
            host_type::name;
    }
finish:
    t.str_ = string_view(
        start, it - start);
    return true;
}

} // rfc
} // urls
} // boost

#endif

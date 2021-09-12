 //
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PARSE_IPP
#define BOOST_URL_DETAIL_IMPL_PARSE_IPP

#include <boost/url/detail/parse.hpp>

namespace boost {
namespace urls {
namespace detail {

void
apply(
    parts& p,
    scheme_part_bnf const& t) noexcept
{
    p.scheme = t.scheme_id;
    p.resize(
        part::id_scheme,
        t.scheme_part.size());
}

void
apply(
    parts& p,
    host_bnf const& t) noexcept
{
    p.host_type = t.host_type;
    switch(t.host_type)
    {
    default:
    case urls::host_type::none:
    {
        break;
    }
    case urls::host_type::name:
    {
        p.decoded[id_host] =
            t.name.decoded_size;
        break;
    }
    case urls::host_type::ipv4:
    {
        auto const bytes =
            t.ipv4.to_bytes();
        std::memcpy(
            &p.ip_addr[0],
            bytes.data(), 4);
        break;
    }
    case urls::host_type::ipv6:
    {
        auto const bytes =
            t.ipv6.to_bytes();
        std::memcpy(
            &p.ip_addr[0],
            bytes.data(), 16);
        break;
    }
    case urls::host_type::ipvfuture:
    {
        break;
    }
    }

    if(t.host_type != host_type::none)
    {
        p.resize(
            part::id_host,
            t.host_part.size());
    }
}

void
apply(
    parts& p, 
    authority_bnf const& t) noexcept
{
    if(t.has_userinfo)
    {
        auto const& u = t.userinfo;

        // leading "//" for authority
        p.resize(
            part::id_user,
            u.user.str.size() + 2);
        p.decoded[id_user] = u.user.decoded_size;

        if(u.has_password)
        {
            // leading ':' for password,
            // trailing '@' for userinfo
            p.resize(
                part::id_pass,
                u.password.str.size() + 2);
            p.decoded[id_pass] =
                u.password.decoded_size;
        }
        else
        {
            // trailing '@' for userinfo
            p.resize(part::id_pass, 1);
            p.decoded[id_pass] = 0;
        }
    }
    else
    {
        // leading "//" for authority
        p.resize(part::id_user, 2);
        p.decoded[id_user] = 0;
    }

    // host
    apply(p, t.host);

    // port
    if(t.port.has_port)
    {
        p.resize(
            part::id_port,
            t.port.port.size() + 1);

        if(t.port.has_number)
            p.port_number =
                t.port.port_number;
    }
}

void
apply_path(
    parts& p,
    string_view path,
    std::size_t path_count) noexcept
{
    p.resize(part::id_path,
        path.size());
    p.nseg = path_count;
}

void
apply(
    parts& p,
    query_part_bnf const& t) noexcept
{
    if(t.has_query)
    {
        p.resize(
            part::id_query,
            t.query_part.size());
        p.nparam = t.query_count;
    }
    else
    {
        p.nparam = 0;
    }
}

void
apply(
    parts& p,
    fragment_part_bnf const& t) noexcept
{
    if(t.has_fragment)
    {
        p.resize(
            part::id_frag,
            t.fragment_part.size());
        p.decoded[id_frag] =
            t.fragment.decoded_size;
    }
    else
    {
        p.decoded[id_frag] = 0;
    }
}

} // detail
} // urls
} // boost

#endif

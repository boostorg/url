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
#include <boost/optional.hpp>

namespace boost {
namespace urls {
namespace detail {

void
apply_host(
    parts& p,
    host_bnf const& h) noexcept
{
    p.host_type = h.host_type();
    switch(h.host_type())
    {
    default:
    case urls::host_type::none:
    {
        break;
    }
    case urls::host_type::name:
    {
        p.decoded[id_host] =
            h.get_name().decoded_size;
        break;
    }
    case urls::host_type::ipv4:
    {
        auto const bytes =
            h.get_ipv4().to_bytes();
        std::memcpy(
            &p.ip_addr[0],
            bytes.data(), 4);
        break;
    }
    case urls::host_type::ipv6:
    {
        auto const bytes =
            h.get_ipv6().to_bytes();
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

    if(h.host_type() !=
        host_type::none)
    {
        p.resize(
            part::id_host,
            h.str().size());
    }
}

void
apply_authority(
    parts& p, optional<
        authority_bnf> const& t) noexcept
{
    if(! t.has_value())
    {
        // no authority
        return;
    }
    auto const& u = t->userinfo;
    if(u.has_value())
    {
        // leading "//" for authority
        p.resize(
            part::id_user,
            u->username.str.size() + 2);

        if(u->password.has_value())
        {
            // leading ':' for password,
            // trailing '@' for userinfo
            p.resize(
                part::id_pass,
                u->password->str.size() + 2);
        }
        else
        {
            // trailing '@' for userinfo
            p.resize(part::id_pass, 1);
        }
    }
    else
    {
        // leading "//" for authority
        p.resize(part::id_user, 2);
    }

    apply_host(p, t->host);

    if(t->port.has_value())
    {
        // leading ':' for port
        p.resize(
            part::id_port,
            t->port->str.size() + 1);
        if(t->port->number.has_value())
            p.port_number = *t->port->number;
    }
}

void
apply_path(
    parts& p,
    bnf::range<
        pct_encoded_str> const& t) noexcept
{
    p.resize(
        part::id_path,
        t.str().size());
    p.nseg = t.size();
}

void
apply_query(
    parts& p,
    optional<bnf::range<
        query_param>> const& t) noexcept
{
    if(t.has_value())
    {
        p.resize(
            part::id_query,
            t->str().size() + 1);
        p.nparam = t->size();
    }
}

void
apply_fragment(
    parts& p,
    optional<
        pct_encoded_str> const& t) noexcept
{
    if(t.has_value())
    {
        p.resize(
            part::id_frag,
            t->str.size() + 1);
        p.decoded[id_frag] =
            t->decoded_size;
    }
}

} // detail
} // urls
} // boost

#endif

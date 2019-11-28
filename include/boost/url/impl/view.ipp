//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_VIEW_IPP
#define BOOST_URL_IMPL_VIEW_IPP

#include <boost/url/view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace url {

//----------------------------------------------------------

void
view::
serialize_userinfo(
    detail::any_buffer& b) const
{
    string_view s;
    s = get(id_username);
    if(! s.empty())
        b.encode(s,
            detail::userinfo_pct_set());
    s = get(id_password);
    if(! s.empty())
    {
        b.append(':');
        b.encode(s,
            detail::userinfo_pct_set());
    }
}

void
view::
serialize_host(
    detail::any_buffer& b) const
{
    string_view s;
    if(! is_reset())
    {
        s = get(id_hostname);
        if(! s.empty())
            b.encode(s,
                detail::reg_name_pct_set());
    }
    if(port_.has_value())
    {
        b.append(':');
        detail::port_string ps(*port_);
        b.append(ps.data(), ps.size());
    }
}

void
view::
serialize_authority(
    detail::any_buffer& b) const
{
    serialize_userinfo(b);
    if( ! empty(id_username) ||
        ! empty(id_password))
        b.append('@');
    serialize_host(b);
}

void
view::
serialize_origin(
    detail::any_buffer& b) const
{
    auto s = get(id_scheme);
    if(! s.empty())
    {
        b.append(s.data(), s.size());
        b.append("://", 3);
    }
    serialize_host(b);
}

void
view::
serialize_href(
    detail::any_buffer& b) const
{
    string_view s;
    
    s = get(id_scheme);
    if(! s.empty())
    {
        b.append(s.data(), s.size());
        b.append(':');
    }

    if(! empty(id_hostname))
    {
        b.append("//", 2);
        serialize_authority(b);
    }
    else if(
        string_to_scheme(s) ==
        url::scheme::file)
    {
        b.append("//", 2);
    }

    s = get(id_query());
    if(! s.empty())
    {
        b.append('?');
        b.encode(s,
            detail::query_pct_set());
    }

    s = get(id_fragment());
    if(! s.empty())
    {
        b.append('#');
        b.encode(s,
            detail::frag_pct_set());
    }
}

} // url
} // boost

#endif

//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_URL_IMPL_IPP
#define BOOST_URL_DETAIL_IMPL_URL_IMPL_IPP

#include <boost/url/detail/url_impl.hpp>
#include <boost/url/authority_view.hpp>
#include <boost/assert.hpp>
#include <cstring>

namespace boost {
namespace urls {
namespace detail {

void
url_impl::
apply_scheme(
    string_view s) noexcept
{
    scheme_ = string_to_scheme(s);
    set_size(id_scheme, s.size() + 1);
}

void
url_impl::
apply_userinfo(
    pct_encoded_view const& user,
    pct_encoded_view const* pass) noexcept
{
    // this function is for
    // authority_view_rule only
    BOOST_ASSERT(is_authority);

    // userinfo
    set_size(
        id_user,
        user.encoded().size());
    decoded_[id_user] = user.size();
    if(pass)
    {
        set_size(
            id_pass,
            pass->encoded().size() + 2);
        decoded_[id_pass] = pass->size();
    }
    else
    {
        // trailing '@'
        set_size(id_pass, 1 );
    }
}

void
url_impl::
apply_host(
    host_type ht,
    string_view s,
    unsigned char const* addr,
    pct_encoded_view const& name) noexcept
{
    // this function is for
    // authority_view_rule only
    BOOST_ASSERT(is_authority);

    // host, port
    host_type_ = ht;
    set_size(id_host, s.size());
    if(ht == host_type::name)
        decoded_[id_host] = name.size();
    else
        decoded_[id_host] = s.size();
    std::memcpy(
        ip_addr_,
        addr,
        sizeof(ip_addr_));
}

void
url_impl::
apply_port(
    string_view s,
    unsigned short pn) noexcept
{
    // this function is for
    // authority_view_rule only
    BOOST_ASSERT(is_authority);

    port_number_ = pn;
    set_size(id_port, 1 + s.size());
}

void
url_impl::
apply_authority(
    authority_view const& a) noexcept
{
    BOOST_ASSERT(! is_authority);

    // userinfo
    set_size(id_user,
        a.u_.len(id_user) +
        (is_authority ? 0 : 2));
    set_size(id_pass, a.u_.len(id_pass));
    decoded_[id_user] = a.u_.decoded_[id_user];
    decoded_[id_pass] = a.u_.decoded_[id_pass];

    // host, port
    host_type_ = a.u_.host_type_;
    port_number_ = a.u_.port_number_;
    set_size(id_host, a.u_.len(id_host));
    set_size(id_port, a.u_.len(id_port));
    std::memcpy(
        ip_addr_,
        a.u_.ip_addr_,
        sizeof(ip_addr_));
    decoded_[id_host] = a.u_.decoded_[id_host];
}

void
url_impl::
apply_path(
    string_view s,
    std::size_t nseg) noexcept
{
    set_size(id_path, s.size());
    // VFALCO we are decoding twice
    decoded_[id_path] =
        pct_decode_bytes_unchecked(s);
    nseg_ = detail::path_segments(s, nseg);
}

void
url_impl::
apply_query(
    string_view s,
    std::size_t n) noexcept
{
    nparam_ = n;
    set_size(id_query, 1 + s.size());

    // VFALCO we are doing two passes over
    // the string. once for the range and
    // again for the decoded size.
    decoded_[id_query] =
        pct_decode_bytes_unchecked(s);
}

void
url_impl::
apply_frag(
    pct_encoded_view s) noexcept
{
    set_size(id_frag,
        s.encoded().size() + 1);
    decoded_[id_frag] = s.size();
}

} // detail
} // urls
} // boost

#endif

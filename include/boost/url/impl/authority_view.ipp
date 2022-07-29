//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_AUTHORITY_VIEW_IPP
#define BOOST_URL_IMPL_AUTHORITY_VIEW_IPP

#include <boost/url/authority_view.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <array>
#include <ostream>

namespace boost {
namespace urls {

//------------------------------------------------

namespace detail {

authority_view
url_impl::
construct_authority() const noexcept
{
    return authority_view(*this);
}

} // detail

//------------------------------------------------

authority_view::
authority_view(
    detail::url_impl const& u) noexcept
    : u_(u)
{
}

//------------------------------------------------

authority_view::
~authority_view()
{
}

authority_view::
authority_view() noexcept
    : u_(true)
{
}

authority_view::
authority_view(
    authority_view const&) noexcept = default;

authority_view&
authority_view::
operator=(
    authority_view const&) noexcept = default;

//----------------------------------------------------------
//
// Authority
//
//----------------------------------------------------------

// userinfo

bool
authority_view::
has_userinfo() const noexcept
{
    auto n = u_.len(id_pass);
    if(n == 0)
        return false;
    BOOST_ASSERT(u_.get(
        id_pass).ends_with('@'));
    return true;
}

string_view
authority_view::
encoded_userinfo() const noexcept
{
    auto s = u_.get(
        id_user, id_host);
    if(s.empty())
        return s;
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.ends_with('@'));
    s.remove_suffix(1);
    return s;
}

bool
authority_view::
has_password() const noexcept
{
    auto const n = u_.len(id_pass);
    if(n > 1)
    {
        BOOST_ASSERT(u_.get(id_pass
            ).starts_with(':'));
        BOOST_ASSERT(u_.get(id_pass
            ).ends_with('@'));
        return true;
    }
    BOOST_ASSERT(n == 0 || u_.get(
        id_pass).ends_with('@'));
    return false;
}

string_view
authority_view::
encoded_password() const noexcept
{
    auto s = u_.get(id_pass);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(
            s.starts_with('@'));
        BOOST_FALLTHROUGH;
    case 0:
        return s.substr(0, 0);
    default:
        break;
    }
    BOOST_ASSERT(
        s.ends_with('@'));
    BOOST_ASSERT(
        s.starts_with(':'));
    return s.substr(1,
        s.size() - 2);
}

// host

string_view
authority_view::
encoded_host() const noexcept
{
    return u_.get(id_host);
}

urls::ipv4_address
authority_view::
ipv4_address() const noexcept
{
    if(u_.host_type_ !=
        urls::host_type::ipv4)
        return {};
    ipv4_address::bytes_type bytes;
    std::memcpy(
        &bytes[0],
        &u_.ip_addr_[0], 4);
    return urls::ipv4_address(
        bytes);
}

urls::ipv6_address
authority_view::
ipv6_address() const noexcept
{
    if(u_.host_type_ ==
        urls::host_type::ipv6)
    {
        ipv6_address::bytes_type bytes;
        std::memcpy(
            &bytes[0],
            &u_.ip_addr_[0], 16);
        return urls::ipv6_address(
            bytes);
    }
    return urls::ipv6_address();
}

string_view
authority_view::
ipvfuture() const noexcept
{
    if(u_.host_type_ ==
        urls::host_type::ipvfuture)
        return u_.get(id_host);
    return {};
}

// port

bool
authority_view::
has_port() const noexcept
{
    auto const n = u_.len(id_port);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        u_.get(id_port).starts_with(':'));
    return true;
}

string_view
authority_view::
port() const noexcept
{
    auto s = u_.get(id_port);
    if(s.empty())
        return s;
    BOOST_ASSERT(has_port());
    return s.substr(1);
}

std::uint16_t
authority_view::
port_number() const noexcept
{
    BOOST_ASSERT(
        has_port() ||
        u_.port_number_ == 0);
    return u_.port_number_;
}

string_view
authority_view::
encoded_host_and_port() const noexcept
{
    return u_.get(id_host, id_end);
}

//------------------------------------------------
//
// Parsing
//
//------------------------------------------------

result<authority_view>
parse_authority(
    string_view s) noexcept
{
    if(s.size() > authority_view::max_size())
        detail::throw_length_error(
            "authority_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
    return grammar::parse(s, authority_rule);
}

} // urls
} // boost

#endif

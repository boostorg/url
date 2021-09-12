//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_VIEW_IPP
#define BOOST_URL_IMPL_URL_VIEW_IPP

#include <boost/url/url_view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/detail/over_allocator.hpp>
#include <boost/url/detail/parse.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/uri_bnf.hpp>
#include <boost/url/rfc/relative_ref_bnf.hpp>
#include <array>
#include <ostream>

namespace boost {
namespace urls {

string_view
url_view::
get(int id) const noexcept
{
    return pt_.get(id, s_);
}

string_view
url_view::
get(int id0, int id1) const noexcept
{
    return pt_.get(id0, id1, s_);
}

std::size_t
url_view::
len(int id) const noexcept
{
    return pt_.len(id);
}

std::size_t
url_view::
len(int id0, int id1) const noexcept
{
    return pt_.len(id0, id1);
}

//------------------------------------------------

struct url_view::shared_impl :
    url_view
{
    shared_impl(
        url_view const& u) noexcept
        : url_view(reinterpret_cast<
            char const*>(this + 1), u.pt_)
    {
    }
};

std::shared_ptr<
    url_view const>
url_view::
collect() const
{
    using T = shared_impl;
    using Alloc = std::allocator<char>;
    Alloc a;
    string_view s = str();
    auto p = std::allocate_shared<T>(
        detail::over_allocator<T, Alloc>(
            s.size(), a), *this);
    std::memcpy(
        reinterpret_cast<char*>(
            p.get() + 1),
        s.data(), s.size());
    return p;
}

url_view::
url_view() noexcept = default;

//------------------------------------------------
//
// classification
//
//------------------------------------------------

bool
url_view::
empty() const noexcept
{
    return pt_.len() == 0;
}

//------------------------------------------------

string_view
url_view::
encoded_origin() const noexcept
{
    return get(id_scheme, id_path);
}

//------------------------------------------------
//
// scheme
//
//------------------------------------------------

bool
url_view::
has_scheme() const noexcept
{
    auto const n = len(
        id_scheme);
    if(n == 0)
        return false;
    BOOST_ASSERT(n > 1);
    BOOST_ASSERT(
        get(id_scheme
            ).ends_with(':'));
    return true;
}

string_view
url_view::
scheme() const noexcept
{
    auto s = get(
        id_scheme);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() > 1);
        BOOST_ASSERT(s.ends_with(':'));
        s.remove_suffix(1);
    }
    return s;
}

urls::scheme
url_view::
scheme_id() const noexcept
{
    return pt_.scheme;
}

//----------------------------------------------------------
//
// authority
//
//----------------------------------------------------------

bool
url_view::
has_authority() const noexcept
{
    auto const n = len(id_user);
    if(n == 0)
        return false;
    BOOST_ASSERT(get(
        id_user).starts_with("//"));
    return true;
}

string_view
url_view::
encoded_authority() const noexcept
{
    auto s = get(id_user, id_path);
    if(! s.empty())
    {
        BOOST_ASSERT(has_authority());
        s.remove_prefix(2);
    }
    return s;
}

// userinfo

bool
url_view::
has_userinfo() const noexcept
{
    auto n = len(id_pass);
    if(n == 0)
        return false;
    BOOST_ASSERT(has_authority());
    BOOST_ASSERT(get(
        id_pass).ends_with('@'));
    return true;
}

string_view
url_view::
encoded_userinfo() const noexcept
{
    auto s = get(
        id_user, id_host);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        has_authority());
    s.remove_prefix(2);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.ends_with('@'));
    s.remove_suffix(1);
    return s;
}

string_view
url_view::
encoded_user() const noexcept
{
    auto s = get(id_user);
    if(! s.empty())
    {
        BOOST_ASSERT(
            has_authority());
        s.remove_prefix(2);
    }
    return s;
}

bool
url_view::
has_password() const noexcept
{
    auto const n = len(id_pass);
    if(n > 1)
    {
        BOOST_ASSERT(get(
            id_pass
                ).starts_with(':'));
        BOOST_ASSERT(get(
            id_pass
                ).ends_with('@'));
        return true;
    }
    BOOST_ASSERT(n == 0 ||
        get(id_pass
            ).ends_with('@'));
    return false;
}

string_view
url_view::
encoded_password() const noexcept
{
    auto s = get(id_pass);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(
            s.starts_with('@'));
        BOOST_FALLTHROUGH;
    case 0:
        return s.substr(0,0);
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
url_view::
encoded_host() const noexcept
{
    return get(id_host);
}

urls::ipv4_address
url_view::
ipv4_address() const noexcept
{
    if(pt_.host_type ==
        urls::host_type::ipv4)
    {
        std::array<
            unsigned char, 4> bytes;
        std::memcpy(
            &bytes[0],
            &pt_.ip_addr[0], 4);
        return urls::ipv4_address(
            bytes);
    }
    return urls::ipv4_address();
}

urls::ipv6_address
url_view::
ipv6_address() const noexcept
{
    if(pt_.host_type ==
        urls::host_type::ipv6)
    {
        std::array<
            unsigned char, 16> bytes;
        std::memcpy(
            &bytes[0],
            &pt_.ip_addr[0], 16);
        return urls::ipv6_address(
            bytes);
    }
    return urls::ipv6_address();
}

string_view
url_view::
ipv_future() const noexcept
{
    if(pt_.host_type ==
        urls::host_type::ipvfuture)
        return get(id_host);
    return {};
}

// port

bool
url_view::
has_port() const noexcept
{
    auto const n = len(id_port);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        get(id_port).starts_with(':'));
    return true;
}

string_view
url_view::
port() const noexcept
{
    auto s = get(id_port);
    if(s.empty())
        return s;
    BOOST_ASSERT(has_port());
    return s.substr(1);
}

std::uint16_t
url_view::
port_number() const noexcept
{
    BOOST_ASSERT(
        has_port() ||
        pt_.port_number == 0);
    return pt_.port_number;
}

string_view
url_view::
encoded_host_and_port() const noexcept
{
    return get(id_host, id_path);
}
//----------------------------------------------------------
//
// path
//
//----------------------------------------------------------

string_view
url_view::
encoded_path() const noexcept
{
    return get(id_path);
}

path_view
url_view::
path() const noexcept
{
    return path_view(
        get(id_path), pt_.nseg);
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

bool
url_view::
has_query() const noexcept
{
    auto const n = len(
        id_query);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        get(id_query).
            starts_with('?'));
    return true;
}

string_view
url_view::
encoded_query() const noexcept
{
    auto s = get(
        id_query);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.starts_with('?'));
    return s.substr(1);
}

query_params_view
url_view::
query_params() const noexcept
{
    auto s = get(
        id_query);
    if(s.empty())
        return query_params_view(s, 0);
    BOOST_ASSERT(s.starts_with('?'));
    return query_params_view(
        s.substr(1), pt_.nparam);
}

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

bool
url_view::
has_fragment() const noexcept
{
    auto const n = len(
        id_frag);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        get(id_frag).
            starts_with('#'));
    return true;
}

string_view
url_view::
encoded_fragment() const noexcept
{
    auto s = get(
        id_frag);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.starts_with('#'));
    return s.substr(1);
}

//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

string_view
url_view::
str() const
{
    return get(id_scheme, id_end);
}

//------------------------------------------------

url_view
parse_uri(
    string_view s,
    error_code& ec) noexcept
{
    uri_bnf t;
    if(! bnf::parse_string(s, ec, t))
        return {};

    detail::parts p;

    // scheme
    detail::apply(p, t.scheme_part);

    // authority
    if(t.hier_part.has_authority)
        detail::apply(p,
            t.hier_part.authority);

    // path
    detail::apply_path(p,
        t.hier_part.path,
        t.hier_part.path_count);

    // query
    detail::apply(p, t.query_part);

    // fragment
    detail::apply(p, t.fragment_part);

    return url_view(s.data(), p);
}

url_view
parse_uri(
    string_view s)
{
    error_code ec;
    auto u = parse_uri(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return u;
}

url_view
parse_relative_ref(
    string_view s,
    error_code& ec) noexcept
{
    relative_ref_bnf t;
    if(! bnf::parse_string(
            s, ec, t))
        return {};

    detail::parts p;

    // authority
    if(t.relative_part.has_authority)
        detail::apply(p,
            t.relative_part.authority);

    // path
    detail::apply_path(p,
        t.relative_part.path,
        t.relative_part.path_count);

    // query
    detail::apply(p, t.query_part);

    // fragment
    detail::apply(p, t.fragment_part);

    return url_view(s.data(), p);
}

url_view
parse_relative_ref(
    string_view s)
{
    error_code ec;
    auto u = parse_relative_ref(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return u;
}

std::ostream&
operator<<(
    std::ostream& os,
    url_view const& u)
{
    auto s = u.str();
    os.write(s.data(), s.size());
    return os;
}

} // urls
} // boost

#endif

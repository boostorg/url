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
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/authority_bnf.hpp>
#include <boost/url/rfc/fragment_part_bnf.hpp>
#include <boost/url/rfc/host_bnf.hpp>
#include <boost/url/rfc/paths_bnf.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/query_part_bnf.hpp>
#include <boost/url/rfc/relative_ref_bnf.hpp>
#include <boost/url/rfc/scheme_part_bnf.hpp>
#include <boost/url/rfc/uri_bnf.hpp>
#include <array>
#include <ostream>

namespace boost {
namespace urls {

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

string_view
url_view::
get(int id) const noexcept
{
    return pt_.get(id, cs_);
}

string_view
url_view::
get(int id0, int id1) const noexcept
{
    return pt_.get(id0, id1, cs_);
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

url_view::
url_view(
    char const* s,
    detail::parts const& pt) noexcept
    : cs_(s)
    , pt_(pt)
{
    BOOST_ASSERT(cs_ != nullptr);
    if(size() > max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
}

url_view::
url_view() noexcept = default;

//------------------------------------------------
//
// observers
//
//------------------------------------------------

std::shared_ptr<
    url_view const>
url_view::
collect() const
{
    using T = shared_impl;
    using Alloc = std::allocator<char>;
    Alloc a;
    auto p = std::allocate_shared<T>(
        detail::over_allocator<T, Alloc>(
            size(), a), *this);
    std::memcpy(
        reinterpret_cast<char*>(
            p.get() + 1), data(), size());
    return p;
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
    auto s = get(id_scheme);
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
        BOOST_ASSERT(get(id_pass
            ).starts_with(':'));
        BOOST_ASSERT(get(id_pass
            ).ends_with('@'));
        return true;
    }
    BOOST_ASSERT(n == 0 || get(
        id_pass).ends_with('@'));
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
url_view::
encoded_host() const noexcept
{
    return get(id_host);
}

urls::ipv4_address
url_view::
ipv4_address() const noexcept
{
    if(pt_.host_type !=
        urls::host_type::ipv4)
        return {};
    std::array<
        unsigned char, 4> bytes;
    std::memcpy(
        &bytes[0],
        &pt_.ip_addr[0], 4);
    return urls::ipv4_address(
        bytes);
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

string_view
url_view::
encoded_origin() const noexcept
{
    if(len(id_user) < 2)
        return {};
    return get(id_scheme, id_path);
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

std::size_t
url_view::
segment_count() const noexcept
{
    return pt_.nseg;
}

string_view
url_view::
encoded_segment(int index) const noexcept
{
    if(index >= pt_.nseg)
        return {};
    if(- index > pt_.nseg)
        return {};
    return {};
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
    auto s = get(id_query);
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
//
// Parsing
//
//------------------------------------------------

void
url_view::
apply(
    detail::parts& p,
    scheme_part_bnf const& t) noexcept
{
    p.scheme = t.scheme_id;
    if(t.scheme_id !=
        urls::scheme::none)
    p.set_size(
        id_scheme,
        t.scheme_part.size());
}

void
url_view::
apply(
    detail::parts& p,
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

    if(t.host_type !=
        urls::host_type::none)
    {
        p.set_size(
            id_host,
            t.host_part.size());
    }
}

void
url_view::
apply(
    detail::parts& p, 
    authority_bnf const& t) noexcept
{
    if(t.has_userinfo)
    {
        auto const& u = t.userinfo;

        // leading "//" for authority
        p.set_size(
            id_user,
            u.user.str.size() + 2);
        p.decoded[id_user] = u.user.decoded_size;

        if(u.has_password)
        {
            // leading ':' for password,
            // trailing '@' for userinfo
            p.set_size(
                id_pass,
                u.password.str.size() + 2);
            p.decoded[id_pass] =
                u.password.decoded_size;
        }
        else
        {
            // trailing '@' for userinfo
            p.set_size(id_pass, 1);
            p.decoded[id_pass] = 0;
        }
    }
    else
    {
        // leading "//" for authority
        p.set_size(id_user, 2);
        p.decoded[id_user] = 0;
    }

    // host
    apply(p, t.host);

    // port
    if(t.port.has_port)
    {
        p.set_size(
            id_port,
            t.port.port.size() + 1);

        if(t.port.has_number)
            p.port_number =
                t.port.port_number;
    }
}

void
url_view::
apply(
    detail::parts& p,
    parsed_path const& t) noexcept
{
    p.set_size(id_path, t.path.size());
    p.nseg = t.count;
}

void
url_view::
apply(
    detail::parts& p,
    query_part_bnf const& t) noexcept
{
    if(t.has_query)
    {
        p.set_size(
            id_query,
            t.query_part.size());
        p.nparam = t.query.count;
    }
    else
    {
        p.nparam = 0;
    }
}

void
url_view::
apply(
    detail::parts& p,
    fragment_part_bnf const& t) noexcept
{
    if(t.has_fragment)
    {
        p.set_size(
            id_frag,
            t.fragment_part.size());
        p.decoded[id_frag] =
            t.fragment.decoded_size;
    }
    else
    {
        p.decoded[id_frag] = 0;
    }
}

//------------------------------------------------

url_view
parse_absolute_uri(
    string_view s,
    error_code& ec) noexcept
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
    absolute_uri_bnf t;
    if(! bnf::parse_string(s, ec, t))
        return {};

    detail::parts p;

    // scheme
    url_view::apply(p, t.scheme_part);

    // authority
    if(t.hier_part.has_authority)
        url_view::apply(p,
            t.hier_part.authority);

    // path
    url_view::apply(p, t.hier_part.path);

    // query
    url_view::apply(p, t.query_part);

    return url_view(s.data(), p);
}

url_view
parse_absolute_uri(
    string_view s,
    std::error_code& ec) noexcept
{
    error_code ec0;
    auto u = parse_absolute_uri(s, ec0);
    ec = ec0;
    return u;
}

url_view
parse_absolute_uri(
    string_view s)
{
    error_code ec;
    auto u = parse_absolute_uri(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return u;
}

//------------------------------------------------

url_view
parse_uri(
    string_view s,
    error_code& ec) noexcept
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
    uri_bnf t;
    if(! bnf::parse_string(s, ec, t))
        return {};

    detail::parts p;

    // scheme
    url_view::apply(p, t.scheme_part);

    // authority
    if(t.hier_part.has_authority)
        url_view::apply(p,
            t.hier_part.authority);

    // path
    url_view::apply(p, t.hier_part.path);

    // query
    url_view::apply(p, t.query_part);

    // fragment
    url_view::apply(p, t.fragment_part);

    return url_view(s.data(), p);
}

url_view
parse_uri(
    string_view s,
    std::error_code& ec) noexcept
{
    error_code ec0;
    auto u = parse_uri(s, ec0);
    ec = ec0;
    return u;
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

//------------------------------------------------

url_view
parse_relative_ref(
    string_view s,
    error_code& ec) noexcept
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
    relative_ref_bnf t;
    if(! bnf::parse_string(
            s, ec, t))
        return {};

    detail::parts p;

    // authority
    if(t.relative_part.has_authority)
        url_view::apply(p,
            t.relative_part.authority);

    // path
    url_view::apply(p,
        t.relative_part.path);

    // query
    url_view::apply(p, t.query_part);

    // fragment
    url_view::apply(p, t.fragment_part);

    return url_view(s.data(), p);
}

url_view
parse_relative_ref(
    string_view s,
    std::error_code& ec) noexcept
{
    error_code ec0;
    auto u = parse_relative_ref(s, ec0);
    ec = ec0;
    return u;
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

//------------------------------------------------

url_view
parse_uri_reference(
    string_view s,
    error_code& ec) noexcept
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);
    uri_reference_bnf t;
    if(! bnf::parse_string(s, ec, t))
        return {};

    detail::parts p;

    // scheme
    url_view::apply(p, t.scheme_part);

    // authority
    if(t.has_authority)
        url_view::apply(p,
            t.authority);

    // path
    url_view::apply(p, t.path);

    // query
    url_view::apply(p, t.query_part);

    // fragment
    url_view::apply(p, t.fragment_part);

    return url_view(s.data(), p);
}

url_view
parse_uri_reference(
    string_view s,
    std::error_code& ec) noexcept
{
    error_code ec0;
    auto u = parse_uri_reference(s, ec0);
    ec = ec0;
    return u;
}

url_view
parse_uri_reference(
    string_view s)
{
    error_code ec;
    auto u = parse_uri_reference(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return u;
}

//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    url_view const& u)
{
    auto s = u.encoded_url();
    os.write(s.data(), s.size());
    return os;
}

} // urls
} // boost

#endif

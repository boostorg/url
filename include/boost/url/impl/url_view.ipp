//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
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
#include <boost/url/detail/normalize.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/ascii.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/host_rule.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/relative_ref_rule.hpp>
#include <boost/url/rfc/scheme_rule.hpp>
#include <boost/url/rfc/uri_rule.hpp>
#include <array>
#include <ostream>

namespace boost {
namespace urls {

struct url_view::shared_impl :
    url_view
{
    virtual
    ~shared_impl()
    {
    }

    shared_impl(
        url_view const& u) noexcept
        : url_view(u, reinterpret_cast<
            char const*>(this + 1))
    {
    }
};

url_view::
url_view(url_view const&) noexcept = default;

url_view::
url_view(
    int,
    char const* cs) noexcept
    : cs_(cs)
{
}

url_view::
url_view(
    url_view const& u,
    char const* cs) noexcept
    : url_view(u)
{
    cs_ = cs;
}

//------------------------------------------------

url_view::
~url_view()
{
}

url_view::
url_view() noexcept = default;

url_view&
url_view::
operator=(url_view const&) noexcept = default;

url_view::
url_view(string_view s)
{
    auto r = parse_uri_reference(s);
    if(r.has_error())
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
    *this = r.value();
}

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

std::shared_ptr<
    url_view const>
url_view::persist() const
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
// Scheme
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
    return scheme_;
}

//----------------------------------------------------------
//
// Authority
//
//----------------------------------------------------------

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

authority_view
url_view::
authority() const noexcept
{
    string_view s = encoded_authority();
    authority_view a;
    a.cs_ = s.data();
    pos_t off_user = s.data() - data();
    a.offset_[0] = offset(id_pass) - off_user;
    a.offset_[1] = offset(id_host) - off_user;
    a.offset_[2] = offset(id_port) - off_user;
    a.offset_[3] = offset(id_path) - off_user;
    a.decoded_[0] = decoded_[id_user];
    a.decoded_[1] = decoded_[id_pass];
    a.decoded_[2] = decoded_[id_host];
    a.decoded_[3] = decoded_[id_port];
    for (int i = 0; i < 16; ++i)
        a.ip_addr_[i] = ip_addr_[i];
    a.port_number_ = port_number_;
    a.host_type_ = host_type_;
    return a;
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
    if(host_type_ !=
        urls::host_type::ipv4)
        return {};
    std::array<
        unsigned char, 4> bytes;
    std::memcpy(
        &bytes[0],
        &ip_addr_[0], 4);
    return urls::ipv4_address(
        bytes);
}

urls::ipv6_address
url_view::
ipv6_address() const noexcept
{
    if(host_type_ ==
        urls::host_type::ipv6)
    {
        std::array<
            unsigned char, 16> bytes;
        std::memcpy(
            &bytes[0],
            &ip_addr_[0], 16);
        return urls::ipv6_address(
            bytes);
    }
    return urls::ipv6_address();
}

string_view
url_view::
ipv_future() const noexcept
{
    if(host_type_ ==
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
        port_number_ == 0);
    return port_number_;
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
// Query
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
    auto s = get(id_query);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.starts_with('?'));
    return s.substr(1);
}

params_encoded_view
url_view::
encoded_params() const noexcept
{
    auto s = get(id_query);
    if(s.empty())
        return params_encoded_view(s, 0);
    BOOST_ASSERT(s[0] == '?');
    s.remove_prefix(1);
    return params_encoded_view(s, nparam_);
}

params_view
url_view::
params() const noexcept
{
    auto s = get(id_query);
    if(s.empty())
        return {s, 0};
    BOOST_ASSERT(s[0] == '?');
    s.remove_prefix(1);
    return {s, nparam_};
}

//----------------------------------------------------------
//
// Fragment
//
//----------------------------------------------------------

bool
url_view::
has_fragment() const noexcept
{
    auto const n = len(id_frag);
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
    auto s = get(id_frag);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.starts_with('#'));
    return s.substr(1);
}

//------------------------------------------------
//
// Comparisons
//
//------------------------------------------------

int
url_view::
compare(const url_view& other) const noexcept
{
    int comp = detail::ci_compare(
        scheme(),
        other.scheme());
    if ( comp != 0 )
        return comp;

    comp = detail::compare_encoded(
        encoded_user(),
        other.encoded_user());
    if ( comp != 0 )
        return comp;

    comp = detail::compare_encoded(
        encoded_password(),
        other.encoded_password());
    if ( comp != 0 )
        return comp;

    comp = detail::ci_compare_encoded(
        encoded_host(),
        other.encoded_host());
    if ( comp != 0 )
        return comp;

    comp = detail::compare(
        port(),
        other.port());
    if ( comp != 0 )
        return comp;

    comp = detail::normalized_path_compare(
        encoded_path(),
        other.encoded_path(),
        is_path_absolute(),
        other.is_path_absolute());
    if ( comp != 0 )
        return comp;

    comp = detail::compare_encoded(
        encoded_query(),
        other.encoded_query());
    if ( comp != 0 )
        return comp;

    comp = detail::compare_encoded(
        encoded_fragment(),
        other.encoded_fragment());
    if ( comp != 0 )
        return comp;

    return 0;
}


//------------------------------------------------
//
// Parsing
//
//------------------------------------------------

void
url_view::
apply(
    scheme_part_rule const& t) noexcept
{
    scheme_ = t.scheme_id;
    if(t.scheme_id !=
        urls::scheme::none)
    set_size(
        id_scheme,
        t.scheme_part.size());
}

void
url_view::
apply(
    host_rule const& t) noexcept
{
    host_type_ = t.host_type;
    if(t.host_type ==
        urls::host_type::name)
    {
        decoded_[id_host] =
            t.name.size();
    }
    else if(t.host_type ==
        urls::host_type::ipv4)
    {
        auto const bytes =
            t.ipv4.to_bytes();
        decoded_[id_host] = t.host_part.size();
        std::memcpy(
            &ip_addr_[0],
            bytes.data(), 4);
    }
    else if(t.host_type ==
        urls::host_type::ipv6)
    {
        auto const bytes =
            t.ipv6.to_bytes();
        decoded_[id_host] = t.host_part.size();
        std::memcpy(
            &ip_addr_[0],
            bytes.data(), 16);
    }
    else
    {
        decoded_[id_host] = t.host_part.size();
    }

    if(t.host_type !=
        urls::host_type::none)
        set_size(
            id_host,
            t.host_part.size());
}

void
url_view::
apply(
    authority_rule const& t) noexcept
{
    if(t.has_userinfo)
    {
        auto const& u = t.userinfo;

        // leading "//" for authority
        set_size(
            id_user,
            u.user.encoded().size() + 2);
        decoded_[id_user] = u.user.size();

        if(u.has_password)
        {
            // leading ':' for password,
            // trailing '@' for userinfo
            set_size(
                id_pass,
                u.password.encoded().size() + 2);
            decoded_[id_pass] =
                u.password.size();
        }
        else
        {
            // trailing '@' for userinfo
            set_size(id_pass, 1);
            decoded_[id_pass] = 0;
        }
    }
    else
    {
        // leading "//" for authority
        set_size(id_user, 2);
        decoded_[id_user] = 0;
    }

    // host
    apply(t.host);

    // port
    if(t.port.has_port)
    {
        set_size(
            id_port,
            t.port.port.size() + 1);
        if(t.port.has_number)
            port_number_ =
                t.port.port_number;
    }
}

void
url_view::
apply(
    parsed_path const& t) noexcept
{
    auto s = t.path;
    set_size(id_path, s.size());
    decoded_[id_path] =
        pct_decode_bytes_unchecked(t.path);
    nseg_ = detail::path_segments(
        t.path, t.count);
}

void
url_view::
apply(
    query_part_rule const& t) noexcept
{
    if(t.has_query)
    {
        set_size(
            id_query,
            t.query_part.size());
        decoded_[id_query] =
            pct_decode_bytes_unchecked(
                t.query_part.substr(1));
        nparam_ = t.query.v.size();
    }
    else
    {
        decoded_[id_query] = 0;
        nparam_ = 0;
    }
}

void
url_view::
apply(
    fragment_part_rule const& t) noexcept
{
    if(t.has_fragment)
    {
        set_size(
            id_frag,
            t.fragment.encoded().size() + 1);
        decoded_[id_frag] =
            t.fragment.size();
    }
    else
    {
        decoded_[id_frag] = 0;
    }
}

//------------------------------------------------
//
// Parsing
//
//------------------------------------------------

result<url_view>
parse_absolute_uri(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);

    error_code ec;
    absolute_uri_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;

    url_view u(0, s.data());

    // scheme
    u.apply(t.scheme_part);

    // authority
    if(t.hier_part.has_authority)
        u.apply(t.hier_part.authority);

    // path
    u.apply(t.hier_part.path);

    // query
    u.apply(t.query_part);

    return u;
}

result<url_view>
parse_uri(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);

    error_code ec;
    uri_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;

    url_view u(0, s.data());

    // scheme
    u.apply(t.scheme_part);

    // authority
    if(t.hier_part.has_authority)
        u.apply(t.hier_part.authority);

    // path
    u.apply(t.hier_part.path);

    // query
    u.apply(t.query_part);

    // fragment
    u.apply(t.fragment_part);

    return u;
}

result<url_view>
parse_relative_ref(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);

    error_code ec;
    relative_ref_rule t;
    if(! grammar::parse_string(
            s, ec, t))
        return ec;

    url_view u(0, s.data());

    // authority
    if(t.relative_part.has_authority)
        u.apply(t.relative_part.authority);

    // path
    u.apply(t.relative_part.path);

    // query
    u.apply(t.query_part);

    // fragment
    u.apply(t.fragment_part);

    return u;
}

result<url_view>
parse_uri_reference(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "url_view::max_size exceeded",
            BOOST_CURRENT_LOCATION);

    error_code ec;
    uri_reference_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;

    url_view u(0, s.data());

    // scheme
    u.apply(t.scheme_part);

    // authority
    if(t.has_authority)
        u.apply(t.authority);

    // path
    u.apply(t.path);

    // query
    u.apply(t.query_part);

    // fragment
    u.apply(t.fragment_part);

    return u;
}

//------------------------------------------------

} // urls
} // boost

//----------------------------------------------------------
//
// std::hash specialization
//
//----------------------------------------------------------

std::size_t
std::hash<::boost::urls::url_view>::operator()(
    ::boost::urls::url_view const& u) const noexcept
{
    ::boost::urls::detail::fnv_1a hasher(salt_);
    using parts = ::boost::urls::detail::parts_base;
    ::boost::urls::detail::ci_digest(
        u.get(parts::id_scheme), hasher);
    ::boost::urls::detail::digest_encoded(
        u.get(parts::id_user), hasher);
    ::boost::urls::detail::digest_encoded(
        u.get(parts::id_pass), hasher);
    ::boost::urls::detail::ci_digest_encoded(
        u.get(parts::id_host), hasher);
    hasher.put(u.get(parts::id_port));
    ::boost::urls::detail::normalized_path_digest(
        u.get(parts::id_path),
        u.is_path_absolute(),
        hasher);
    ::boost::urls::detail::digest_encoded(
        u.get(parts::id_query), hasher);
    ::boost::urls::detail::digest_encoded(
        u.get(parts::id_frag), hasher);
    return hasher.digest();
}


#endif

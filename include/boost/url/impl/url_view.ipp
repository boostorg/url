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
    return pt_.get(
        id0, id1, s_);
}

std::size_t
url_view::
len(int id) const noexcept
{
    return pt_.length(id);
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

std::shared_ptr<url_view>
url_view::
make_shared() const
{
    using T = shared_impl;
    using Alloc = std::allocator<char>;
    Alloc a;
    string_view s = encoded_url();
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
url_view(string_view s)
    : s_(s.data())
{
    detail::parser pr(s);
    error_code ec;
    detail::parse_url(pt_, s, ec);
    if(ec)
        invalid_part::raise();
}

//------------------------------------------------

string_view
url_view::
encoded_url() const
{
    return get(id_scheme, id_end);
}

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
    auto n = pt_.length(id_pass);
    if(n == 0)
        return false;
    BOOST_ASSERT(has_authority());
    BOOST_ASSERT(get(
        id_pass).ends_with('@'));
    return true;
}

bool
url_view::
has_username() const noexcept
{
    auto const n = len(
        id_user);
    if(n == 0)
    {
        // no authority
        BOOST_ASSERT(pt_.length(
            id_user,
            id_host) == 0);
        return false;
    }
    BOOST_ASSERT(n > 1);
    BOOST_ASSERT(
        has_authority());
    if(n > 2)
        return true;
    return false;
}

string_view
url_view::
encoded_userinfo() const noexcept
{
    auto s = pt_.get(
        id_user,
        id_host,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        has_authority());
    s.remove_prefix(2);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == '@');
    s.remove_suffix(1);
    return s;
}

string_view
url_view::
encoded_username() const noexcept
{
    auto s = pt_.get(
        id_user,
        s_);
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
    auto const n = pt_.length(
        id_pass);
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
    BOOST_ASSERT(pt_.host_type ==
        host_type::ipv4);
    if(pt_.host_type !=
        host_type::ipv4)
        return ipv4_address();
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
    BOOST_ASSERT(pt_.host_type ==
        host_type::ipv6);
    if(pt_.host_type !=
        host_type::ipv6)
        return ipv6_address();
    std::array<
        unsigned char, 16> bytes;
    std::memcpy(
        &bytes[0],
        &pt_.ip_addr[0], 16);
    return urls::ipv6_address(
        bytes);
}

string_view
url_view::
ipv_future() const noexcept
{
    BOOST_ASSERT(pt_.host_type ==
        host_type::ipvfuture);
    if(pt_.host_type !=
        host_type::ipvfuture)
        return {};
    return get(id_host);
}

// port

string_view
url_view::
encoded_host_and_port() const noexcept
{
    return pt_.get(
        id_host,
        id_path,
        s_);
}

bool
url_view::
has_port() const noexcept
{
    auto const n = len(
        id_port);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        get(id_port
            ).starts_with(':'));
    return true;
}

string_view
url_view::
port() const noexcept
{
    auto s =
        get(id_port);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.starts_with(':'));
    return s.substr(1);
}

std::uint16_t
url_view::
port_number() const noexcept
{
    return pt_.port_number;
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
    return pt_.get(
        id_path,
        s_);
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

//----------------------------------------------------------
//
// segments_type
//
//----------------------------------------------------------

url_view::
segments_type::
iterator::
iterator() noexcept
    : s_(nullptr)
    , pt_(nullptr)
    , off_(0)
    , n_(0)
{
}

url_view::
segments_type::
iterator::
iterator(
    segments_type const* v,
    bool end) noexcept
    : s_(v->s_)
    , pt_(v->pt_)
{
    if(! pt_)
    {
        off_ = 0;
        n_ = 0;
    }
    else if( end ||
        pt_->nseg == 0)
    {
        off_ = pt_->offset[
            id_query];
        n_ = 0;
    }
    else
    {
        off_ = pt_->offset[
            id_path];
        parse();
    }
}

auto
url_view::
segments_type::
iterator::
operator*() const noexcept ->
    value_type
{
    string_view s = {
        s_ + off_, n_ };
    if(! s.empty() &&
        s.front() == '/')
        s = s.substr(1);    
    return value_type(s);
}

auto
url_view::
segments_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            id_frag]);
    off_ = off_ + n_;
    if(off_ == pt_->offset[
        id_frag])
    {
        // end
        n_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
url_view::
segments_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            id_path]);
    auto const begin =
        s_ + pt_->offset[
            id_path];
    auto p = s_ + off_;
    while(--p > begin)
    {
        if(*p == '/')
        {
            off_ = p - s_;
            parse();
            return *this;
        }
    }
    // fails for relative-uri
    //BOOST_ASSERT(*p == '/');
    auto const off = p - s_;
    n_ = off_ - off;
    off_ = off;
    return *this;
}

void
url_view::
segments_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(off_ !=
        pt_->offset[
            id_frag]);
    auto const end =
        s_ + pt_->offset[
            id_frag];
    auto const p0 = s_ + off_;
    auto p = p0;
    if(*p == '/')
        ++p;
    while(p < end)
    {
        if(*p == '/')
            break;
        ++p;
    }
    n_ = p - p0;
}

//----------------------------------------------------------

auto
url_view::
segments_type::
begin() const noexcept ->
    iterator
{
    return iterator(this, false);
}

auto
url_view::
segments_type::
end() const noexcept ->
    iterator
{
    return iterator(this, true);
}

//------------------------------------------------

namespace detail {

static
void
apply_host(
    parts& p,
    rfc::host_bnf const& h)
{
    p.host_type = h.host_type();
    switch(h.host_type())
    {
    default:
    case urls::host_type::none:
        break;
    case urls::host_type::name:
        p.decoded[id_host] =
            h.get_name().decoded_size;
        break;
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
        auto const bytes =
            h.get_ipv6().to_bytes();
        std::memcpy(
            &p.ip_addr[0],
            bytes.data(), 16);
        break;
    case urls::host_type::ipvfuture:
        break;
    }

    if(h.host_type() !=
        host_type::none)
    {
        p.resize(
            part::id_host,
            h.str().size());
    }
}

static
void
apply_authority(
    parts& p, optional<
        rfc::authority_bnf> const& t)
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

static
void
apply_path(parts& p, bnf::range<
    rfc::pct_encoded_str> const& t)
{
    p.resize(
        part::id_path,
        t.str().size());
}

static
void
apply_query(parts& p,
    optional<bnf::range<
        rfc::query_param>> const& t)
{
    if(t.has_value())
        p.resize(
            part::id_query,
            t->str().size() + 1);
}

static
void
apply_fragment(
    parts& p,
    optional<rfc::pct_encoded_str> const& t)
{
    if(t.has_value())
        p.resize(
            part::id_frag,
            t->str.size() + 1);
}

} // detail

//------------------------------------------------

optional<url_view>
parse_uri(
    string_view s,
    error_code& ec) noexcept
{
    rfc::uri_bnf t;
    if(! bnf::parse(s, ec, t))
        return none;
    detail::parts p;
    using detail::part;
    p.resize(
        part::id_scheme,
        t.scheme.str.size() + 1);

    // authority
    detail::apply_authority(
        p, t.authority);

    // path
    detail::apply_path(
        p, t.path);

    // query
    detail::apply_query(
        p, t.query);

    // fragment
    detail::apply_fragment(
        p, t.fragment);

    return url_view(
        s.data(), p);
}

url_view
parse_uri(
    string_view s)
{
    error_code ec;
    auto u = parse_uri(s, ec);
    if(ec.failed())
        detail::throw_system_error(ec,
            BOOST_CURRENT_LOCATION);
    BOOST_ASSERT(u.has_value());
    return *u;
}

optional<url_view>
parse_relative_ref(
    string_view s,
    error_code& ec) noexcept
{
    rfc::relative_ref_bnf t;
    if(! bnf::parse(s, ec, t))
        return none;
    detail::parts p;

    // authority
    detail::apply_authority(
        p, t.authority);

    // path
    detail::apply_path(
        p, t.path);

    // query
    detail::apply_query(
        p, t.query);

    // fragment
    detail::apply_fragment(
        p, t.fragment);

    return url_view(
        s.data(), p);
}

url_view
parse_relative_ref(
    string_view s)
{
    error_code ec;
    auto u = parse_relative_ref(s, ec);
    if(ec.failed())
        detail::throw_system_error(ec,
            BOOST_CURRENT_LOCATION);
    BOOST_ASSERT(u.has_value());
    return *u;
}

} // urls
} // boost

#endif

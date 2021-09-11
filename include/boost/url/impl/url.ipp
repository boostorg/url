//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_IPP
#define BOOST_URL_IMPL_URL_IPP

#include <boost/url/url.hpp>
#include <boost/url/error.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/rfc/scheme_bnf.hpp>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace boost {
namespace urls {

string_view
url::
get(int id) const noexcept
{
    return pt_.get(id, s_);
}

string_view
url::
get(int id0, int id1) const noexcept
{
    return pt_.get(
        id0, id1, s_);
}

std::size_t
url::
len(int id) const noexcept
{
    return pt_.len(id);
}

std::size_t
url::
len(int id0, int id1) const noexcept
{
    return pt_.len(id0, id1);
}

//------------------------------------------------

url::
url(char* buf,
    std::size_t cap) noexcept
    : s_(buf)
    , cap_(cap)
{
    BOOST_ASSERT(cap > 0);
    s_[0] = '\0';
}

void
url::
copy(
    char const* s,
    detail::parts const& pt)
{
    auto n = pt.len(
        id_scheme, id_end);
    if(n == 0 && ! s_)
        return;
    if(cap_ < n)
    {
        auto cap = growth_impl(
            cap_, n);
        auto p = alloc_impl(cap);
        if(s_)
            free_impl(s_);
        s_ = p;
        cap_ = cap;
    }
    std::memcpy(s_, s, n);
    s_[n] = 0;
    pt_ = pt;
}

char*
url::
alloc_impl(std::size_t n)
{
    return new char[n + 1];
}

void
url::
free_impl(char* s)
{
    delete[] s;
}

std::size_t
url::
growth_impl(
    std::size_t cap,
    std::size_t new_size)
{
    if(new_size >= std::size_t(-1))
        detail::throw_bad_alloc(
            BOOST_CURRENT_LOCATION);
    BOOST_ASSERT(new_size > cap);
    if(cap == 0)
        return new_size;
    // 50% growth factor
    auto n = cap + (cap >> 1);
    if(n < cap)
    {
        // overflow
        return std::size_t(-1)
            - 1; // for null
    }
    if(n < new_size)
        return new_size;
    return n;
}

//------------------------------------------------

url::
~url()
{
    if(s_)
    {
        BOOST_ASSERT(cap_ != 0);
        free_impl(s_);
    }
}

url::
url() noexcept = default;

url::
url(url&& u) noexcept
{
    s_ = u.s_;
    pt_ = u.pt_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.pt_ = {};
    u.cap_ = 0;
}

url::
url(url const& u)
{
    copy(u.s_, u.pt_);
}

url::
url(url_view const& u)
{
    copy(u.s_, u.pt_);
}

url&
url::
operator=(url&& u) noexcept
{
    if(s_)
        free_impl(s_);
    s_ = u.s_;
    pt_ = u.pt_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.pt_ = {};
    u.cap_ = 0;
    return *this;
}

url&
url::
operator=(url const& u)
{
    copy(u.s_, u.pt_);
    return *this;
}

url&
url::
operator=(url_view const& u)
{
    copy(u.s_, u.pt_);
    return *this;
}

//------------------------------------------------
//************************************************
//************************************************
//************************************************
//------------------------------------------------

//------------------------------------------------
//
// classification
//
//------------------------------------------------

bool
url::
empty() const noexcept
{
    return len(
        id_scheme, id_end) == 0;
}

//------------------------------------------------

string_view
url::
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
url::
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
url::
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
url::
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
url::
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
url::
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
url::
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
url::
encoded_username() const noexcept
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
url::
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
url::
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
url::
encoded_host() const noexcept
{
    return get(id_host);
}

urls::ipv4_address
url::
ipv4_address() const noexcept
{
    BOOST_ASSERT(pt_.host_type ==
        urls::host_type::ipv4);
    if(pt_.host_type !=
        urls::host_type::ipv4)
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
url::
ipv6_address() const noexcept
{
    BOOST_ASSERT(pt_.host_type ==
        urls::host_type::ipv6);
    if(pt_.host_type !=
        urls::host_type::ipv6)
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
url::
ipv_future() const noexcept
{
    BOOST_ASSERT(pt_.host_type ==
        urls::host_type::ipvfuture);
    if(pt_.host_type !=
        urls::host_type::ipvfuture)
        return {};
    return get(id_host);
}

// port

bool
url::
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
url::
port() const noexcept
{
    auto s = get(id_port);
    if(s.empty())
        return s;
    BOOST_ASSERT(has_port());
    return s.substr(1);
}

std::uint16_t
url::
port_number() const noexcept
{
    BOOST_ASSERT(
        has_port() ||
        pt_.port_number == 0);
    return pt_.port_number;
}

string_view
url::
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
url::
encoded_path() const noexcept
{
    return get(id_path);
}

path_view
url::
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
url::
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
url::
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
url::
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
url::
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
url::
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
//************************************************
//************************************************
//************************************************
//------------------------------------------------

string_view
url::
str() const noexcept
{
    return get(id_scheme, id_end);
}

char const*
url::
c_str() const noexcept
{
    static constexpr char
        empty_[] = { '\0' };
    if(s_ == nullptr)
        return empty_;
    return s_;
}

void
url::
clear() noexcept
{
    if(s_)
    {
        pt_ = {};
        s_[0] = '\0';
    }
}

void
url::
reserve(std::size_t cap)
{
    if(cap < cap_)
        return;
    cap = growth_impl(cap_, cap);
    auto s = alloc_impl(cap);
    auto n = len(id_scheme, id_end);
    if(n > 0)
        std::memcpy(s, s_, n + 1);
    if(s_)
        free_impl(s_);
    s_ = s;
    cap_ = cap;
}

//------------------------------------------------

#if 0

url&
url::
set_encoded_origin(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(
            id_scheme,
            id_path, 0);
        return *this;
    }

    error_code ec;
    detail::parts pt;
    detail::parse_origin(pt, s, ec);
    if(ec)
        invalid_part::raise();
    auto const dest =
        resize_impl(
            id_scheme,
            id_path,
            s.size());
    s.copy(dest, s.size());
    pt_.split(
        id_scheme,
        pt.len(id_scheme));
    pt_.split(
        id_user,
        pt.len(id_user));
    pt_.split(
        id_pass,
        pt.len(id_pass));
    pt_.split(
        id_host,
        pt.len(id_host));
    pt_.split(
        id_port, pt.len(id_port));
    return *this;
}
#endif

//------------------------------------------------
//
// scheme
//
//------------------------------------------------

url&
url::
set_scheme(
    string_view s)
{
#if 0
    if(s.empty())
    {
        resize_impl(id_scheme, 0);
        return *this;
    }

    scheme_bnf b;
    bnf::parse(s, b);
    auto const n = s.size();
    auto const dest =
        resize_impl(id_scheme, n + 1);
    s.copy(dest, n);
    dest[n] = ':';
#endif
    return *this;
}

#if 0
//------------------------------------------------
//
// authority
//
//------------------------------------------------

url&
url::
set_encoded_authority(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(
            id_user,
            id_path, 0);
        return *this;
    }

    detail::parts pt;
    detail::parse_authority(pt, s);
    auto const dest = resize_impl(
        id_user,
        id_path,
        2 + s.size());
    //---
    dest[0] = '/';
    dest[1] = '/';
    s.copy(dest + 2, s.size());
    pt_.split(
        id_user,
        2 + pt.len(id_user));
    pt_.split(
        id_pass,
        pt.len(id_pass));
    pt_.split(
        id_host,
        pt.len(id_host));
    BOOST_ASSERT(
        pt_.len(id_port) ==
            pt.len(id_port));
    return *this;
}

//------------------------------------------------
//
// userinfo
//
//------------------------------------------------

url&
url::
set_encoded_userinfo(
    string_view s)
{
    if(s.empty())
    {
        if(pt_.len(
            id_host,
            id_path) == 0)
        {
            // no authority
            resize_impl(
                id_user,
                id_host, 0);
            return *this;
        }
        // keep "//"
        resize_impl(
            id_user,
            id_host, 2);
        return *this;
    }

    detail::parts pt;
    detail::parse_userinfo(pt, s);
    auto dest = resize_impl(
        id_user,
        id_host,
        2 + s.size() + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest += 2;
    s.copy(dest, s.size());
    pt_.split(
        id_user,
        pt.len(id_user));
    dest[s.size()] = '@';
    return *this;
}

url&
url::
set_userinfo_part(
    string_view s)
{
    if(! s.empty())
    {
        if(s.back() != '@')
            invalid_part::raise();
        s.remove_suffix(1);
    }
    return set_encoded_userinfo(s);
}

url&
url::
set_user(
    string_view s)
{
    if(s.empty())
    {
        if(pt_.len(
            id_user) == 0)
            return *this;
        BOOST_ASSERT(pt_.get(
            id_pass, s_).back() == '@');
        BOOST_ASSERT(pt_.get(
            id_user, s_).size() >= 2);
        BOOST_ASSERT(pt_.get(
            id_user, s_)[0] == '/');
        BOOST_ASSERT(pt_.get(
            id_user, s_)[1] == '/');
        if(pt_.len(
            id_pass) == 1)
        {
            // remove '@'
            resize_impl(
                id_user,
                id_host, 2);
        }
        else
        {
            resize_impl(id_user, 2);
        }
        return *this;
    }

    auto const e =
        detail::userinfo_nc_pct_set();
    if(pt_.len(
        id_pass) != 0)
    {
        BOOST_ASSERT(pt_.get(
            id_pass, s_).back() == '@');
        // preserve "//"
        auto const dest = resize_impl(
            id_user,
            2 + e.encoded_size(s));
        e.encode(dest + 2, s);
        return *this;
    }
    auto const n = e.encoded_size(s);
    auto const dest = resize_impl(
        id_user, 2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    pt_.split(
        id_user,
        2 + n);
    e.encode(dest + 2, s);
    return *this;
}

url&
url::
set_encoded_user(
    string_view s)
{
    if(s.empty())
        return set_user(s);

    auto const e =
        detail::userinfo_nc_pct_set();
    e.validate(s);

    auto const n = s.size();
    if(pt_.len(id_pass) != 0)
    {
        BOOST_ASSERT(pt_.get(
            id_pass, s_).back() == '@');
        // preserve "//"
        auto const dest = resize_impl(
            id_user, 2 + n);
        s.copy(dest + 2, n);
        return *this;
    }

    // add '@'
    auto const dest = resize_impl(
        id_user,
        2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    pt_.split(
        id_user,
        2 + n);
    s.copy(dest + 2, n);
    return *this;
}

url&
url::
set_password(
    string_view s)
{
    if(s.empty())
    {
        auto const n = pt_.len(
            id_pass);
        if(n == 0)
            return *this;
        BOOST_ASSERT(pt_.get(
            id_pass, s_).back() == '@');
        BOOST_ASSERT(pt_.get(
            id_user, s_).size() >= 2);
        BOOST_ASSERT(pt_.get(
            id_user, s_)[0] == '/');
        BOOST_ASSERT(pt_.get(
            id_user, s_)[1] == '/');
        if(pt_.len(id_user) == 2)
        {
            // remove '@'
            resize_impl(id_pass, 0);
            return *this;
        }
        // retain '@'
        *resize_impl(id_pass, 1) = '@';
        return *this;
    }

    auto const e =
        detail::userinfo_pct_set();
    auto const n =
        e.encoded_size(s);
    if(pt_.len(id_user) != 0)
    {
        auto const dest = resize_impl(
            id_pass, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        e.encode(dest + 1, s);
        return *this;
    }
    auto const dest = resize_impl(
        id_user,
        id_host,
        2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    e.encode(dest + 3, s);
    pt_.split(id_user, 2);
    return *this;
}

url&
url::
set_encoded_password(
    string_view s)
{
    if(s.empty())
        return set_password(s);

    auto const e =
        detail::userinfo_pct_set();
    if(s[0] == ':')
        invalid_part::raise();
    e.validate(s);

    auto const n = s.size();
    if(pt_.len(id_user) != 0)
    {
        auto const dest = resize_impl(
            id_pass, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        s.copy(dest + 1, n);
        return *this;
    }
    auto const dest = resize_impl(
        id_user,
        id_host,
        2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    s.copy(dest + 3, n);
    pt_.split(id_user, 2);
    return *this;
}

url&
url::
set_password_part(
    string_view s)
{
    if(s.empty())
        return set_password(s);
    if(s.size() == 1)
    {
        if(s.front() != ':')
            invalid_part::raise();
        if(pt_.len(
            id_user) != 0)
        {
            auto const dest = resize_impl(
                id_pass, 2);
            dest[0] = ':';
            dest[1] = '@';
            return *this;
        }
        auto const dest = resize_impl(
            id_user,
            id_host, 4);
        dest[0] = '/';
        dest[1] = '/';
        dest[2] = ':';
        dest[3] = '@';
        pt_.split(
            id_user, 2);
    }
    set_encoded_password(
        s.substr(1));
    return *this;
}

//------------------------------------------------
//
// host
//
//------------------------------------------------

url&
url::
set_host(
    string_view s)
{
    if(s.empty())
    {
        // just hostname
        if( pt_.len(
            id_user,
            id_host) == 2 &&
            pt_.len(id_port) == 0)
        {
            BOOST_ASSERT(pt_.get(
                id_user, s_
                    ) == "//");
            // remove authority
            resize_impl(
                id_user,
                id_path, 0);
        }
        else
        {
            resize_impl(id_host, 0);
        }
        return *this;
    }
    detail::parts pt;
    detail::parse_plain_hostname(pt, s);
    BOOST_ASSERT(pt.host_type !=
        urls::host_type::none);
    if(pt.host_type != urls::host_type::name)
    {
        if(! has_authority())
        {
            // add authority
            auto const dest = resize_impl(
                id_user,
                2 + s.size());
            dest[0] = '/';
            dest[1] = '/';
            pt_.split(
                id_user, 2);
            pt_.split(
                id_pass, 0);
            s.copy(dest + 2, s.size());
        }
        else
        {
            auto const dest = resize_impl(
                id_host,
                s.size());
            s.copy(dest, s.size());
        }
    }
    else
    {
        auto const e =
            detail::reg_name_pct_set();
        if(! has_authority())
        {
            // add authority
            auto const dest = resize_impl(
                id_user,
                2 + e.encoded_size(s));
            dest[0] = '/';
            dest[1] = '/';
            pt_.split(
                id_user, 2);
            pt_.split(
                id_pass, 0);
            e.encode(dest + 2, s);
        }
        else
        {
            auto const dest = resize_impl(
                id_host,
                e.encoded_size(s));
            e.encode(dest, s);
        }
    }
    pt_.host_type = pt.host_type;
    return *this;
}

url&
url::
set_encoded_host(
    string_view s)
{
    if(s.empty())
        return set_host(s);
    detail::parts pt;
    detail::parse_hostname(pt, s);
    if(! has_authority())
    {
        // add authority
        auto const dest = resize_impl(
            id_user,
            2 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        pt_.split(
            id_user, 2);
        pt_.split(
            id_pass, 0);
        s.copy(dest + 2, s.size());
    }
    else
    {
        auto const dest = resize_impl(
            id_host,
            s.size());
        s.copy(dest, s.size());
    }
    pt_.host_type = pt.host_type;
    return *this;
}

url&
url::
set_port(unsigned n)
{
    detail::port_string s(n);
    return set_port(s.get());
}

url&
url::
set_port(string_view s)
{
    if(s.empty())
    {
        // just port
        if(pt_.len(
            id_user,
            id_port) == 2)
        {
            // remove authority
            BOOST_ASSERT(pt_.get(
                id_user, s_).substr(
                    0, 2) == "//");
            resize_impl(
                id_user,
                id_path, 0);
        }
        else
        {
            resize_impl(id_port, 0);
        }
        return *this;
    }
    detail::match_port(s);
    if(! has_authority())
    {
        // add authority
        auto const dest = resize_impl(
            id_user,
            3 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        dest[2] = ':';
        pt_.split(
            id_user, 2);
        pt_.split(
            id_pass, 0);
        pt_.split(
            id_host, 0);
        s.copy(dest + 3, s.size());
    }
    else
    {
        auto const dest = resize_impl(
            id_port,
            1 + s.size());
        dest[0] = ':';
        s.copy(dest + 1, s.size());
    }
    return *this;
}

url&
url::
set_port_part(string_view s)
{
    if(s.empty())
    {
        set_port(s);
        return *this;
    }
    if(s.front() != ':')
        invalid_part::raise();
    if(s.size() > 1)
        return set_port(s.substr(1));
    resize_impl(
        id_port, 1)[0] = ':';
    return *this;
}

//------------------------------------------------
//
// path
//
//------------------------------------------------

url&
url::
set_encoded_path(
    string_view s)
{
    // path-empty
    if(s.empty())
    {
        resize_impl(
            id_path, 0);
        return *this;
    }
    if(has_authority())
    {
        // path-abempty
        detail::match_path_abempty(s);
    }
    else if(s.front() == '/')
    {
        // path-absolute
        detail::match_path_absolute(s);
    }
    else if(pt_.len(
        id_scheme) == 0)
    {
        // path-noscheme
        detail::match_path_noscheme(s);
    }
    else
    {
        // path-rootless
        detail::match_path_rootless(s);
    }
    auto const dest = resize_impl(
        id_path, s.size());
    s.copy(dest, s.size());
    return *this;
}

auto
url::
path() noexcept ->
    segments_type
{
    return segments_type(*this);
}

//------------------------------------------------
//
// query
//
//------------------------------------------------

url&
url::
set_query(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_query, 0);
        return *this;
    }
    auto const e =
        detail::query_pct_set();
    auto const n =
        e.encoded_size(s);
    auto const dest = resize_impl(
        id_query,
        1 + n);
    dest[0] = '?';
    e.encode(dest + 1, s);
    return *this;
}

url&
url::
set_encoded_query(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_query, 0);
        return *this;
    }
    auto const e =
        detail::query_pct_set();
    e.validate(s);
    auto const dest = resize_impl(
        id_query,
        1 + s.size());
    dest[0] = '?';
    s.copy(dest + 1, s.size());
    return *this;
}

url&
url::
set_query_part(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_query, 0);
        return *this;
    }
    if(s.front() != '?')
        invalid_part::raise();
    s = s.substr(1);
    auto const e =
        detail::query_pct_set();
    e.validate(s);
    auto const dest = resize_impl(
        id_query,
        1 + s.size());
    dest[0] = '?';
    s.copy(dest + 1, s.size());
    return *this;
}

auto
url::
query_params() noexcept ->
    params_type
{
    return params_type(*this);
}

//------------------------------------------------
//
// fragment
//
//------------------------------------------------

url&
url::
set_fragment(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_frag, 0);
        return *this;
    }
    auto const e =
        detail::frag_pct_set();
    auto const n =
        e.encoded_size(s);
    auto const dest = resize_impl(
        id_frag, 1 + n);
    dest[0] = '#';
    e.encode(dest + 1, s);
    return *this;
}

url&
url::
set_encoded_fragment(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_frag, 0);
        return *this;
    }
    auto const e =
        detail::frag_pct_set();
    e.validate(s);
    auto const dest = resize_impl(
        id_frag,
        1 + s.size());
    dest[0] = '#';
    s.copy(dest + 1, s.size());
    return *this;
}

url&
url::
set_fragment_part(
    string_view s)
{
    if(s.empty())
    {
        resize_impl(id_frag, 0);
        return *this;
    }
    if(s.front() != '#')
        invalid_part::raise();
    s = s.substr(1);
    auto const e =
        detail::frag_pct_set();
    e.validate(s);
    auto const dest = resize_impl(
        id_frag,
        1 + s.size());
    dest[0] = '#';
    s.copy(dest + 1, s.size());
    return *this;
}

//------------------------------------------------

url&
url::
normalize()
{
    normalize_scheme();
    return *this;
}

url&
url::
normalize_scheme() noexcept
{
    auto n = pt_.len(
        id_scheme);
    if(n == 0)
        return *this;
    --n;
    BOOST_ASSERT(s_[pt_.offset[
        id_scheme] + n] == ':');
    auto p = s_ + pt_.offset[
        id_scheme];
    while(n-- > 0)
    {
        if(static_cast<unsigned char>(
            *p - 65) < 26)
            *p += 32;
        ++p;
    }
    return *this;
}

//------------------------------------------------
//
// segments_type
//
//------------------------------------------------

// check iterator invariants
void
url::
segments_type::
iterator::
check(url const* v) noexcept
{
    BOOST_ASSERT(v != nullptr);
    BOOST_ASSERT(v_ == v);
    BOOST_ASSERT(off_ >=
        v_->pt_.offset[id_path]);
    BOOST_ASSERT(off_ <=
        v_->pt_.offset[id_query]);
}

url::
segments_type::
iterator::
iterator() noexcept
    : v_(nullptr)
    , off_(0)
    , n_(0)
{
}

url::
segments_type::
iterator::
iterator(
    url* v,
    bool end) noexcept
    : v_(v)
{
    if(! v_)
    {
        off_ = 0;
        n_ = 0;
    }
    else if( end ||
        v_->pt_.nseg == 0)
    {
        off_ = v_->pt_.offset[
            id_query];
        n_ = 0;
    }
    else
    {
        off_ = v_->pt_.offset[
            id_path];
        parse();
    }
}

auto
url::
segments_type::
iterator::
operator*() const noexcept ->
    value_type
{
    string_view s = {
        v_->s_ + off_, n_ };
    if(! s.empty() &&
        s.front() == '/')
        s = s.substr(1);
    return value_type(s);
}

auto
url::
segments_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            id_frag]);
    off_ = off_ + n_;
    if(off_ == v_->pt_.offset[
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
url::
segments_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            id_path]);
    auto const begin =
        v_->s_ + v_->pt_.offset[
            id_path];
    auto p = v_->s_ + off_;
    while(--p > begin)
    {
        if(*p == '/')
        {
            off_ = p - v_->s_;
            parse();
            return *this;
        }
    }
    // fails for relative-uri
    //BOOST_ASSERT(*p == '/');
    auto const off = p - v_->s_;
    n_ = off_ - off;
    off_ = off;
    return *this;
}

void
url::
segments_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(off_ !=
        v_->pt_.offset[
            id_frag]);
    auto const end =
        v_->s_ + v_->pt_.offset[
            id_query];
    auto const p0 =
        v_->s_ + off_;
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

//------------------------------------------------

auto
url::
segments_type::
begin() const noexcept ->
    iterator
{
    return iterator(v_, false);
}

auto
url::
segments_type::
end() const noexcept ->
    iterator
{
    return iterator(v_, true);
}

auto
url::
segments_type::
erase( iterator pos ) noexcept ->
    iterator
{
    auto last = pos;
    return erase(pos, ++last);
}

auto
url::
segments_type::
erase( iterator first, iterator last ) noexcept ->
    iterator
{
    first.check(v_);
    last.check(v_);
    BOOST_ASSERT(first.off_ <= last.off_);

    url& v = *v_;
    auto const d = last.off_ - first.off_;

    int c = 0; // number of segments in range
    for(auto i = v.s_ + first.off_,
            e = v.s_ + last.off_; i != e; ++i )
        c += (*i == '/');

    BOOST_ASSERT(v.pt_.nseg >= c);
    v.pt_.nseg -= c;
    std::memmove(
        v.s_ + first.off_,
        v.s_ + last.off_,
        v.pt_.offset[id_end] - last.off_ + 1);
    v.pt_.resize(
        id_path,
        v.pt_.len(id_path, id_query) - d);
    BOOST_ASSERT(v.s_[v.pt_.offset[id_end]] == '\0');
    first.parse();
    return first;
}

auto
url::
segments_type::
insert_encoded_impl( iterator pos, string_view s ) ->
    iterator
{
    pos.check(v_);
    BOOST_ASSERT(detail::pchar_pct_set().check(s));
    BOOST_ASSERT(v_ != nullptr);
    url& v = *v_;
    BOOST_ASSERT(pos.v_ == &v);
    BOOST_ASSERT(pos.off_ >= v.pt_.offset[id_path]);
    BOOST_ASSERT(pos.off_ <= v.pt_.offset[id_query]);
    auto const n0 = v.pt_.offset[id_end];
    auto const n = s.size() + 1;
    v.resize_impl(v.size() + n);
    v.pt_.resize(id_path, v.pt_.len(id_path, id_query) + n);
    std::memmove(v.s_ + v.pt_.offset[id_end] + pos.off_ - n0, v.s_ + pos.off_, n0 - pos.off_ + 1);
    BOOST_ASSERT(v.s_[v.pt_.offset[id_end]] == '\0');
    v.s_[pos.off_] = '/';
    std::memcpy(v.s_ + pos.off_ + 1, s.data(), s.size());
    ++v.pt_.nseg;
    pos.off_ += n;
    pos.parse();
    return pos;
}

auto
url::
segments_type::
insert_impl( iterator pos, string_view s, std::size_t const ns ) ->
    iterator
{
    BOOST_ASSERT(v_ != nullptr);
    url& v = *v_;
    BOOST_ASSERT(pos.v_ == &v);
    BOOST_ASSERT(pos.off_ >= v.pt_.offset[id_path]);
    BOOST_ASSERT(pos.off_ <= v.pt_.offset[id_query]);
    auto const pct = detail::pchar_pct_set();
    BOOST_ASSERT(pct.encoded_size(s) == ns);
    auto const n0 = v.pt_.offset[id_end];
    auto const n = ns + 1;
    v.resize_impl(v.size() + n);
    v.pt_.resize(id_path, v.pt_.len(id_path, id_query) + n);
    std::memmove(v.s_ + v.pt_.offset[id_end] + pos.off_ - n0, v.s_ + pos.off_, n0 - pos.off_ + 1);
    BOOST_ASSERT(v.s_[v.pt_.offset[id_end]] == '\0');
    v.s_[pos.off_] = '/';
    pct.encode(v.s_ + pos.off_ + 1, s);
    ++v.pt_.nseg;
    pos.off_ += n;
    pos.parse();
    return pos;
}

auto
url::
segments_type::
insert_encoded( iterator pos, string_view s ) ->
    iterator
{
    return insert_encoded_impl(pos, detail::pchar_pct_set().validate(s));
}

auto
url::
segments_type::
insert( iterator pos, string_view s ) ->
    iterator
{
    return insert_impl(pos, s, detail::pchar_pct_set().encoded_size(s));
}

auto
url::
segments_type::
replace_encoded( iterator pos, string_view s ) ->
    iterator
{
    detail::pchar_pct_set().validate(s);
    BOOST_ASSERT(v_ != nullptr);
    url& v = *v_; (void)v;
    //auto const ns = s.size();
    //auto const n0 = pos.n_;
    //auto const n = ns + 1;
// VFALCO This is to make insert and erase compose
//        and offer the strong exception safety guarantee
//if( n0 < n ) v.s_ = v.a_->reserve(v.size() + n - n0);
    auto r = insert_encoded_impl(erase(pos), s);
    return r;
}

auto
url::
segments_type::
replace( iterator pos, string_view s ) ->
    iterator
{
    BOOST_ASSERT(v_ != nullptr);
    url& v = *v_; (void)v;
    auto const ns = detail::pchar_pct_set().encoded_size(s);
    //auto const n0 = pos.n_;
    //auto const n = ns + 1;
// VFALCO This is to make insert and erase compose
//        and offer the strong exception safety guarantee
//if( n0 < n ) v.s_ = v.a_->reserve(v.size() + n - n0);
    auto r = insert_impl(erase(pos), s, ns);
    return r;
}

//------------------------------------------------
//
// params_type
//
//------------------------------------------------

url::
params_type::
iterator::
iterator() noexcept
    : v_(nullptr)
    , off_(0)
    , nk_(0)
    , nv_(0)
{
}

url::
params_type::
iterator::
iterator(
    url* v,
    bool end) noexcept
    : v_(v)
{
    if(! v_)
    {
        off_ = 0;
        nk_ = 0;
        nv_ = 0;
    }
    else if( end ||
            v_->pt_.nparam == 0)
    {
        off_ = v_->pt_.offset[
            id_frag];
        nk_ = 0;
        nv_ = 0;
    }
    else
    {
        off_ = v_->pt_.offset[
            id_query];
        parse();
    }
}

auto
url::
params_type::
iterator::
operator*() const noexcept ->
    value_type
{
    BOOST_ASSERT(v_);
    BOOST_ASSERT(v_->pt_.nparam > 0);
    BOOST_ASSERT(nk_ > 0);
    BOOST_ASSERT(
        off_ == v_->pt_.offset[
            id_query] ?
        v_->s_[off_] == '?' :
        v_->s_[off_] == '&');
    string_view const k = {
        v_->s_ + off_ + 1,
        nk_ - 1 };
    if(nv_ == 0)
        return { k, { } };
    BOOST_ASSERT(
        v_->s_[off_ + nk_] == '=');
    string_view const v = {
        v_->s_ + off_ + nk_ + 1,
        nv_ - 1};
    return { k, v };
}

auto
url::
params_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(v_);
    BOOST_ASSERT(v_->pt_.nparam > 0);
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            id_frag]);
    off_ = off_ + nv_ + nk_;
    if(off_ == v_->pt_.offset[
        id_frag])
    {
        // end
        nv_ = 0;
        nk_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
url::
params_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(v_);
    BOOST_ASSERT(v_->pt_.nparam > 0);
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            id_query]);
    auto const begin =
        v_->s_ + v_->pt_.offset[
            id_query];
    auto p = v_->s_ + off_;
    while(--p > begin)
    {
        if(*p == '&')
        {
            off_ = p - v_->s_;
            parse();
            return *this;
        }
    }
    BOOST_ASSERT(*p == '?');
    off_ = p - v_->s_;
    return *this;
}

void
url::
params_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(v_);
    BOOST_ASSERT(v_->pt_.nparam > 0);
    auto const end =
        v_->s_ + v_->pt_.offset[
            id_end];
    char const* p = v_->s_ + off_;
    BOOST_ASSERT(
        ( off_ == v_->pt_.offset[
            id_query] &&
            *p == '?' ) ||
        ( off_ != v_->pt_.offset[
            id_query] &&
            *p == '&' ));
    auto p0 = p++;
    auto const ek =
        detail::qkey_pct_set();
    error_code ec;
    p = ek.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nk_ = p - p0;
    if(p == end)
    {
        nv_ = 0;
        return;
    }
    auto const ev =
        detail::qval_pct_set();
    BOOST_ASSERT(*p == '=');
    p0 = p++;
    p = ev.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nv_ = p - p0;
}

//------------------------------------------------

auto
url::
params_type::
begin() const noexcept ->
    iterator
{
    return iterator(v_, false);
}

auto
url::
params_type::
end() const noexcept ->
    iterator
{
    return iterator(v_, true);
}

bool
url::
params_type::
contains(string_view key) const noexcept
{
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            return true;
    return false;
}

std::size_t
url::
params_type::
count(string_view key) const noexcept
{
    std::size_t n = 0;
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            ++n;
    return n;
}

auto
url::
params_type::
find(string_view key) const noexcept ->
    iterator
{
    auto it = begin();
    for(auto const last = end();
        it != last; ++it)
        if(detail::key_equal(
            it->encoded_key(),
            key))
            break;
    return it;
}

std::string
url::
params_type::
operator[](string_view key) const
{
    auto const it = find(key);
    if(it == end())
        return "";
    return it->value();
}

//------------------------------------------------

void
url::
resize_impl(
    std::size_t new_size)
{
    if(new_size > cap_)
    {
        // reallocate
        auto p = static_cast<char*>(
            sp_->allocate(new_size + 1));
        if(s_)
        {
            BOOST_ASSERT(cap_ != 0);
            std::memcpy(p, s_, size() + 1);
            sp_->deallocate(s_, cap_ + 1, 1);
        }
        s_ = p;
        cap_ = new_size;
    }

    s_[new_size] = '\0';
}

char*
url::
resize_impl(
    int id,
    std::size_t new_size)
{
    return resize_impl(id, id + 1, new_size);
}

char*
url::
resize_impl(
    int first,
    int last,
    std::size_t new_len)
{
    auto const len =
        pt_.len(first, last);
    if(new_len == 0 && len == 0)
    {
        // VFALCO This happens
        //BOOST_ASSERT(s_ != nullptr);
        return s_ + pt_.offset[first];
    }

    if(new_len <= len)
    {
        // shrinking
        auto const n = static_cast<
            std::size_t>(len - new_len);
        auto const pos = pt_.offset[last];
        std::memmove(
            s_ + pos - n,
            s_ + pos,
            pt_.offset[
                id_end] - pos + 1);
        for(auto i = first + 1;
            i < last; ++i)
            pt_.offset[i] =
                pt_.offset[last] - n;
        for(auto i = last;
            i <= id_end; ++i)
            pt_.offset[i] -= n;
        s_[size()] = '\0';
        return s_ + pt_.offset[first];
    }

    // growing
    auto const n = static_cast<
        std::size_t>(new_len - len);

    // check for exceeding max size
    if(n > (
        (std::size_t)-1)/*max_size()*/ - size())
        too_large::raise();

    if(cap_ < size() + n)
    {
        // reallocate
        auto p = static_cast<char*>(
            sp_->allocate(cap_ + n + 1));
        if(s_)
        {
            BOOST_ASSERT(cap_ != 0);
            std::memcpy(p, s_, size() + 1);
            sp_->deallocate(s_, cap_ + 1, 1);
        }
        s_ = p;
        cap_ = cap_ + n;
    }

    auto const pos =
        pt_.offset[last];
    std::memmove(
        s_ + pos + n,
        s_ + pos,
        pt_.offset[id_end] -
            pos + 1);
    for(auto i = first + 1;
        i < last; ++i)
        pt_.offset[i] =
            pt_.offset[last] + n;
    for(auto i = last;
        i <= id_end; ++i)
        pt_.offset[i] += n;

    s_[size()] = '\0';
    return s_ + pt_.offset[first];
}

#endif

} // urls
} // boost

#endif

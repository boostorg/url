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
#include <boost/url/pct_encoding.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/detail/print.hpp>
#include <boost/url/rfc/authority_bnf.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/host_bnf.hpp>
#include <boost/url/rfc/port_bnf.hpp>
#include <boost/url/rfc/scheme_bnf.hpp>
#include <boost/url/rfc/userinfo_bnf.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace boost {
namespace urls {

void
url::
check_invariants() const noexcept
{
    BOOST_ASSERT(
        len(id_scheme) == 0 ||
        get(id_scheme).ends_with(':'));
    BOOST_ASSERT(
        len(id_user) == 0 ||
        get(id_user).starts_with("//"));
    BOOST_ASSERT(
        len(id_pass) == 0 ||
        get(id_user).starts_with("//"));
    BOOST_ASSERT(
        len(id_pass) == 0 ||
        (len(id_pass) == 1 &&
            get(id_pass) == "@") ||
        (len(id_pass) > 1 &&
            get(id_pass).starts_with(':') &&
            get(id_pass).ends_with('@')));
    BOOST_ASSERT(
        len(id_user, id_path) == 0 ||
        get(id_user).starts_with("//"));
    BOOST_ASSERT(
        len(id_port) == 0 ||
        get(id_port).starts_with(':'));
    BOOST_ASSERT(
        len(id_query) == 0 ||
        get(id_query).starts_with('?'));
    BOOST_ASSERT(
        len(id_frag) == 0 ||
        get(id_frag).starts_with('#'));
    BOOST_ASSERT(c_str()[
        pt_.offset[id_end]] == '\0');
#if 0
    BOOST_ASSERT(
        (len(id_user) <= 2 &&
            pt_.decoded[id_user] == 0) ||
        (len(id_user) > 2 &&
            pt_.decoded[id_user] != 0));
    BOOST_ASSERT(
        (len(id_pass) <= 2 &&
            pt_.decoded[id_pass] == 0) ||
        (len(id_pass) > 2 &&
            pt_.decoded[id_pass] != 0));
    BOOST_ASSERT(
        (len(id_host) == 0 &&
            pt_.decoded[id_host] == 0) ||
        (len(id_host) != 0 &&
            pt_.decoded[id_host] != 0));
    BOOST_ASSERT(
        (len(id_query) <= 1 &&
            pt_.decoded[id_query] == 0) ||
        (len(id_query) > 1 &&
            pt_.decoded[id_query] != 0));
    BOOST_ASSERT(
        (len(id_frag) <= 1 &&
            pt_.decoded[id_frag] == 0) ||
        (len(id_frag) > 1 &&
            pt_.decoded[id_frag] != 0));
#endif
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
    cs_ = s_;
}

void
url::
copy(
    char const* s,
    detail::parts const& pt)
{
    auto n = pt.offset[id_end];
    if(n == 0 && ! s_)
        return;
    if(cap_ < n)
    {
        auto cap = growth_impl(
            cap_, n);
        auto s1 = alloc_impl(cap);
        if(s_)
            free_impl(s_);
        s_ = s1;
        cs_ = s_;
        cap_ = cap;
    }
    if(n != 0)
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
    if(new_size > max_size())
        detail::throw_length_error(
            "url::reserve",
            BOOST_CURRENT_LOCATION);
    BOOST_ASSERT(new_size > cap);
    if(cap == 0)
    {
        // minimum
        if( new_size < 24)
            return 24;
        return new_size;
    }
    // 50% growth factor
    auto n = cap + cap / 2;
    if(n < cap)
    {
        // overflow
        return max_size();
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
    cs_ = u.cs_;
    pt_ = u.pt_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.cs_ = "";
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
    copy(u.cs_, u.pt_);
}

url&
url::
operator=(url&& u) noexcept
{
    if(s_)
        free_impl(s_);
    s_ = u.s_;
    cs_ = s_;
    pt_ = u.pt_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.cs_ = "";
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
    copy(u.cs_, u.pt_);
    return *this;
}

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
    else
        s[0] = '\0';
    if(s_)
        free_impl(s_);
    s_ = s;
    cs_ = s_;
    cap_ = cap;
}

//------------------------------------------------
//
// scheme
//
//------------------------------------------------

void
url::
set_scheme_impl(
    string_view s,
    urls::scheme id)
{
    check_invariants();
    scheme_bnf b;
    error_code ec;
    bnf::parse_string(s, ec, b);
    if(ec.failed())
        detail::throw_invalid_argument(
            "url::set_scheme",
            BOOST_CURRENT_LOCATION);
    auto n = s.size();
    auto dest = resize_impl(
        id_scheme, n + 1);
    s.copy(dest, n);
    dest[n] = ':';
    pt_.scheme = id;
    check_invariants();
    return;
}

url&
url::
remove_scheme() noexcept
{
    check_invariants();
    auto const n = len(id_scheme);
    if(n == 0)
        return *this;

    // Check if we are changing
    // path-rootless to path-noscheme
    bool const need_dot = [this]
    {
        if(len(id_user) >= 2)
        {
            // authority
            return false;
        }
        auto s = get(id_path);
        if(s.empty())
        {
            // path-empty
            return false;
        }
        if(s.starts_with('/'))
        {
            // path-absolute
            return false;
        }
        auto const p = url_view::path();
        BOOST_ASSERT(! p.empty());
        auto it = p.begin();
        s = it->encoded_segment();
        if(s.find_first_of(':') ==
            string_view::npos)
        {
            // path-noscheme
            return false;
        }
        return true;
    }();

    if(! need_dot)
    {
        // just remove the scheme
        resize_impl(id_scheme, 0);
        pt_.scheme = urls::scheme::none;
        check_invariants();
        return *this;
    }

    // remove the scheme but add "./"
    // to the beginning of the path
    BOOST_ASSERT(n >= 2);
    std::memmove(
        s_, s_ + n,
        pt_.offset[id_path] - n);
    std::memmove(
        s_ + pt_.offset[
            id_path] - (n - 2),
        s_ + pt_.offset[id_path],
        pt_.offset[id_end] -
            pt_.offset[id_path]);
    pt_.offset[id_scheme] = 0;
    for(int i = id_user;
            i <= id_path; ++i)
        pt_.offset[i] -= n;
    for(int i = id_query;
            i <= id_end; ++i)
        pt_.offset[i] -= n - 2;
    auto dest = s_ +
        pt_.offset[id_path];
    dest[0] = '.';
    dest[1] = '/';
    dest[pt_.offset[id_end]] = '\0';
    ++pt_.nseg;
    pt_.scheme = urls::scheme::none;
    check_invariants();
    return *this;
}

url&
url::
set_scheme(string_view s)
{
    set_scheme_impl(
        s, string_to_scheme(s));
    return *this;
}

url&
url::
set_scheme(urls::scheme id)
{
    if(id == urls::scheme::unknown)
        detail::throw_invalid_argument(
            "url::set_scheme",
            BOOST_CURRENT_LOCATION);
    if(id == urls::scheme::none)
        return remove_scheme();
    set_scheme_impl(
        to_string(id), id);
    return *this;
}

//------------------------------------------------
//
// authority
//
//------------------------------------------------

char*
url::
set_user_impl(std::size_t n)
{
    check_invariants();
    if(len(id_pass) != 0)
    {
        // keep "//"
        auto dest = resize_impl(
            id_user, 2 + n);
        check_invariants();
        return dest + 2;
    }
    // add authority
    auto dest = resize_impl(
        id_user, 2 + n + 1);
    pt_.split(id_user, 2 + n);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    check_invariants();
    return dest + 2;
}

url&
url::
remove_user() noexcept
{
    check_invariants();
    if(len(id_user) == 0)
    {
        // no authority
    }
    else if(len(id_pass) == 1)
    {
        // no password, remove '@'
        resize_impl(
            id_user, id_host, 2);
    }
    else
    {
        // keep password
        resize_impl(id_user, 2);
    }
    pt_.decoded[id_user] = 0;
    check_invariants();
    return *this;
 }

url&
url::
set_user(string_view s)
{
    check_invariants();
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars;
    auto const n =
        pct_encode_size(s, cs);
    auto dest = set_user_impl(n);
    dest = detail::pct_encode(
        dest, get(id_pass).data(),
            s, {},  cs);
    pt_.decoded[id_user] = s.size();
    BOOST_ASSERT(dest ==
        get(id_pass).data());
    check_invariants();
    return *this;
}

url&
url::
set_encoded_user(
    string_view s)
{
    check_invariants();
    error_code ec;
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars;
    auto const n =
        pct_decode_size(s, ec, cs);
    if(ec.failed())
        detail::throw_invalid_argument(
            "url::set_encoded_user",
            BOOST_CURRENT_LOCATION);
    auto dest = set_user_impl(s.size());
    pt_.decoded[id_user] = n;
    if(! s.empty())
    {
        BOOST_ASSERT(dest != nullptr);
        std::memcpy(dest,
            s.data(), s.size());
    }
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url::
set_password_impl(
    std::size_t n)
{
    check_invariants();
    if(len(id_user) != 0)
    {
        // already have authority
        auto const dest = resize_impl(
            id_pass, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        check_invariants();
        return dest + 1;
    }
    // add authority
    auto const dest =
        resize_impl(
        id_user, id_host,
        2 + 1 + n + 1);
    pt_.split(id_user, 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    check_invariants();
    return dest + 3;
}

url&
url::
remove_password() noexcept
{
    check_invariants();
    auto const n = len(id_pass);
    if(n < 2)
        return *this;
    // clear password, retain '@'
    auto dest =
        resize_impl(id_pass, 1);
    dest[0] = '@';
    pt_.decoded[id_pass] = 0;
    check_invariants();
    return *this;
}

url&
url::
set_password(string_view s)
{
    check_invariants();
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars + ':';
    auto const n =
        pct_encode_size(s, cs);
    auto dest = set_password_impl(n);
    dest = detail::pct_encode(dest,
        get(id_host).data() - 1,
            s, {},  cs);
    pt_.decoded[id_pass] = s.size();
    BOOST_ASSERT(dest ==
        get(id_host).data() - 1);
    check_invariants();
    return *this;
}

url&
url::
set_encoded_password(
    string_view s)
{
    check_invariants();
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars + ':';
    error_code ec;
    auto const n =
        pct_decode_size(s, ec, cs);
    if(ec.failed())
        detail::throw_invalid_argument(
            "url::set_encoded_password",
            BOOST_CURRENT_LOCATION);
    auto dest =
        set_password_impl(s.size());
    pt_.decoded[id_pass] = n;
    if(! s.empty())
    {
        BOOST_ASSERT(dest != nullptr);
        std::memcpy(dest,
            s.data(), s.size());
    }
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url::
set_userinfo_impl(
    std::size_t n)
{
    // "//" {dest} "@"
    check_invariants();
    auto dest = resize_impl(
        id_user, id_host, n + 3);
    pt_.split(id_user, n + 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[n + 2] = '@';
    check_invariants();
    return dest + 2;
}

url&
url::
remove_userinfo() noexcept
{
    check_invariants();
    if(len(id_pass) == 0)
    {
        // no userinfo
        return *this;
    }
    // keep authority '//'
    resize_impl(
        id_user, id_host, 2);
    pt_.decoded[id_user] = 0;
    pt_.decoded[id_pass] = 0;
    check_invariants();
    return *this;
}

url&
url::
set_userinfo(
    string_view s)
{
    check_invariants();
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars;
    auto const n =
        pct_encode_size(s, cs);
    auto dest = set_userinfo_impl(n);
    dest = detail::pct_encode(dest,
        get(id_host).data() - 1,
            s, {},  cs);
    pt_.decoded[id_user] = s.size();
    BOOST_ASSERT(dest ==
        get(id_host).data() - 1);
    check_invariants();
    return *this;
}

url&
url::
set_encoded_userinfo(
    string_view s)
{
    check_invariants();
    error_code ec;
    userinfo_bnf t;
    if(! bnf::parse_string(s, ec, t))
        detail::throw_invalid_argument(
            "url::set_encoded_userinfo",
            BOOST_CURRENT_LOCATION);
    auto dest = set_userinfo_impl(s.size());
    pt_.split(id_user, 2 + t.user.str.size());
    if(! s.empty())
        std::memcpy(dest, s.data(), s.size());
    pt_.decoded[id_user] =
        t.user.decoded_size;
    if(t.has_password)
        pt_.decoded[id_pass] =
            t.password.decoded_size;
    else
        pt_.decoded[id_pass] = 0;
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url::
set_host_impl(std::size_t n)
{
    check_invariants();
    if(len(id_user) == 0)
    {
        // add authority
        auto dest = resize_impl(
            id_user, n + 2);
        pt_.split(id_user, 2);
        pt_.split(id_pass, 0);
        dest[0] = '/';
        dest[1] = '/';
        check_invariants();
        return dest + 2;
    }
    // already have authority
    auto const dest =
        resize_impl(id_host, n);
    check_invariants();
    return dest;
}

url&
url::
set_host(
    urls::ipv4_address const& addr)
{
    check_invariants();
    char buf[urls::
        ipv4_address::max_str_len];
    auto s = addr.to_buffer(
        buf, sizeof(buf));
    auto dest =
        set_host_impl(s.size());
    std::memcpy(
        dest, s.data(), s.size());
    pt_.decoded[id_host] = len(id_host);
    pt_.host_type =
        urls::host_type::ipv4;
    auto bytes = addr.to_bytes();
    std::memcpy(pt_.ip_addr,
        bytes.data(), bytes.size());
    check_invariants();
    return *this;
}

url&
url::
set_host(
    urls::ipv6_address const& addr)
{
    check_invariants();
    char buf[2 + urls::
        ipv6_address::max_str_len];
    auto s = addr.to_buffer(
        buf + 1, sizeof(buf) - 2);
    buf[0] = '[';
    buf[s.size() + 1] = ']';
    auto dest =
        set_host_impl(s.size() + 2);
    std::memcpy(
        dest, buf, s.size() + 2);
    pt_.decoded[id_host] = len(id_host);
    pt_.host_type =
        urls::host_type::ipv6;
    auto bytes = addr.to_bytes();
    std::memcpy(pt_.ip_addr,
        bytes.data(), bytes.size());
    check_invariants();
    return *this;
}

url&
url::
set_host(
    string_view s)
{
    // try ipv4
    {
        error_code ec;
        auto a = make_ipv4_address(s, ec);
        if(! ec.failed())
            return set_host(a);
    }
    check_invariants();
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars;
    auto const n =
        pct_encode_size(s, cs);
    auto dest = set_host_impl(n);
    dest = detail::pct_encode(
        dest, get(id_path).data(),
            s, {},  cs);
    pt_.decoded[id_host] = s.size();
    pt_.host_type =
        urls::host_type::name;
    check_invariants();
    return *this;
}

url&
url::
set_encoded_host(string_view s)
{
    // first try parsing it
    host_bnf t;
    error_code ec;
    if(! bnf::parse_string(s, ec, t))
        detail::throw_invalid_argument(
            "url::set_encoded_host",
            BOOST_CURRENT_LOCATION);
    BOOST_ASSERT(t.host_type !=
        urls::host_type::none);
    if(t.host_type ==
            urls::host_type::ipv4)
        return set_host(t.ipv4);
    if(t.host_type ==
            urls::host_type::ipv6)
        return set_host(t.ipv6);

    // set it as a name or ipvfuture
    check_invariants();
    pct_decode_opts opt;
    static constexpr auto cs =
        unreserved_chars +
        subdelim_chars;
    opt.plus_to_space = false;
    auto const n = pct_decode_size(
        s, ec, cs, opt);
    if(ec.failed())
        detail::throw_invalid_argument(
            "url::set_encoded_host",
            BOOST_CURRENT_LOCATION);
    auto dest =
        set_host_impl(s.size());
    std::memcpy(
        dest, s.data(), s.size());
    pt_.decoded[id_host] = n;
    pt_.host_type = t.host_type;
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url::
set_port_impl(std::size_t n)
{
    check_invariants();
    if(len(id_user) != 0)
    {
        // authority exists
        auto dest = resize_impl(
            id_port, n + 1);
        dest[0] = ':';
        check_invariants();
        return dest + 1;
    }
    auto dest = resize_impl(
        id_user, 3 + n);
    pt_.split(id_user, 2);
    pt_.split(id_pass, 0);
    pt_.split(id_host, 0);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    check_invariants();
    return dest + 3;
}

url&
url::
remove_port() noexcept
{
    check_invariants();
    resize_impl(id_port, 0);
    pt_.port_number = 0;
    check_invariants();
    return *this;
}

url&
url::
set_port(std::uint16_t n)
{
    check_invariants();
    auto s =
        detail::make_printed(n);
    auto dest = set_port_impl(
        s.str().size());
    std::memcpy(
        dest, s.str().data(),
            s.str().size());
    pt_.port_number = n;
    check_invariants();
    return *this;
}

url&
url::
set_port(string_view s)
{
    check_invariants();
    port_bnf t;
    error_code ec;
    if(! bnf::parse_string(
            s, ec, t))
        detail::throw_invalid_argument(
            "url::set_port",
            BOOST_CURRENT_LOCATION);
    auto dest =
        set_port_impl(t.str.size());
    std::memcpy(dest,
        t.str.data(), t.str.size());
    if(t.has_number)
        pt_.port_number = t.number;
    else
        pt_.port_number = 0;
    check_invariants();
    return *this;
}

//------------------------------------------------

url&
url::
remove_authority() noexcept
{
    check_invariants();
    if(len(id_user) == 0)
    {
        // no authority
        return *this;
    }
    // remove authority
    resize_impl(
        id_user, id_path, 0);
    pt_.host_type =
        urls::host_type::none;
    check_invariants();
    return *this;
}

url&
url::
set_encoded_authority(string_view s)
{
    error_code ec;
    authority_bnf t;
    if(! bnf::parse_string(
            s, ec, t))
        detail::throw_invalid_argument(
            "url::set_encoded_authority",
            BOOST_CURRENT_LOCATION);
    auto n = s.size();
    auto dest = resize_impl(
        id_user, id_path, n + 2);
    dest[0] = '/';
    dest[1] = '/';
    std::memcpy(dest + 2,
        s.data(), s.size());
    if(t.has_userinfo)
    {
        auto const& t0 = t.userinfo;
        pt_.split(id_user,
            2 + t0.user.str.size());
        n -= t0.user.str.size();
        pt_.decoded[id_user] =
            t0.user.decoded_size;
        if(t0.has_password)
        {
            pt_.split(id_pass, 2 +
                t0.password.str.size());
            pt_.decoded[id_pass] =
                t0.password.decoded_size;
            n -= 1 + t0.password.str.size();
        }
        else
        {
            pt_.split(id_pass, 1);
            pt_.decoded[id_pass] = 0;
            n -= 1;
        }
    }
    else
    {
        pt_.split(id_user, 2);
        pt_.split(id_pass, 0);
    }
    pt_.split(id_host,
        t.host.host_part.size());
    n -= t.host.host_part.size();
    pt_.host_type = t.host.host_type;
    if(pt_.host_type ==
        urls::host_type::ipv4)
    {
        auto const bytes =
            t.host.ipv4.to_bytes();
        std::memcpy(pt_.ip_addr,
            bytes.data(), bytes.size());
        pt_.decoded[id_host] =
            len(id_host);
    }
    else if(pt_.host_type ==
        urls::host_type::ipv6)
    {
        auto const bytes =
            t.host.ipv6.to_bytes();
        std::memcpy(pt_.ip_addr,
            bytes.data(), bytes.size());
        pt_.decoded[id_host] =
            len(id_host);
    }
    else if(pt_.host_type ==
        urls::host_type::ipvfuture)
    {
        pt_.decoded[id_host] =
            len(id_host);
    }
    else
    {
        pt_.decoded[id_host] =
            t.host.name.decoded_size;
    }
    if(t.port.has_port)
    {
        if(t.port.has_number)
            pt_.port_number =
                t.port.port_number;
        else
            pt_.port_number = 0;
    }
    else
    {
        pt_.port_number = 0;
    }
    check_invariants();
    return *this;
}

//------------------------------------------------

url&
url::
remove_origin() noexcept
{
    check_invariants();
    if(len(id_scheme,
        id_path) == 0)
    {
        // no origin
        return *this;
    }

    pt_.decoded[id_user] = 0;
    pt_.decoded[id_pass] = 0;
    pt_.decoded[id_host] = 0;
    pt_.host_type =
        urls::host_type::none;
    pt_.port_number = 0;

    // Check if we will be left with
    // "//" or a rootless segment
    // with a colon
    auto s = get(id_path);
    if(s.starts_with("//"))
    {
        // need "."
        auto dest = resize_impl(
            id_scheme, id_path, 1);
        dest[0] = '.';
        pt_.offset[id_scheme] = 0;
        pt_.offset[id_user] = 0;
        pt_.offset[id_pass] = 0;
        pt_.offset[id_host] = 0;
        pt_.offset[id_port] = 0;
        BOOST_ASSERT(
            pt_.offset[id_path] >= 1);
        pt_.offset[id_path] -= 1;
        return *this;
    }
    if( s.empty() ||
        s.starts_with('/'))
    {
        // path-empty,
        // path-absolute
        resize_impl(
            id_scheme, id_path, 0);
        check_invariants();
        return *this;
    }
    auto const p = url_view::path();
    BOOST_ASSERT(! p.empty());
    auto it = p.begin();
    s = it->encoded_segment();
    if(s.find_first_of(':') ==
        string_view::npos)
    {
        // path-noscheme
        resize_impl(
            id_scheme, id_path, 0);
        check_invariants();
        return *this;
    }

    // need "./"
    auto dest = resize_impl(
        id_scheme, id_path, 2);
    dest[0] = '.';
    dest[1] = '/';
    pt_.offset[id_scheme] = 0;
    pt_.offset[id_user] = 0;
    pt_.offset[id_pass] = 0;
    pt_.offset[id_host] = 0;
    pt_.offset[id_port] = 0;
    BOOST_ASSERT(
        pt_.offset[id_path] >= 2);
    pt_.offset[id_path] -= 2;
    return *this;
}

//------------------------------------------------

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
    auto const n0 =
        pt_.len(first, last);
    if(new_len == 0 && n0 == 0)
        return s_ + pt_.offset[first];
    if(new_len <= n0)
    {
        // shrinking
        std::size_t n = n0 - new_len;
        auto const pos =
            pt_.offset[last];
        // adjust chars
        std::memmove(
            s_ + pos - n,
            s_ + pos,
            pt_.offset[
                id_end] - pos + 1);
        // collapse [first, last)
        for(auto i = first + 1;
                i < last; ++i)
            pt_.offset[i] =
                pt_.offset[last] - n;
        // shift [last, end) left
        for(auto i = last;
                i <= id_end; ++i)
            pt_.offset[i] -= n;
        s_[pt_.offset[id_end]] = '\0';
        return s_ + pt_.offset[first];
    }

    // growing
    std::size_t n = new_len - n0;

    if(n > max_size() - pt_.offset[id_end])
        detail::throw_length_error(
            "url::resize_impl",
            BOOST_CURRENT_LOCATION);

    if(cap_ < pt_.offset[id_end] + n)
    {
        // reallocate
        auto new_cap = growth_impl(
            cap_, pt_.offset[id_end] + n);
        auto s1 = alloc_impl(new_cap);
        if(s_)
        {
            BOOST_ASSERT(cap_ != 0);
            std::memcpy(s1, s_, pt_.offset[id_end] + 1);
            free_impl(s_);
        }
        s_ = s1;
        cs_ = s_;
        cap_ = new_cap;
    }

    auto const pos =
        pt_.offset[last];
        // adjust chars
    std::memmove(
        s_ + pos + n,
        s_ + pos,
        pt_.offset[id_end] -
            pos + 1);
    // collapse [first, last)
    for(auto i = first + 1;
        i < last; ++i)
        pt_.offset[i] =
            pt_.offset[last] + n;
    // shift [last, end) right
    for(auto i = last;
        i <= id_end; ++i)
        pt_.offset[i] += n;
    s_[pt_.offset[id_end]] = '\0';
    return s_ + pt_.offset[first];
}

//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    url const& u)
{
    auto const s = u.str();
    os.write(s.data(), s.size());
    return os;
}

} // urls
} // boost

#endif

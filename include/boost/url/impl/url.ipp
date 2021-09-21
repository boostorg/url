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
#include <boost/url/detail/copied_strings.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/detail/print.hpp>
#include <boost/url/rfc/authority_bnf.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/host_bnf.hpp>
#include <boost/url/rfc/paths_bnf.hpp>
#include <boost/url/rfc/port_bnf.hpp>
#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/rfc/scheme_bnf.hpp>
#include <boost/url/rfc/userinfo_bnf.hpp>
#include <boost/align/align_up.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace boost {
namespace urls {

// construct from static storage
url::
url(char* buf,
    std::size_t cap) noexcept
    : s_(buf)
    , cap_(cap)
{
    using alignment::align_up;
    BOOST_ASSERT(cap > 0);
    BOOST_ASSERT(align_up(cap_,
        alignof(pos_t)) == cap_);
    s_[0] = '\0';
    cs_ = s_;
}

// make a copy of u
void
url::
copy(url_view const& u)
{
    if(u.size() == 0)
    {
        clear();
        return;
    }
    ensure_space(
        u.size(),
        u.nseg_,
        u.nparam_);
    base() = u;
    cs_ = s_;
    std::memcpy(s_,
        u.data(), u.size());
    s_[size()] = '\0';
}

// allocate n aligned up
char*
url::
allocate(std::size_t n)
{
    using alignment::align_up;
    n = align_up(
        n, alignof(pos_t));
    auto s = new char[n];
    cap_ = n;
    return s;
}

void
url::
deallocate(char* s)
{
    delete[] s;
}

//------------------------------------------------

url::
~url()
{
    if(s_)
    {
        BOOST_ASSERT(
            cap_ != 0);
        deallocate(s_);
    }
}

url::
url() noexcept = default;

url::
url(url&& u) noexcept
{
    base() = u;
    cap_ = u.cap_;
    s_ = u.s_;
    u = url();
}

url::
url(url_view const& u)
{
    copy(u);
}

url&
url::
operator=(url&& u) noexcept
{
    if(s_)
        deallocate(s_);
    base() = u;
    cap_ = u.cap_;
    s_ = u.s_;
    u.base() = {};
    u.cap_ = 0;
    u.s_ = nullptr;
    return *this;
}

url&
url::
operator=(url_view const& u)
{
    copy(u);
    return *this;
}

//------------------------------------------------

void
url::
clear() noexcept
{
    if(s_)
    {
        base() = {};
        s_[0] = '\0';
        cs_ = s_;
    }
    else
    {
        BOOST_ASSERT(
            cs_ == empty_);
    }
}

//------------------------------------------------
//
// Scheme
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
    scheme_ = id;
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
        auto const p =
            url_view::encoded_segments();
        BOOST_ASSERT(! p.empty());
        auto it = p.begin();
        if(it->find_first_of(':') ==
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
        scheme_ = urls::scheme::none;
        check_invariants();
        return *this;
    }

    // remove the scheme but add "./"
    // to the beginning of the path
    BOOST_ASSERT(n >= 2);
    ensure_space(
        size(),
        nseg_ + 1,
        nparam_);
    // move chars
    std::memmove(
        s_, s_ + n,
        offset(id_path) - n);
    std::memmove(
        s_ + offset(
            id_path) - (n - 2),
        s_ + offset(id_path),
        offset(id_end) -
            offset(id_path));
    // adjust table
    ++nseg_;
    adjust(id_user, id_path, 0-n);
    adjust(
        id_query, id_end, 0-(n - 2));
    auto dest = s_ +
        offset(id_path);
    dest[0] = '.';
    dest[1] = '/';
    dest[offset(id_end)] = '\0';
    scheme_ = urls::scheme::none;
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
    split(id_user, 2 + n);
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
    decoded_[id_user] = 0;
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
    decoded_[id_user] = s.size();
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
    decoded_[id_user] = n;
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
    split(id_user, 2);
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
    decoded_[id_pass] = 0;
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
    decoded_[id_pass] = s.size();
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
    decoded_[id_pass] = n;
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
    split(id_user, n + 2);
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
    decoded_[id_user] = 0;
    decoded_[id_pass] = 0;
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
    decoded_[id_user] = s.size();
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
    split(id_user, 2 + t.user.str.size());
    if(! s.empty())
        std::memcpy(dest, s.data(), s.size());
    decoded_[id_user] =
        t.user.decoded_size;
    if(t.has_password)
        decoded_[id_pass] =
            t.password.decoded_size;
    else
        decoded_[id_pass] = 0;
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
        split(id_user, 2);
        split(id_pass, 0);
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
    decoded_[id_host] = len(id_host);
    host_type_ =
        urls::host_type::ipv4;
    auto bytes = addr.to_bytes();
    std::memcpy(ip_addr_,
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
    decoded_[id_host] = len(id_host);
    host_type_ =
        urls::host_type::ipv6;
    auto bytes = addr.to_bytes();
    std::memcpy(ip_addr_,
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
    decoded_[id_host] = s.size();
    host_type_ =
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
    decoded_[id_host] = n;
    host_type_ = t.host_type;
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
    split(id_user, 2);
    split(id_pass, 0);
    split(id_host, 0);
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
    port_number_ = 0;
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
    port_number_ = n;
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
        port_number_ = t.number;
    else
        port_number_ = 0;
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
    host_type_ =
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
        split(id_user,
            2 + t0.user.str.size());
        n -= t0.user.str.size();
        decoded_[id_user] =
            t0.user.decoded_size;
        if(t0.has_password)
        {
            split(id_pass, 2 +
                t0.password.str.size());
            decoded_[id_pass] =
                t0.password.decoded_size;
            n -= 1 + t0.password.str.size();
        }
        else
        {
            split(id_pass, 1);
            decoded_[id_pass] = 0;
            n -= 1;
        }
    }
    else
    {
        split(id_user, 2);
        split(id_pass, 0);
    }
    split(id_host,
        t.host.host_part.size());
    n -= t.host.host_part.size();
    host_type_ = t.host.host_type;
    if(host_type_ ==
        urls::host_type::ipv4)
    {
        auto const bytes =
            t.host.ipv4.to_bytes();
        std::memcpy(ip_addr_,
            bytes.data(), bytes.size());
        decoded_[id_host] =
            len(id_host);
    }
    else if(host_type_ ==
        urls::host_type::ipv6)
    {
        auto const bytes =
            t.host.ipv6.to_bytes();
        std::memcpy(ip_addr_,
            bytes.data(), bytes.size());
        decoded_[id_host] =
            len(id_host);
    }
    else if(host_type_ ==
        urls::host_type::ipvfuture)
    {
        decoded_[id_host] =
            len(id_host);
    }
    else
    {
        decoded_[id_host] =
            t.host.name.decoded_size;
    }
    if(t.port.has_port)
    {
        if(t.port.has_number)
            port_number_ =
                t.port.port_number;
        else
            port_number_ = 0;
    }
    else
    {
        port_number_ = 0;
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

    decoded_[id_user] = 0;
    decoded_[id_pass] = 0;
    decoded_[id_host] = 0;
    host_type_ =
        urls::host_type::none;
    port_number_ = 0;

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
        split(id_scheme, 0);
        split(id_user, 0);
        split(id_pass, 0);
        split(id_host, 0);
        split(id_port, 0);
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
    auto const p =
        url_view::encoded_segments();
    BOOST_ASSERT(! p.empty());
    auto it = p.begin();
    if(it->find_first_of(':') ==
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
    split(id_scheme, 0);
    split(id_user, 0);
    split(id_pass, 0);
    split(id_host, 0);
    split(id_port, 0);
    return *this;
}

//------------------------------------------------
//
// Path
//
//------------------------------------------------

auto
url::
get_segment(
    std::size_t i) const noexcept ->
        raw_segment
{
    if(segment_count() == 0)
        return { offset(id_path), 0 };
    if(i == segment_count())
        return { offset(id_query), 0 };
    BOOST_ASSERT(
        i <= segment_count());
#if 0
    std::size_t i;
    if(index < 0)
        i = nseg + index;
    else
        i = static_cast<
            std::size_t>(index);
    if(i >= nseg)
        return empty_;
    string_view s(
        s_ + segment_pos(i),
        segment_len(i));
    if(i > 0)
    {
        BOOST_ASSERT(
            s.starts_with('/'));
        s.remove_prefix(1);
        return s;
    }
    if(s.starts_with('/'))
        s.remove_prefix(1);
    return s;
#else
    auto n = len(id_path);
    if(segment_count() < 2)
        return { offset(id_path), n };
    auto it = s_ + offset(id_path);
    auto start = it;
    auto const last =
        s_ + offset(id_query);
    BOOST_ASSERT(n > 0);
    for(;;)
    {
        for(;;)
        {
            ++it;
            if(*it == '/')
                break;
            if(it == last)
                break;
        }
        if(i == 0)
            break;
        start = it;
        --i;
    }
    return {
        static_cast<std::size_t>(
            start - s_),
        static_cast<std::size_t>(
            it - start ) };
#endif
}

char*
url::
resize(
    raw_segment const& r,
    std::size_t n)
{
    if(n < r.len)
    {
        // shrinking
        std::memmove(
            s_ + r.pos + n,
            s_ + r.pos + r.len,
            size() - (r.pos + r.len));
        resize_impl(
            id_path,
            len(id_path) - (
                r.len - n));
        return s_ + r.pos;
    }
    // growing
    auto n0 = offset(id_query) -
        (r.pos + r.len);
    resize_impl(
        id_path,
        len(id_path) + (
            n - r.len));
    std::memmove(
        s_ + r.pos + n,
        s_ + r.pos + r.len,
        n0);
    return s_ + r.pos;
}

// insert before r
char*
url::
insert(
    raw_segment const& r,
    std::size_t n)
{
    // growing
    auto n0 = offset(
        id_query) - r.pos;
    resize_impl(
        id_path,
        len(id_path) + n);
    std::memmove(
        s_ + r.pos + n,
        s_ + r.pos,
        n0);
    return s_ + r.pos;
}

void
url::
set_encoded_segment(
    std::size_t i,
    string_view s0)
{
    // validate
    error_code ec;
    pct_decode_size(s0, ec, pchars);
    if(ec)
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);

    detail::copied_strings cs(
        encoded_url());
    auto s = cs.maybe_copy(s0);
    auto r = get_segment(i);
    auto n = s.size();
    if( r.len == 0 ||
        s_[r.pos] == '/')
    {
        auto p =
            resize(r, n + 1);
        if(! s.empty())
            std::memcpy(p + 1,
                s.data(), s.size());
        p[0] = '/';
        return;
    }
    auto p = resize(r, n);
    if(! s.empty())
        std::memcpy(p,
            s.data(), s.size());
}

void
url::
insert_encoded_segment(
    std::size_t i,
    string_view s0)
{
    // validate
    error_code ec;
    pct_decode_size(s0, ec, pchars);
    if(ec)
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);

    detail::copied_strings cs(
        encoded_url());
    auto s = cs.maybe_copy(s0);
    auto r = get_segment(i);
    auto n = s.size();
    if( r.len == 0 ||
        s_[r.pos] == '/')
    {
        auto p = insert(r, n + 1);
        if(! s.empty())
            std::memcpy(p + 1,
                s.data(), s.size());
        p[0] = '/';
        ++nseg_;
        return;
    }
    auto p = insert(r, n);
    if(! s.empty())
        std::memcpy(p,
            s.data(), s.size());
    ++nseg_;
}

void
url::
erase_segments(
    std::size_t first,
    std::size_t last)
{
    if(segment_count() == 0)
        return;
    if(first == last)
        return;
    BOOST_ASSERT(last > first);
    auto r0 = get_segment(first);
    auto r1 = get_segment(last);
    // shrinking
    std::memmove(
        s_ + r0.pos,
        s_ + r1.pos,
        size() - r1.pos);
    resize_impl(
        id_path,
        len(id_path) - (
            r1.pos - r0.pos));
    nseg_ -= last - first;
}

//------------------------------------------------

string_view
url::
encoded_segment(
    int index) const noexcept
{
    std::size_t i;
    raw_segment r;
    if(index >= 0)
    {
        i = static_cast<
            std::size_t>(index);
        if(i >= nseg_)
            return empty_;
        r = get_segment(i);
    }
    else
    {
        i = static_cast<
            std::size_t>(-index);
        if(i > nseg_)
            return empty_;
        r = get_segment(nseg_ - i);
    }
    string_view s = {
        s_ + r.pos, r.len };
    if(s.starts_with('/'))
        s.remove_prefix(1);
    return s;
}

segments_encoded
url::
encoded_segments() noexcept
{
    return segments_encoded(*this);
}

//------------------------------------------------
//
// implementation
//
//------------------------------------------------

void
url::
check_invariants() const noexcept
{
    using alignment::align_up;
    BOOST_ASSERT(align_up(cap_,
        alignof(pos_t)) == cap_);
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
    BOOST_ASSERT(c_str()[size()] == '\0');
    // validate segments
#if 0
    if(nseg > 0)
    {
        auto it = cs_ +
            offset(id_path);
        auto const end = s_ +
            offset(id_query);
        error_code ec;
        pct_encoded_str t;
        auto start = it;
        if(get(id_path).starts_with('/'))
            path_abempty_bnf::begin(
                it, end, ec, t);
        else
            path_rootless_bnf::begin(
                it, end, ec, t);
        for(std::size_t i = 0;;++i)
        {
            if(ec == error::end)
                break;
            BOOST_ASSERT(! ec.failed());
            if(ec.failed())
                break;
            BOOST_ASSERT(
                cs_ + segment_pos(i) == start);
            BOOST_ASSERT(
                start + segment_len(i) == it);
            start = it;
            path_abempty_bnf::increment(
                it, end, ec, t);
        }
    }
#endif
}

void
url::
build_tab() noexcept
{
#if 0
    // path
    if(nseg_ > 1)
    {
        error_code ec;
        // path table
        pos_t* tab = tab_end() - 1;
        auto s = get(id_path);
        auto it = s.data();
        auto const end = it + s.size();
        pct_encoded_str t;
        if( s.starts_with('/') ||
            s.empty())
            path_abempty_bnf::begin(
                it, end, ec, t);
        else
            path_rootless_bnf::begin(
                it, end, ec, t);
        for(;;)
        {
            if(ec == error::end)
                break;
            if(ec)
                detail::throw_system_error(ec,
                    BOOST_CURRENT_LOCATION);
            *tab = it - s_;
            tab -= 2;
            path_abempty_bnf::increment(
                it, end, ec, t);
        }
    }
    // query
    if(nparam_ > 1)
    {
        error_code ec;
        // query table
        pos_t* tab = tab_end() - 2;
        auto s = get(id_query);
        auto it = s.data();
        auto const end = it + s.size();
        query_param t;
        query_bnf::begin(
            it, end, ec, t);
        for(;;)
        {
            if(ec == error::end)
                break;
            if(ec)
                detail::throw_system_error(ec,
                    BOOST_CURRENT_LOCATION);
            *tab = it - s_;
            tab -= 2;
            query_bnf::increment(
                it, end, ec, t);
        }
    }
#endif
}

void
url::
ensure_space(
    std::size_t nchar, // excluding null
    std::size_t nseg,
    std::size_t nparam)
{
    // minimum size
    if( nchar < 15)
        nchar = 15;
    if(nchar > max_size() - 1)
        detail::throw_length_error(
            "nchar > max_size",
            BOOST_CURRENT_LOCATION);
    std::size_t new_cap = nchar + 1;
    if(nseg > nparam)
        new_cap += 2 * sizeof(pos_t) *
            (nseg + 1);
    else
        new_cap += 2 * sizeof(pos_t) *
            (nparam + 1);
    if(new_cap <= cap_)
        return;
    char* s;
    if(s_ != nullptr)
    {
        // 50% growth policy
        auto n = cap_ + (cap_ / 2);
        if(n < cap_)
        {
            // overflow
            n = std::size_t(-1) &
                ~(alignof(pos_t)-1);
        }
        if( new_cap < n)
            new_cap = n;
        s = allocate(new_cap);
        std::memcpy(s, s_, size());
        deallocate(s_);
    }
    else
    {
        s = allocate(new_cap);
    }
    s_ = s;
    cs_ = s;
}

char*
url::
resize_impl(
    int id,
    std::size_t new_size)
{
    return resize_impl(
        id, id + 1, new_size);
}

char*
url::
resize_impl(
    int first,
    int last,
    std::size_t new_len)
{
    auto const n0 = len(first, last);
    if(new_len == 0 && n0 == 0)
        return s_ + offset(first);
    if(new_len <= n0)
    {
        // shrinking
        std::size_t n = n0 - new_len;
        auto const pos =
            offset(last);
        // adjust chars
        std::memmove(
            s_ + pos - n,
            s_ + pos,
            offset(
                id_end) - pos + 1);
        // collapse (first, last)
        collapse(first,  last, 
            offset(last) - n);
        // shift (last, end) left
        adjust(
            last, id_end, 0 - n);
#if 0
        // update table
        if( nseg > 1 &&
            first <= id_path)
        {
            // adjust segments
            auto const tab =
                tab_end() - 1;
            for(std::size_t i = 0;
                i < nseg - 1; ++i)
                tab[0-2*i] += 0 - n;
        }
        if( nparam > 1 &&
            first <= id_query)
        {
            // adjust params
            auto const tab =
                tab_end() - 2;
            for(std::size_t i = 0;
                i < nparam - 1; ++i)
                tab[0-2*i] += 0 - n;
        }
#endif
        s_[size()] = '\0';
        return s_ + offset(first);
    }

    // growing
    std::size_t n = new_len - n0;
    ensure_space(size() + n,
        nseg_, nparam_);
    auto const pos =
        offset(last);
    // adjust chars
    std::memmove(
        s_ + pos + n,
        s_ + pos,
        offset(id_end) -
            pos + 1);
    // collapse (first, last)
    collapse(first, last,
        offset(last) + n);
    // shift (last, end) right
    adjust(last, id_end, n);
#if 0
    // update table
    if( nseg > 1 &&
        last > id_path &&
        first < id_path)
    {
        // adjust segments
        auto const tab =
            tab_end() - 1;
        for(std::size_t i = 0;
            i < nseg - 1; ++i)
            tab[0-2*i] += n;
    }
    if( nparam > 1 &&
        last > id_query &&
        first < id_query)
    {
        // adjust params
        auto const tab =
            tab_end() - 2;
        for(std::size_t i = 0;
            i < nparam - 1; ++i)
            tab[0-2*i] += n;
    }
#endif
    s_[size()] = '\0';
    return s_ + offset(first);
}

//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    url const& u)
{
    auto const s = u.encoded_url();
    os.write(s.data(), s.size());
    return os;
}

} // urls
} // boost

#endif

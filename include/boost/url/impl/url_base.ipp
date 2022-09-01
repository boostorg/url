//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_BASE_IPP
#define BOOST_URL_IMPL_URL_BASE_IPP

#include <boost/url/url_base.hpp>
#include <boost/url/error.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/detail/decode.hpp>
#include <boost/url/detail/encode.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/move_chars.hpp>
#include <boost/url/detail/print.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/detail/charsets.hpp>
#include <boost/url/rfc/detail/fragment_rule.hpp>
#include <boost/url/rfc/detail/host_rule.hpp>
#include <boost/url/rfc/detail/ipvfuture_rule.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/url/rfc/detail/port_rule.hpp>
#include <boost/url/rfc/detail/scheme_rule.hpp>
#include <boost/url/rfc/detail/userinfo_rule.hpp>
#include <boost/url/grammar/detail/copied_strings.hpp>
#include <boost/url/grammar/parse.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace boost {
namespace urls {

//------------------------------------------------

// these objects help handle the cases
// where the user passes in strings that
// come from inside the url buffer.

url_base::
op_t::
~op_t()
{
    if(old)
        u.cleanup(*this);
    u.check_invariants();
}

url_base::
op_t::
op_t(
    url_base& u_) noexcept
    : u(u_)
    , s(s_)
{
    u.check_invariants();
}

url_base::
op_t::
op_t(
    url_base& u_,
    string_view& s) noexcept
    : u(u_)
    , s(s)
{
    u.check_invariants();
}

void
url_base::
op_t::
move(char* dest, char const* src,
    std::size_t n) noexcept
{
    detail::move_chars(
        dest, src, n, s);
}

//------------------------------------------------

// construct reference
url_base::
url_base(
    detail::url_impl const& impl) noexcept
    : url_view_base(impl)
{
}

void
url_base::
reserve_impl(std::size_t n)
{
    op_t op(*this);
    reserve_impl(n, op);
    s_[size()] = '\0';
}

// make a copy of u
void
url_base::
copy(url_view_base const& u)
{
    op_t op(*this);
    if(u.size() == 0)
    {
        clear();
        return;
    }
    reserve_impl(
        u.size(), op);
    u_ = u.u_;
    u_.cs_ = s_;
    std::memcpy(s_,
        u.data(), u.size());
    s_[size()] = '\0';
}

//------------------------------------------------
//
// Scheme
//
//------------------------------------------------

url_base&
url_base::
remove_scheme() noexcept
{
    op_t op(*this);
    auto const n = u_.len(id_scheme);
    if(n == 0)
        return *this;
    auto const p = u_.offset(id_path);
    // Check if we are changing
    // path-rootless to path-noscheme
    bool const need_dot =
        [this, p]
    {
        if(has_authority())
            return false;
        if(u_.nseg_ == 0)
            return false;
        BOOST_ASSERT(u_.len(id_path) > 0);
        if(s_[p] == '/')
            return false;
        string_view const s(
            s_ + p, segment(1) - p);
        if(s.find_first_of(':') ==
            string_view::npos)
            return false;
        return true;
    }();
    if(! need_dot)
    {
        // just remove the scheme
        resize_impl(id_scheme, 0, op);
        u_.scheme_ = urls::scheme::none;
        check_invariants();
        return *this;
    }
    // remove the scheme but add "./"
    // to the beginning of the path
    BOOST_ASSERT(n >= 2);
    // move [id_scheme, id_path) left
    op.move(
        s_,
        s_ + n,
        p - n);
    // move [id_path, id_end) left
    op.move(
        s_ + p - (n - 2),
        s_ + p,
        u_.offset(id_end) - p);
    // adjust part offsets.
    // (p is invalidated)
    u_.adjust(id_user, id_path, 0-n);
    u_.adjust(id_query, id_end, 0-(n - 2));
    auto dest = s_ + u_.offset(id_path);
    dest[0] = '.';
    dest[1] = '/';
    s_[size()] = '\0';
    u_.scheme_ = urls::scheme::none;
    return *this;
}

url_base&
url_base::
set_scheme(string_view s)
{
    set_scheme_impl(
        s, string_to_scheme(s));
    return *this;
}

url_base&
url_base::
set_scheme(urls::scheme id)
{
    if(id == urls::scheme::unknown)
        detail::throw_invalid_argument();
    if(id == urls::scheme::none)
        return remove_scheme();
    set_scheme_impl(to_string(id), id);
    return *this;
}

//------------------------------------------------
//
// Authority
//
//------------------------------------------------

url_base&
url_base::
remove_authority() noexcept
{
    if(! has_authority())
        return *this;
    op_t op(*this);
    if(u_.get(id_path
        ).starts_with("//"))
    {
        // prepend "/."
        auto p = resize_impl(
            id_user, id_path, 2, op);
        p[0] = '/';
        p[1] = '.';
        u_.split(id_user, 0);
        u_.split(id_pass, 0);
        u_.split(id_host, 0);
        u_.split(id_port, 0);
    }
    else
    {
        resize_impl(
            id_user, id_path, 0, op);
    }
    u_.host_type_ =
        urls::host_type::none;
    return *this;
}

url_base&
url_base::
set_encoded_authority(
    pct_string_view s)
{
    op_t op(*this, detail::ref(s));
    // [ user [ ':' pass ] "@" ] host [ ":" port ]
#if 0
    auto pos1 = s.find_first_of('@');
    auto pos2 = s.find_last_of(':');
#endif
    auto t = grammar::parse(
        s, authority_rule).value();
    auto n = s.size() + 2;
    auto const need_slash =
        ! is_path_absolute() &&
        u_.len(id_path) > 0;
    if(need_slash)
        ++n;
    auto dest = resize_impl(
        id_user, id_path, n, op);
    dest[0] = '/';
    dest[1] = '/';
    std::memcpy(dest + 2,
        s.data(), s.size());
    if(need_slash)
        dest[n - 1] = '/';

    u_.apply_authority(t);
    if(need_slash)
        u_.adjust(
            id_query, id_end, 1);
    return *this;
}

//------------------------------------------------
//
// Userinfo
//
//------------------------------------------------

url_base&
url_base::
remove_userinfo() noexcept
{
    if(u_.len(id_pass) == 0)
        return *this; // no userinfo

    op_t op(*this);
    // keep authority '//'
    resize_impl(
        id_user, id_host, 2, op);
    u_.decoded_[id_user] = 0;
    u_.decoded_[id_pass] = 0;
    return *this;
}

url_base&
url_base::
set_userinfo(
    string_view s)
{
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, detail::userinfo_chars);
    auto dest = set_userinfo_impl(n, op);
    encode(
        dest,
        dest + n,
        s,
        opt,
        detail::userinfo_chars);
    auto const pos = u_.get(
        id_user, id_host
            ).find_first_of(':');
    if(pos != string_view::npos)
    {
        u_.split(id_user, pos);
        // find ':' in plain string
        auto const pos2 =
            s.find_first_of(':');
        u_.decoded_[id_user] =
            pos2 - 1;
        u_.decoded_[id_pass] =
            s.size() - pos2;
    }
    else
    {
        u_.decoded_[id_user] = s.size();
        u_.decoded_[id_pass] = 0;
    }
    return *this;
}

url_base&
url_base::
set_encoded_userinfo(
    pct_string_view s)
{
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const pos = s.find_first_of(':');
    if(pos != string_view::npos)
    {
        // user:pass
        auto const s0 = s.substr(0, pos);
        auto const s1 = s.substr(pos + 1);
        auto const n0 =
            detail::re_encoded_size(s0, opt,
                detail::user_chars);
        auto const n1 =
            detail::re_encoded_size(s1, opt,
                detail::password_chars);
        auto dest =
            set_userinfo_impl(n0 + n1 + 1, op);
        detail::re_encode_unchecked(
            dest,
            dest + n0,
            s0,
            opt,
            detail::user_chars);
        dest[n0] = ':';
        detail::re_encode_unchecked(
            dest + n0 + 1,
            dest + n0 + 1 + n1,
            s1,
            opt,
            detail::password_chars);
        u_.split(id_user, 2 + n0);
        u_.decoded_[id_user] =
            detail::decode_bytes_unchecked(s0);
        u_.decoded_[id_pass] =
            s.decoded_size() -
                u_.decoded_[id_user];
    }
    else
    {
        // user
        auto const n =
            detail::re_encoded_size(
                s, opt, detail::user_chars);
        auto dest = set_userinfo_impl(n, op);
        detail::re_encode_unchecked(
            dest,
            dest + n,
            s,
            opt,
            detail::user_chars);
        u_.split(id_user, 2 + n);
        u_.decoded_[id_user] =
            s.decoded_size();
        u_.decoded_[id_pass] = 0;
    }
    return *this;
}

//------------------------------------------------

url_base&
url_base::
set_user(string_view s)
{
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, detail::user_chars);
    auto dest = set_user_impl(n, op);
    detail::encode_unchecked(
        dest,
        u_.get(id_pass).data(),
        s,
        opt,
        detail::user_chars);
    u_.decoded_[id_user] = s.size();
    return *this;
}

url_base&
url_base::
set_encoded_user(
    pct_string_view s)
{
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n =
        detail::re_encoded_size(
            s, opt, detail::user_chars);
    auto dest = set_user_impl(n, op);
    detail::re_encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::user_chars);
    u_.decoded_[id_user] =
        s.decoded_size();
    return *this;
}

//------------------------------------------------

url_base&
url_base::
remove_password() noexcept
{
    auto const n = u_.len(id_pass);
    if(n < 2)
        return *this; // no password

    op_t op(*this);
    // clear password, retain '@'
    auto dest =
        resize_impl(id_pass, 1, op);
    dest[0] = '@';
    u_.decoded_[id_pass] = 0;
    return *this;
}

url_base&
url_base::
set_password(string_view s)
{
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, detail::password_chars);
    auto dest = set_password_impl(n, op);
    detail::encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::password_chars);
    u_.decoded_[id_pass] = s.size();
    return *this;
}

url_base&
url_base::
set_encoded_password(
    pct_string_view s)
{
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n =
        detail::re_encoded_size(s, opt,
            detail::password_chars);
    auto dest = set_password_impl(n, op);
    detail::re_encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::password_chars);
    u_.decoded_[id_pass] =
        s.decoded_size();
    return *this;
}

//------------------------------------------------
//
// Host
//
//------------------------------------------------
/*
host_type       host_type()                 // ipv4, ipv6, ipvfuture, name

std::string     host()                      // return encoded_host().decode_to_string()
pct_string_view encoded_host()              // return host part, as-is
std::string     host_address()              // return encoded_host_address().decode_to_string()
pct_string_view encoded_host_address()      // ipv4, ipv6, ipvfut, or encoded name, no brackets

ipv4_address    host_ipv4_address()         // return ipv4_address or {}
ipv6_address    host_ipv6_address()         // return ipv6_address or {}
string_view     host_ipvfuture()            // return ipvfuture or {}
std::string     host_name()                 // return decoded name or ""
pct_string_view encoded_host_name()         // return encoded host name or ""

--------------------------------------------------

set_host( string_view )                     // set host part from plain text
set_encoded_host( pct_string_view )         // set host part from encoded text
set_host_address( string_view )             // set host from ipv4, ipv6, ipvfut, or plain reg-name string
set_encoded_host_address( pct_string_view ) // set host from ipv4, ipv6, ipvfut, or encoded reg-name string

set_host_ipv4_address( ipv4_address )       // set ipv4
set_host_ipv6_address( ipv6_address )       // set ipv6
set_host_ipvfuture( string_view )           // set ipvfuture
set_host_name( string_view )                // set name from plain
set_encoded_host_name( pct_string_view )    // set name from encoded
*/

// set host part from plain text
url_base&
url_base::
set_host(
    string_view s)
{
    if( s.size() > 2 &&
        s.front() == '[' &&
        s.back() == ']')
    {
        // IP-literal
        {
            // IPv6-address
            auto rv = parse_ipv6_address(
                s.substr(1, s.size() - 2));
            if(rv)
                return set_host_ipv6_address(*rv);
        }
        {
            // IPvFuture
            auto rv = grammar::parse(
                s.substr(1, s.size() - 2),
                    detail::ipvfuture_rule);
            if(rv)
                return set_host_ipvfuture(rv->str);
        }
    }
    else if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        auto rv = parse_ipv4_address(s);
        if(rv)
            return set_host_ipv4_address(*rv);
    }

    // reg-name
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, detail::host_chars);
    auto dest = set_host_impl(n, op);
    encode(
        dest,
        u_.get(id_path).data(),
        s,
        opt,
        detail::host_chars);
    u_.decoded_[id_host] = s.size();
    u_.host_type_ =
        urls::host_type::name;
    return *this;
}

// set host part from encoded text
url_base&
url_base::
set_encoded_host(
    pct_string_view s)
{
    if( s.size() > 2 &&
        s.front() == '[' &&
        s.back() == ']')
    {
        // IP-literal
        {
            // IPv6-address
            auto rv = parse_ipv6_address(
                s.substr(1, s.size() - 2));
            if(rv)
                return set_host_ipv6_address(*rv);
        }
        {
            // IPvFuture
            auto rv = grammar::parse(
                s.substr(1, s.size() - 2),
                    detail::ipvfuture_rule);
            if(rv)
                return set_host_ipvfuture(rv->str);
        }
    }
    else if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        auto rv = parse_ipv4_address(s);
        if(rv)
            return set_host_ipv4_address(*rv);
    }

    // reg-name
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n = detail::re_encoded_size(
        s, opt, detail::host_chars);
    auto dest = set_host_impl(n, op);
    detail::re_encode_unchecked(
        dest,
        u_.get(id_path).data(),
        s,
        opt,
        detail::host_chars);
    u_.decoded_[id_host] = s.decoded_size();
    u_.host_type_ =
        urls::host_type::name;
    return *this;
}

url_base&
url_base::
set_host_address(
    string_view s)
{
    {
        // IPv6-address
        auto rv = parse_ipv6_address(s);
        if(rv)
            return set_host_ipv6_address(*rv);
    }
    {
        // IPvFuture
        auto rv = grammar::parse(
            s, detail::ipvfuture_rule);
        if(rv)
            return set_host_ipvfuture(rv->str);
    }
    if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        auto rv = parse_ipv4_address(s);
        if(rv)
            return set_host_ipv4_address(*rv);
    }

    // reg-name
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, detail::host_chars);
    auto dest = set_host_impl(n, op);
    encode(
        dest,
        u_.get(id_path).data(),
        s,
        opt,
        detail::host_chars);
    u_.decoded_[id_host] = s.size();
    u_.host_type_ =
        urls::host_type::name;
    return *this;
}

url_base&
url_base::
set_encoded_host_address(
    pct_string_view s)
{
    {
        // IPv6-address
        auto rv = parse_ipv6_address(s);
        if(rv)
            return set_host_ipv6_address(*rv);
    }
    {
        // IPvFuture
        auto rv = grammar::parse(
            s, detail::ipvfuture_rule);
        if(rv)
            return set_host_ipvfuture(rv->str);
    }
    if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        auto rv = parse_ipv4_address(s);
        if(rv)
            return set_host_ipv4_address(*rv);
    }

    // reg-name
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n = detail::re_encoded_size(
        s, opt, detail::host_chars);
    auto dest = set_host_impl(n, op);
    detail::re_encode_unchecked(
        dest,
        u_.get(id_path).data(),
        s,
        opt,
        detail::host_chars);
    u_.decoded_[id_host] = s.decoded_size();
    u_.host_type_ =
        urls::host_type::name;
    return *this;
}

url_base&
url_base::
set_host_ipv4_address(
    ipv4_address const& addr)
{
    op_t op(*this);
    char buf[urls::ipv4_address::max_str_len];
    auto s = addr.to_buffer(buf, sizeof(buf));
    auto dest = set_host_impl(s.size(), op);
    std::memcpy(dest, s.data(), s.size());
    u_.decoded_[id_host] = u_.len(id_host);
    u_.host_type_ = urls::host_type::ipv4;
    auto bytes = addr.to_bytes();
    std::memcpy(
        u_.ip_addr_,
        bytes.data(),
        bytes.size());
    return *this;
}

url_base&
url_base::
set_host_ipv6_address(
    ipv6_address const& addr)
{
    op_t op(*this);
    char buf[2 +
        urls::ipv6_address::max_str_len];
    auto s = addr.to_buffer(
        buf + 1, sizeof(buf) - 2);
    buf[0] = '[';
    buf[s.size() + 1] = ']';
    auto const n = s.size() + 2;
    auto dest = set_host_impl(n, op);
    std::memcpy(dest, buf, n);
    u_.decoded_[id_host] = n;
    u_.host_type_ = urls::host_type::ipv6;
    auto bytes = addr.to_bytes();
    std::memcpy(
        u_.ip_addr_,
        bytes.data(),
        bytes.size());
    return *this;
}

url_base&
url_base::
set_host_ipvfuture(
    string_view s)
{
    op_t op(*this, s);
    // validate
    grammar::parse(s,
        detail::ipvfuture_rule).value();
    auto dest = set_host_impl(
        s.size() + 2, op);
    *dest++ = '[';
    dest += s.copy(dest, s.size());
    *dest = ']';
    u_.host_type_ =
        urls::host_type::ipvfuture;
    u_.decoded_[id_host] = s.size() + 2;
    return *this;
}

url_base&
url_base::
set_host_name(
    string_view s)
{
    bool is_ipv4 = false;
    if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        if(parse_ipv4_address(s).has_value())
            is_ipv4 = true;
    }
    auto allowed = detail::host_chars;
    if(is_ipv4)
        allowed = allowed - '.';

    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(
        s, opt, allowed);
    auto dest = set_host_impl(n, op);
    detail::encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        allowed);
    u_.host_type_ =
        urls::host_type::name;
    u_.decoded_[id_host] = s.size();
    return *this;
}

url_base&
url_base::
set_encoded_host_name(
    pct_string_view s)
{
    bool is_ipv4 = false;
    if(s.size() >= 7) // "0.0.0.0"
    {
        // IPv4-address
        if(parse_ipv4_address(s).has_value())
            is_ipv4 = true;
    }
    auto allowed = detail::host_chars;
    if(is_ipv4)
        allowed = allowed - '.';

    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n = detail::re_encoded_size(
        s, opt, allowed);
    auto dest = set_host_impl(n, op);
    detail::re_encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        allowed);
    u_.host_type_ =
        urls::host_type::name;
    u_.decoded_[id_host] = s.decoded_size();
    return *this;
}

//------------------------------------------------

url_base&
url_base::
remove_port() noexcept
{
    op_t op(*this);
    resize_impl(id_port, 0, op);
    u_.port_number_ = 0;
    return *this;
}

url_base&
url_base::
set_port(
    std::uint16_t n)
{
    op_t op(*this);
    auto s =
        detail::make_printed(n);
    auto dest = set_port_impl(
        s.string().size(), op);
    std::memcpy(
        dest, s.string().data(),
            s.string().size());
    u_.port_number_ = n;
    return *this;
}

url_base&
url_base::
set_port(
    string_view s)
{
    op_t op(*this, s);
    auto t = grammar::parse(
        s, detail::port_rule{}).value();
    auto dest =
        set_port_impl(t.str.size(), op);
    std::memcpy(dest,
        t.str.data(), t.str.size());
    if(t.has_number)
        u_.port_number_ = t.number;
    else
        u_.port_number_ = 0;
    return *this;
}

//------------------------------------------------

url_base&
url_base::
remove_origin() noexcept
{
    op_t op(*this);
    check_invariants();
    if(u_.len(id_scheme,
        id_path) == 0)
    {
        // no origin
        return *this;
    }

    u_.decoded_[id_user] = 0;
    u_.decoded_[id_pass] = 0;
    u_.decoded_[id_host] = 0;
    u_.host_type_ =
        urls::host_type::none;
    u_.port_number_ = 0;

    // Check if we will be left with
    // "//" or a rootless segment
    // with a colon
    auto s = u_.get(id_path);
    if(s.starts_with("//"))
    {
        // need "."
        auto dest = resize_impl(
            id_scheme, id_path, 1, op);
        dest[0] = '.';
        u_.split(id_scheme, 0);
        u_.split(id_user, 0);
        u_.split(id_pass, 0);
        u_.split(id_host, 0);
        u_.split(id_port, 0);
        return *this;
    }
    if( s.empty() ||
        s.starts_with('/'))
    {
        // path-empty,
        // path-absolute
        resize_impl(
            id_scheme, id_path, 0, op);
        check_invariants();
        return *this;
    }
    auto const p =
        url_view_base::encoded_segments();
    BOOST_ASSERT(! p.empty());
    auto it = p.begin();
    if((*it).find_first_of(':') ==
        string_view::npos)
    {
        // path-noscheme
        resize_impl(
            id_scheme, id_path, 0, op);
        check_invariants();
        return *this;
    }

    // need "./"
    auto dest = resize_impl(
        id_scheme, id_path, 2, op);
    dest[0] = '.';
    dest[1] = '/';
    u_.split(id_scheme, 0);
    u_.split(id_user, 0);
    u_.split(id_pass, 0);
    u_.split(id_host, 0);
    u_.split(id_port, 0);
    return *this;
}

//------------------------------------------------
//
// Path
//
//------------------------------------------------

bool
url_base::
set_path_absolute(
    bool absolute)
{
    op_t op(*this);
    if(u_.len(id_path) == 0)
    {
        if(! absolute)
            return true;
        auto dest = resize_impl(
            id_path, 1, op);
        *dest = '/';
        return true;
    }

    if(s_[u_.offset(id_path)] == '/')
    {
        if(absolute)
            return true;
        if( has_authority() &&
            u_.len(id_path) > 1)
            return false;
        auto n = u_.len(id_port);
        u_.split(id_port, n + 1);
        resize_impl(id_port, n, op);
        return true;
    }

    if(! absolute)
        return true;
    auto n = u_.len(id_port);
    auto dest = resize_impl(
        id_port, n + 1, op) + n;
    u_.split(id_port, n);
    *dest = '/';
    return true;
}

url_base&
url_base::
set_encoded_path(
    pct_string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    int abs_hint;
    if(s.starts_with('/'))
        abs_hint = 1;
    else
        abs_hint = 0;
    edit_segments(
        0,
        u_.nseg_,
        detail::enc_path_iter(s),
        detail::enc_path_iter(s),
        abs_hint);
    return *this;
}

url_base&
url_base::
set_path(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    int abs_hint;
    if(s.starts_with('/'))
        abs_hint = 1;
    else
        abs_hint = 0;
    edit_segments(
        0, u_.nseg_,
        detail::plain_path_iter(s),
        detail::plain_path_iter(s),
        abs_hint);
    return *this;
}

segments_encoded
url_base::
encoded_segments() noexcept
{
    return segments_encoded(*this);
}

//------------------------------------------------
//
// Query
//
//------------------------------------------------

url_base&
url_base::
remove_query() noexcept
{
    op_t op(*this);
    resize_impl(id_query, 0, op);
    u_.nparam_ = 0;
    u_.decoded_[id_query] = 0;
    return *this;
}

url_base&
url_base::
set_encoded_query(
    pct_string_view s)
{
    op_t op(*this);
    encode_opts opt;
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);

    std::size_t n = 0;      // encoded size
    std::size_t dn = 0;     // decoded size
    std::size_t nparam = 1; // param count
    auto const end = s.end();
    auto p = s.begin();

    // measure
    while(p != end)
    {
        if(*p == '&')
        {
            ++p;
            ++n;
            ++nparam;
        }
        else if(*p != '%')
        {
            if(detail::query_chars(*p))
                n += 1; // allowed
            else
                n += 3; // escaped
            ++p;
        }
        else
        {
            // escape
            n += 3;
            p += 3;
        }
        ++dn;
    }

    // resize
    auto dest = resize_impl(
        id_query, n + 1, op);
    *dest++ = '?';

    // encode
    detail::re_encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::query_chars);
    u_.nparam_ = nparam;
    u_.decoded_[id_query] = dn;
    return *this;
}

url_base&
url_base::
set_query(
    string_view s)
{
    if (s.empty())
        remove_query();
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    edit_params(
        detail::params_iter_impl(u_),
        detail::params_iter_impl(u_, 0),
        detail::query_iter(
            s, detail::not_empty));
    u_.decoded_[id_query] =
        detail::decode_bytes_unchecked(
            encoded_query());
    return *this;
}

//------------------------------------------------
//
// Fragment
//
//------------------------------------------------

url_base&
url_base::
remove_fragment() noexcept
{
    op_t op(*this);
    resize_impl(id_frag, 0, op);
    u_.decoded_[id_frag] = 0;
    return *this;
}

url_base&
url_base::
set_encoded_fragment(
    pct_string_view s)
{
    op_t op(*this, detail::ref(s));
    encode_opts opt;
    auto const n =
        detail::re_encoded_size(s,
            opt, detail::fragment_chars);
    auto dest = resize_impl(
        id_frag, n + 1, op);
    *dest++ = '#';
    detail::re_encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::fragment_chars);
    u_.decoded_[id_frag] =
        s.decoded_size();
    return *this;
}

url_base&
url_base::
set_fragment(string_view s)
{
    op_t op(*this, s);
    encode_opts opt;
    auto const n = encoded_size(s,
        opt, detail::fragment_chars);
    auto dest = resize_impl(
        id_frag, n + 1, op);
    *dest++ = '#';
    detail::encode_unchecked(
        dest,
        dest + n,
        s,
        opt,
        detail::fragment_chars);
    u_.decoded_[id_frag] = s.size();
    return *this;
}

//------------------------------------------------
//
// Resolution
//
//------------------------------------------------

result<void>
url_base::
resolve_impl(
    url_view_base const& base,
    url_view_base const& ref)
{
    op_t op(*this);
    auto const remove_dot_segments =
        [this]
    {
        auto segs = encoded_segments();
        if(segs.empty())
            return;
        auto it = segs.begin();
        int n = 0;
        for(;;)
        {
            if(*it == ".")
            {
                if(it != std::prev(segs.end()))
                    it = segs.erase(it);
                else
                    it = segs.replace(it, "");
            }
            else if(it == segs.begin())
            {
            #if 0
                if(*it == "..")
                    it = segs.erase(it);
                else
                    ++it;
            #else
            /*  Errata 4547
                https://www.rfc-editor.org/errata/eid4547
            */
                if(*it != "..")
                    ++n;
                ++it;
            #endif
            }
            else if(*it == "..")
            {
                if(n > 0)
                {
                    it = segs.erase(std::prev(it));
                    if( it == segs.begin() ||
                        it != std::prev(segs.end()))
                        it = segs.erase(it);
                    else
                        it = segs.replace(it, "");
                    --n;
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++n;
                ++it;
            }
            if(it == segs.end())
                break;
        }
    };

    if(! base.has_scheme())
    {
        return error::not_a_base;
    }

    //
    // 5.2.2. Transform References
    // https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.2
    //

    if(ref.has_scheme())
    {
        reserve_impl(ref.size(), op);
        copy(ref);
        remove_dot_segments();
        return {};
    }
    if(ref.has_authority())
    {
        reserve_impl(
            base.u_.offset(id_user) +
                ref.size(), op);
        clear();
        set_scheme(base.scheme());
        set_encoded_authority(
            ref.encoded_authority());
        set_encoded_path(
            ref.encoded_path());
        remove_dot_segments();
        if(ref.has_query())
            set_encoded_query(
                ref.encoded_query());
        if(ref.has_fragment())
            set_encoded_fragment(
                ref.encoded_fragment());
        return {};
    }
    if(ref.encoded_path().empty())
    {
        if(ref.has_query())
        {
            reserve_impl(
                base.u_.offset(id_query) +
                    ref.size(), op);
            clear();
            set_scheme(base.scheme());
            if(base.has_authority())
                set_encoded_authority(
                    base.encoded_authority());
            set_encoded_path(
                base.encoded_path());
            remove_dot_segments();
            set_encoded_query(
                ref.encoded_query());
        }
        else
        {
            reserve_impl(
                base.u_.offset(id_query) +
                    ref.size(), op);
            clear();
            set_scheme(base.scheme());
            if(base.has_authority())
                set_encoded_authority(
                    base.encoded_authority());
            set_encoded_path(
                base.encoded_path());
            remove_dot_segments();
            if(base.has_query())
                set_encoded_query(
                    base.encoded_query());
        }
        if(ref.has_fragment())
            set_encoded_fragment(
                ref.encoded_fragment());
        return {};
    }
    if(ref.encoded_path().starts_with('/'))
    {
        reserve_impl(
            base.u_.offset(id_path) +
                ref.size(), op);
        clear();
        set_scheme(base.scheme());
        if(base.has_authority())
            set_encoded_authority(
                base.encoded_authority());
        set_encoded_path(
            ref.encoded_path());
        remove_dot_segments();
        if(ref.has_query())
            set_encoded_query(
                ref.encoded_query());
        if(ref.has_fragment())
            set_encoded_fragment(
                ref.encoded_fragment());
        return {};
    }
    reserve_impl(
        base.u_.offset(id_query) +
            ref.size(), op);
    clear();
    set_scheme(base.scheme());
    if(base.has_authority())
        set_encoded_authority(
            base.encoded_authority());
    {
        // 5.2.3. Merge Paths
        auto es = encoded_segments();
        if(base.u_.nseg_ > 0)
        {
            set_encoded_path(
                base.encoded_path());
            if(u_.nseg_ > 0)
                es.pop_back();
        }
        es.insert(es.end(),
            ref.encoded_segments().begin(),
            ref.encoded_segments().end());
    }
    remove_dot_segments();
    if(ref.has_query())
        set_encoded_query(
            ref.encoded_query());
    if(ref.has_fragment())
        set_encoded_fragment(
            ref.encoded_fragment());
    return {};
}

//------------------------------------------------
//
// Normalization
//
//------------------------------------------------

void
url_base::
normalize_octets_impl(
    int id,
    grammar::lut_chars const& cs,
    op_t& op) noexcept
{
    char* it = s_ + u_.offset(id);
    char* end = s_ + u_.offset(id + 1);
    char buf = 0;
    char* dest = it;
    while (it < end)
    {
        if (*it != '%')
        {
            *dest = *it;
            ++it;
            ++dest;
            continue;
        }
        if (end - it < 3)
            break;

        // decode unreserved octets
        detail::decode_unchecked(
            &buf,
            &buf + 1,
            string_view(it, 3));
        if (cs(buf))
        {
            *dest = buf;
            it += 3;
            ++dest;
            continue;
        }

        // uppercase percent-encoding triplets
        ++it;
        *it = grammar::to_upper(*it);
        ++it;
        *it = grammar::to_upper(*it);
        ++it;
        dest += 3;
    }
    if (it != dest)
    {
        std::size_t diff = it - dest;
        std::size_t n = u_.len(id) - diff;
        shrink_impl(id, n, op);
        s_[size()] = '\0';
    }
}

void
url_base::
decoded_to_lower_impl(int id) noexcept
{
    char* it = s_ + u_.offset(id);
    char const* const end = s_ + u_.offset(id + 1);
    while(it < end)
    {
        if (*it != '%')
        {
            *it = grammar::to_lower(
                *it);
            ++it;
            continue;
        }
        it += 3;
    }
}

void
url_base::
to_lower_impl(int id) noexcept
{
    char* it = s_ + u_.offset(id);
    char const* const end = s_ + u_.offset(id + 1);
    while(it < end)
    {
        *it = grammar::to_lower(
            *it);
        ++it;
    }
}

url_base&
url_base::
normalize_scheme()
{
    to_lower_impl(id_scheme);
    return *this;
}

url_base&
url_base::
normalize_authority()
{
    op_t op(*this);

    // normalize host
    if (host_type() == urls::host_type::name)
    {
        normalize_octets_impl(
            id_host,
            detail::reg_name_chars, op);
    }
    decoded_to_lower_impl(id_host);

    // normalize password
    normalize_octets_impl(id_pass, detail::password_chars, op);

    // normalize user
    normalize_octets_impl(id_user, detail::user_chars, op);
    return *this;
}

url_base&
url_base::
normalize_path()
{
    op_t op(*this);
    normalize_octets_impl(id_path, detail::path_chars, op);
    string_view p = encoded_path();
    char* p_dest = s_ + u_.offset(id_path);
    char* p_end = s_ + u_.offset(id_path + 1);
    std::size_t pn = p.size();
    bool abs = is_path_absolute();
    std::size_t n = detail::remove_dot_segments(
        p_dest, p_end, p, abs);
    if (n != pn)
    {
        BOOST_ASSERT(n < pn);
        shrink_impl(id_path, n, op);
        p = encoded_path();
        if (!p.empty())
            u_.nseg_ = std::count(
                p.begin() + 1, p.end(), '/') + 1;
        else
            u_.nseg_ = 0;
    }
    return *this;
}

url_base&
url_base::
normalize_query()
{
    op_t op(*this);
    normalize_octets_impl(
        id_query, detail::query_chars, op);
    return *this;
}

url_base&
url_base::
normalize_fragment()
{
    op_t op(*this);
    normalize_octets_impl(
        id_frag, detail::fragment_chars, op);
    return *this;
}

url_base&
url_base::
normalize()
{
    normalize_fragment();
    normalize_query();
    normalize_path();
    normalize_authority();
    normalize_scheme();
    return *this;
}

//------------------------------------------------
//
// Implementation
//
//------------------------------------------------

void
url_base::
check_invariants() const noexcept
{
    BOOST_ASSERT(
        u_.len(id_scheme) == 0 ||
        u_.get(id_scheme).ends_with(':'));
    BOOST_ASSERT(
        u_.len(id_user) == 0 ||
        u_.get(id_user).starts_with("//"));
    BOOST_ASSERT(
        u_.len(id_pass) == 0 ||
        u_.get(id_user).starts_with("//"));
    BOOST_ASSERT(
        u_.len(id_pass) == 0 ||
        (u_.len(id_pass) == 1 &&
            u_.get(id_pass) == "@") ||
        (u_.len(id_pass) > 1 &&
            u_.get(id_pass).starts_with(':') &&
            u_.get(id_pass).ends_with('@')));
    BOOST_ASSERT(
        u_.len(id_user, id_path) == 0 ||
        u_.get(id_user).starts_with("//"));
    BOOST_ASSERT(
        u_.len(id_port) == 0 ||
        u_.get(id_port).starts_with(':'));
    BOOST_ASSERT(
        u_.len(id_query) == 0 ||
        u_.get(id_query).starts_with('?'));
    BOOST_ASSERT(
        (u_.len(id_query) == 0 && u_.nparam_ == 0) ||
        (u_.len(id_query) > 0 && u_.nparam_ > 0));
    BOOST_ASSERT(
        u_.len(id_frag) == 0 ||
        u_.get(id_frag).starts_with('#'));
    BOOST_ASSERT(c_str()[size()] == '\0');
}

char*
url_base::
resize_impl(
    int id,
    std::size_t new_size,
    op_t& op)
{
    return resize_impl(
        id, id + 1, new_size, op);
}

char*
url_base::
resize_impl(
    int first,
    int last,
    std::size_t new_len,
    op_t& op)
{
    auto const n0 = u_.len(first, last);
    if(new_len == 0 && n0 == 0)
        return s_ + u_.offset(first);
    if(new_len <= n0)
        return shrink_impl(
            first, last, new_len, op);

    // growing
    std::size_t n = new_len - n0;
    reserve_impl(size() + n, op);
    auto const pos =
        u_.offset(last);
    // adjust chars
    op.move(
        s_ + pos + n,
        s_ + pos,
        u_.offset(id_end) -
            pos + 1);
    // collapse (first, last)
    u_.collapse(first, last,
        u_.offset(last) + n);
    // shift (last, end) right
    u_.adjust(last, id_end, n);
    s_[size()] = '\0';
    return s_ + u_.offset(first);
}

char*
url_base::
shrink_impl(
    int id,
    std::size_t new_size,
    op_t& op)
{
    return shrink_impl(
        id, id + 1, new_size, op);
}

char*
url_base::
shrink_impl(
    int first,
    int last,
    std::size_t new_len,
    op_t& op)
{
    // shrinking
    auto const n0 = u_.len(first, last);
    BOOST_ASSERT(new_len <= n0);
    std::size_t n = n0 - new_len;
    auto const pos =
        u_.offset(last);
    // adjust chars
    op.move(
        s_ + pos - n,
        s_ + pos,
        u_.offset(
            id_end) - pos + 1);
    // collapse (first, last)
    u_.collapse(first,  last,
        u_.offset(last) - n);
    // shift (last, end) left
    u_.adjust(
        last, id_end, 0 - n);
    s_[size()] = '\0';
    return s_ + u_.offset(first);
}

//------------------------------------------------

void
url_base::
set_scheme_impl(
    string_view s,
    urls::scheme id)
{
    op_t op(*this, s);
    check_invariants();
    grammar::parse(s,
        detail::scheme_rule()).value();
    auto const n = s.size();
    auto const p = u_.offset(id_path);

    // check for "./" prefix
    bool const has_dot =
        [this, p]
    {
        if(u_.nseg_ == 0)
            return false;
        // VFALCO Should not be calling segment()
        if(segment(1) <
            u_.offset(id_path) + 2)
            return false;
        auto const src = s_ + p;
        if(src[0] != '.')
            return false;
        if(src[1] != '/')
            return false;
        return true;
    }();

    // Remove "./"
    if(has_dot)
    {
        // do this first, for
        // strong exception safety
        reserve_impl(
            size() + n + 1 - 2, op);
        op.move(
            s_ + p,
            s_ + p + 2,
            size() + 1 -
                (p + 2));
        u_.set_size(
            id_path,
            u_.len(id_path) - 2);
        s_[size()] = '\0';
    }

    auto dest = resize_impl(
        id_scheme, n + 1, op);
    s.copy(dest, n);
    dest[n] = ':';
    u_.scheme_ = id;
    check_invariants();
}

char*
url_base::
set_user_impl(
    std::size_t n,
    op_t& op)
{
    check_invariants();
    if(u_.len(id_pass) != 0)
    {
        // keep "//"
        auto dest = resize_impl(
            id_user, 2 + n, op);
        check_invariants();
        return dest + 2;
    }
    // add authority
    auto dest = resize_impl(
        id_user, 2 + n + 1, op);
    u_.split(id_user, 2 + n);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    check_invariants();
    return dest + 2;
}

char*
url_base::
set_password_impl(
    std::size_t n,
    op_t& op)
{
    check_invariants();
    if(u_.len(id_user) != 0)
    {
        // already have authority
        auto const dest = resize_impl(
            id_pass, 1 + n + 1, op);
        dest[0] = ':';
        dest[n + 1] = '@';
        check_invariants();
        return dest + 1;
    }
    // add authority
    auto const dest =
        resize_impl(
        id_user, id_host,
        2 + 1 + n + 1, op);
    u_.split(id_user, 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    check_invariants();
    return dest + 3;
}

char*
url_base::
set_userinfo_impl(
    std::size_t n,
    op_t& op)
{
    // "//" {dest} "@"
    check_invariants();
    auto dest = resize_impl(
        id_user, id_host, n + 3, op);
    u_.split(id_user, n + 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[n + 2] = '@';
    check_invariants();
    return dest + 2;
}

char*
url_base::
set_host_impl(
    std::size_t n,
    op_t& op)
{
    check_invariants();
    if(u_.len(id_user) == 0)
    {
        // add authority
        auto dest = resize_impl(
            id_user, n + 2, op);
        u_.split(id_user, 2);
        u_.split(id_pass, 0);
        dest[0] = '/';
        dest[1] = '/';
        check_invariants();
        return dest + 2;
    }
    // already have authority
    auto const dest = resize_impl(
        id_host, n, op);
    check_invariants();
    return dest;
}

char*
url_base::
set_port_impl(
    std::size_t n,
    op_t& op)
{
    check_invariants();
    if(u_.len(id_user) != 0)
    {
        // authority exists
        auto dest = resize_impl(
            id_port, n + 1, op);
        dest[0] = ':';
        check_invariants();
        return dest + 1;
    }
    auto dest = resize_impl(
        id_user, 3 + n, op);
    u_.split(id_user, 2);
    u_.split(id_pass, 0);
    u_.split(id_host, 0);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    check_invariants();
    return dest + 3;
}

//------------------------------------------------

/*  Return offset of i-th segment
*/
pos_t
url_base::
segment(
    std::size_t i) const noexcept
{
    if(i == 0)
        return u_.offset(id_path);
    if(i == u_.nseg_)
        return u_.offset(id_query);
    BOOST_ASSERT(i < u_.nseg_);
    auto it = s_ + u_.offset(id_path) +
        detail::path_prefix(
            u_.get(id_path));
    BOOST_ASSERT(it < s_ +
        u_.offset(id_query));
    for(;;)
    {
        while(*it != '/')
            ++it;
        BOOST_ASSERT(it < s_ +
            u_.offset(id_query));
        --i;
        if(i == 0)
            break;
        ++it;
    }
    return it - s_;
}

/*  Remove segments [first, last) and make
    room for nseg new segments inserted
    before first, with space for n chars
    including prefix and/or separators.

    Segments look like this, where ## is the
    malleable prefix and '/' is a literal slash:

    ##_0_ /_1_ /_2_ /_3_
*/
char*
url_base::
resize_segments(
    std::size_t i0,
    std::size_t i1,
    std::size_t n,
    std::size_t nseg,
    op_t& op)
{
    BOOST_ASSERT(i1 >= i0);
    BOOST_ASSERT(i1 - i0 <= u_.nseg_);

    // new number of segments
    std::size_t const nseg1 =
        u_.nseg_ + nseg - (i1 - i0);

    // [p0, p1) range to replace
    auto p0 = segment(i0);
    auto p1 = segment(i1);
    if(i1 == 0)
    {
        p1 += detail::path_prefix(
            u_.get(id_path));
    }
    else if(
        i0 == 0 &&
        nseg == 0 &&
        i1 < u_.nseg_)
    {
        // Remove the slash from segment i1
        // if it is becoming the new first
        // segment.
        BOOST_ASSERT(s_[p1] == '/');
        ++p1;
    }

    // old size of [p0, p1)
    auto const n0 = p1 - p0;

    // adjust capacity
    std::size_t c = size() + n - n0;
    if (c == 0)
        return nullptr;

    reserve_impl(c, op);

    // start of output
    auto dest = s_ + p0;

    // move and size
    op.move(
        dest + n,
        s_ + p1,
        size() - p1);
    u_.set_size(
        id_path,
        u_.len(id_path) -
            (n0 - n));
    u_.nseg_ = nseg1;
    s_[size()] = '\0';
    return dest;
}

// insert or replace [i0, i1)
// with [it0, it1)
void
url_base::
edit_segments(
    std::size_t i0,
    std::size_t i1,
    detail::any_path_iter&& it0,
    detail::any_path_iter&& it1,
    int abs_hint)
{
    op_t op(*this);

    bool abs;
    if( has_authority() ||
        abs_hint == -1)
        abs = is_path_absolute();
    else if(abs_hint == 1)
        abs = true;
    else
        abs = false;

/*
    Measure the number of characters and
    the number of segments we are inserting.
    This does not include leading or trailing
    separators.
*/
    error_code ec;
    std::size_t n = 0;
    std::size_t nseg = 0;
    bool more = it0.measure(n, ec);
    if(ec.failed())
        detail::throw_invalid_argument();
    if(more)
    {
        for(;;)
        {
            ++nseg;
            more = it0.measure(n, ec);
            if(ec.failed())
                detail::throw_invalid_argument();
            if(! more)
                break;
            ++n;
        }
    }

/*  Calculate prefix size for new segment range:
        0 = ""
        1 = "/"
        2 = "./"
        3 = "/./"

    This is a malleable prefix that might need to
    change according the URL scheme and authority.

*/
    int prefix;
    if(i0 > 0)
    {
        if(nseg > 0)
            prefix = 1;
        else
            prefix = 0;
    }
    else if(
        it0.front == "." &&
        nseg > 1)
    {
        if( abs ||
            has_authority())
            prefix = 3;
        else
            prefix = 2;
    }
    else if(has_authority())
    {
        if(nseg == 0)
            prefix = abs ? 1 : 0;
        else if(! it0.front.empty())
            prefix = 1;
        else
            prefix = 3;
    }
    else if(
        nseg > 1 &&
        it0.front.empty())
    {
        prefix = 3;
    }
    else if(
        ! abs &&
        ! has_scheme() &&
        (
            it0.front.find_first_of(
                ':') != string_view::npos ||
            it0.front.empty()))
    {
        if (nseg > 0)
            prefix = 2;
        else
            prefix = 0;
    }
    else if(
        abs &&
        nseg > 0 &&
        it0.front.empty())
    {
        BOOST_ASSERT(
            ! has_authority());
        prefix = 3;
    }
    else
    {
        if(abs)
            prefix = 1;
        else
            prefix = 0;
    }

/*  Calculate suffix size for the new segments:
        0 = ""
        1 = "/"

    This extra suffix should cover the case where
    insertion at the first indexes leaves a
    missing slash in a relative path:

    "file.txt"
    -> insert "etc" as first segment
    -> becomes "etc" "/" "file.txt"

    "file.txt"
    -> insert "path/to" as first segments
    -> becomes "path/to" "/" "file.txt"

    "the/file.txt"
    -> insert "path/to" as first segments
    -> becomes "path/to" "/" "the/file.txt"

    The extra slash is not necessary when
    insertion is not at the first position:

    "path/file.txt"
    -> insert "to/the" as second segment
    -> becomes "path" "/to/the" "/file.txt"

    The extra slash is not necessary when
    the following position already has a slash
    (i.e. other existing valid segments):

    "/path/to/the/file.txt"
    -> replace "etc" as first segment
    -> becomes "/etc" "/to/the/file.txt"

*/
    int suffix;
    // inserting non-empty segments at the
    // beginning of non-empty segments
    if( nseg > 0 &&
        i0 == 0 &&
        i1 == 0 &&
        u_.nseg_ != 0)
    {
        suffix = 1;
    }
    else
    {
        suffix = 0;
    }

    // copy
    n += prefix + suffix;
    auto dest = resize_segments(
        i0, i1, n, nseg, op);
    char const *const last = dest + n;

/*  Write all characters in the destination:

    The output proceeds as:

        prefix [ segment [ '/' segment ] ] suffix
*/
    switch(prefix)
    {
    case 3:
        *dest++ = '/';
        *dest++ = '.';
        *dest++ = '/';
        break;
    case 2:
        *dest++ = '.';
        BOOST_FALLTHROUGH;
    case 1:
        *dest++ = '/';
        break;
    default:
        break;
    }
/*
    Output each segment, placing a slash
    only in between new segments. Leading
    or trailing separators are handled
    outside the loop.
*/
    if(nseg > 0)
    {
        for(;;)
        {
            it1.copy(dest, last);
            if(--nseg == 0)
                break;
            *dest++ = '/';
        }
    }
    if(suffix == 1)
        *dest++ = '/';
}

//------------------------------------------------

/*  The query param range [i0, i1)
    is resized to contain `n` chars
    and nparam elements.
*/
char*
url_base::
resize_params(
    detail::params_iter_impl const& first,
    detail::params_iter_impl const& last,
    std::size_t n,
    std::size_t nparam,
    op_t& op)
{
    BOOST_ASSERT(last.i >= first.i);
    BOOST_ASSERT(
        last.i - first.i <= u_.nparam_);

    // new number of params_view
    auto const nparam1 =
        u_.nparam_ + nparam - (
            last.i - first.i);

    // old size of [first, last)
    auto const n0 =
        last.pos - first.pos;

    // adjust capacity
    reserve_impl(
        size() + n - n0, op);

    auto const src = u_.cs_ +
        u_.offset(id_query);
    auto dest = s_ +
        u_.offset(id_query) +
        first.pos;

    // move and size
    if(u_.nparam_ > 0)
    {
        // needed when we move
        // the beginning of the query
        s_[u_.offset(id_query)] = '&';
    }
    op.move(
        dest + n,
        src + last.pos,
        size() -
            u_.offset(id_query) -
            last.pos);
    u_.set_size(
        id_query,
        u_.len(id_query) +
            n - n0);
    u_.nparam_ = nparam1;
    if(nparam1 > 0)
    {
        // needed when we erase
        // the beginning of the query
        s_[u_.offset(id_query)] = '?';
    }
    s_[size()] = '\0';
    return dest;
}

auto
url_base::
edit_params(
    detail::params_iter_impl const& first,
    detail::params_iter_impl const& last,
    detail::any_params_iter&& it) ->
        detail::params_iter_impl
{
    BOOST_ASSERT(first.u == &u_);
    BOOST_ASSERT(last.u == &u_);
    BOOST_ASSERT(
        first.i == 0 || u_.nparam_ > 0);

    op_t op(*this);

    //
    // measure
    //
    std::size_t n = 0;
    std::size_t nparam = 0;
    error_code ec;
    bool more = it.measure(n, ec);
    if(ec.failed())
    {
        // VFALCO shouldn't we throw `ec`?
        detail::throw_invalid_argument();
    }
    if(more)
    {
        ++n; // for '?' or '&'
        for(;;)
        {
            ++nparam;
            more = it.measure(n, ec);
            if(ec.failed())
            {
                // VFALCO shouldn't we throw `ec`?
                detail::throw_invalid_argument();
            }
            if(! more)
                break;
            ++n; // for '&'
        }
    }

    //
    // copy
    //
    it.rewind();
    auto dest = resize_params(
        first, last, n, nparam, op);
    if(nparam > 0)
    {
        auto const end = dest + n;
        if(first.i == 0)
            *dest++ = '?';
        else
            *dest++ = '&';
        for(;;)
        {
            it.copy(dest, end);
            if(--nparam == 0)
                break;
            *dest++ = '&';
        }
    }
    u_.decoded_[id_query] =
        detail::decode_bytes_unchecked(
            encoded_query());

    return detail::params_iter_impl(
        u_, first.pos, first.i);
}

} // urls
} // boost

#endif

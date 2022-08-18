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
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/print.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/detail/charsets.hpp>
#include <boost/url/rfc/detail/fragment_rule.hpp>
#include <boost/url/rfc/detail/host_rule.hpp>
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

// construct reference
url_base::
url_base(
    detail::url_impl const& impl) noexcept
    : url_view_base(impl)
{
}

// make a copy of u
void
url_base::
copy(url_view_base const& u)
{
    if(u.size() == 0)
    {
        clear();
        return;
    }
    reserve(u.size());
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

void
url_base::
set_scheme_impl(
    string_view s,
    urls::scheme id)
{
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
        reserve(
            size() + n + 1 - 2);
        std::memmove(
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
        id_scheme, n + 1);
    s.copy(dest, n);
    dest[n] = ':';
    u_.scheme_ = id;
    check_invariants();
}

url_base&
url_base::
remove_scheme() noexcept
{
    check_invariants();
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
        resize_impl(id_scheme, 0);
        u_.scheme_ = urls::scheme::none;
        check_invariants();
        return *this;
    }

    // remove the scheme but add "./"
    // to the beginning of the path
    BOOST_ASSERT(n >= 2);
    // move [id_scheme, id_path) left
    std::memmove(
        s_,
        s_ + n,
        p - n);
    // move [id_path, id_end) left
    std::memmove(
        s_ + p - (n - 2),
        s_ + p,
        u_.offset(id_end) - p);
    // VFALCO adjust table
    // adjust part offsets.
    // (p is invalidated)
    u_.adjust(id_user, id_path, 0-n);
    u_.adjust(id_query, id_end, 0-(n - 2));
    auto dest = s_ + u_.offset(id_path);
    dest[0] = '.';
    dest[1] = '/';
    s_[size()] = '\0';
    u_.scheme_ = urls::scheme::none;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_scheme(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES>cs(
            this->string());
    s = cs.maybe_copy(s);
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
url_base::
set_user_impl(std::size_t n)
{
    check_invariants();
    if(u_.len(id_pass) != 0)
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
    u_.split(id_user, 2 + n);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    check_invariants();
    return dest + 2;
}

url_base&
url_base::
set_user(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const n = pct_encode_bytes(
        s, detail::user_chars);
    auto dest = set_user_impl(n);
    pct_encode(dest, u_.get(id_pass).data(),
        s, detail::user_chars);
    u_.decoded_[id_user] = s.size();
    check_invariants();
    return *this;
}

url_base&
url_base::
set_user(pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    check_invariants();
    auto const n =
        detail::pct_encode_bytes_impl(
            s.begin(),
            s.end(),
            detail::user_chars);
    auto dest = set_user_impl(n);
    detail::pct_encode_impl(
        dest,
        u_.get(id_pass).data(),
        s.begin(),
        s.end(),
        detail::user_chars);
    u_.decoded_[id_user] = s.size();
    check_invariants();
    return *this;
}

url_base&
url_base::
set_encoded_user(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const rn =
        validate_pct_encoding(
            s,
            detail::user_chars,
            {});
    if( !rn )
        detail::throw_invalid_argument();
    auto dest = set_user_impl(s.size());
    u_.decoded_[id_user] = *rn;
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
url_base::
set_password_impl(
    std::size_t n)
{
    check_invariants();
    if(u_.len(id_user) != 0)
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
    u_.split(id_user, 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    check_invariants();
    return dest + 3;
}

url_base&
url_base::
remove_password() noexcept
{
    check_invariants();
    auto const n = u_.len(id_pass);
    if(n < 2)
        return *this;
    // clear password, retain '@'
    auto dest =
        resize_impl(id_pass, 1);
    dest[0] = '@';
    u_.decoded_[id_pass] = 0;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_password(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const n = pct_encode_bytes(
        s, detail::password_chars);
    auto dest = set_password_impl(n);
    pct_encode(
        dest,
        u_.get(id_host).data() - 1,
        s,
        detail::password_chars);
    u_.decoded_[id_pass] = s.size();
    check_invariants();
    return *this;
}

url_base&
url_base::
set_password(pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    check_invariants();
    auto const n =
        detail::pct_encode_bytes_impl(
            s.begin(),
            s.end(),
            detail::password_chars);
    auto dest = set_password_impl(n);
    detail::pct_encode_impl(
        dest,
        u_.get(id_host).data() - 1,
        s.begin(),
        s.end(),
        detail::password_chars);
    u_.decoded_[id_pass] = s.size();
    check_invariants();
    return *this;
}

url_base&
url_base::
set_encoded_password(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const rn = validate_pct_encoding(
        s, detail::password_chars, {});
    if( !rn )
        detail::throw_invalid_argument();
    auto dest =
        set_password_impl(s.size());
    u_.decoded_[id_pass] = *rn;
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
url_base::
set_userinfo_impl(
    std::size_t n)
{
    // "//" {dest} "@"
    check_invariants();
    auto dest = resize_impl(
        id_user, id_host, n + 3);
    u_.split(id_user, n + 2);
    dest[0] = '/';
    dest[1] = '/';
    dest[n + 2] = '@';
    check_invariants();
    return dest + 2;
}

url_base&
url_base::
remove_userinfo() noexcept
{
    check_invariants();
    if(u_.len(id_pass) == 0)
    {
        // no userinfo
        return *this;
    }
    // keep authority '//'
    resize_impl(
        id_user, id_host, 2);
    u_.decoded_[id_user] = 0;
    u_.decoded_[id_pass] = 0;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_userinfo(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const n = pct_encode_bytes(
        s, detail::userinfo_chars);
    auto dest = set_userinfo_impl(n);
    pct_encode(
        dest,
        u_.get(id_host).data() - 1,
        s,
        detail::userinfo_chars);
    auto pct_s = u_.get(id_user, id_host);
    auto pct_sep = pct_s.find_first_of(':');
    if (pct_sep != string_view::npos)
    {
        u_.split(id_user, pct_sep);
        auto sep = s.find_first_of(':');
        u_.decoded_[id_user] = sep - 1;
        u_.decoded_[id_pass] = s.size() - sep;
    }
    else
    {
        u_.decoded_[id_user] = s.size();
        u_.decoded_[id_pass] = 0;
    }
    check_invariants();
    return *this;
}

url_base&
url_base::
set_userinfo(
    pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    check_invariants();
    auto const n =
        detail::pct_encode_bytes_impl(
            s.begin(),
            s.end(),
            detail::userinfo_chars);
    auto dest = set_userinfo_impl(n);
    detail::pct_encode_impl(
        dest,
        u_.get(id_host).data() - 1,
        s.begin(),
        s.end(),
        detail::userinfo_chars);
    auto pct_s = u_.get(id_user, id_host);
    auto pct_sep = pct_s.find_first_of(':');
    if (pct_sep != string_view::npos)
    {
        u_.split(id_user, pct_sep);
        std::size_t sep = 0;
        for (char c: s)
        {
            if (c == ':')
                break;
            ++sep;
        }
        u_.decoded_[id_user] = sep - 1;
        u_.decoded_[id_pass] = s.size() - sep;
    }
    else
    {
        u_.decoded_[id_user] = s.size();
        u_.decoded_[id_pass] = 0;
    }
    check_invariants();
    return *this;
}

url_base&
url_base::
set_encoded_userinfo(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto t = grammar::parse(
        s, detail::userinfo_rule).value();
    auto dest = set_userinfo_impl(s.size());
    u_.split(id_user, 2 + t.user.encoded().size());
    if(! s.empty())
        std::memcpy(dest, s.data(), s.size());
    u_.decoded_[id_user] =
        t.user.size();
    if(t.has_password)
        u_.decoded_[id_pass] =
            t.password.size();
    else
        u_.decoded_[id_pass] = 0;
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url_base::
set_host_impl(std::size_t n)
{
    check_invariants();
    if(u_.len(id_user) == 0)
    {
        // add authority
        auto dest = resize_impl(
            id_user, n + 2);
        u_.split(id_user, 2);
        u_.split(id_pass, 0);
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

url_base&
url_base::
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
    u_.decoded_[id_host] = u_.len(id_host);
    u_.host_type_ =
        urls::host_type::ipv4;
    auto bytes = addr.to_bytes();
    std::memcpy(u_.ip_addr_,
        bytes.data(), bytes.size());
    check_invariants();
    return *this;
}

url_base&
url_base::
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
    u_.decoded_[id_host] = u_.len(id_host);
    u_.host_type_ =
        urls::host_type::ipv6;
    auto bytes = addr.to_bytes();
    std::memcpy(u_.ip_addr_,
        bytes.data(), bytes.size());
    check_invariants();
    return *this;
}

url_base&
url_base::
set_host(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    // try ipv4
    {
        auto r = parse_ipv4_address(s);
        if( r )
            return set_host(r.value());
    }
    check_invariants();
    auto const n = pct_encode_bytes(
        s, detail::host_chars);
    auto dest = set_host_impl(n);
    pct_encode(
        dest,
        u_.get(id_path).data(),
        s,
        detail::host_chars);
    u_.decoded_[id_host] = s.size();
    u_.host_type_ =
        urls::host_type::name;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_host(
    pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    // try ipv4
    {
        auto r = parse_ipv4_address(s.encoded());
        if( r )
            return set_host(r.value());
    }
    check_invariants();
    auto const n = detail::pct_encode_bytes_impl(
        s.begin(), s.end(), detail::host_chars);
    auto dest = set_host_impl(n);
    detail::pct_encode_impl(
        dest,
        u_.get(id_path).data(),
        s.begin(),
        s.end(),
        detail::host_chars);
    u_.decoded_[id_host] = s.size();
    u_.host_type_ =
        urls::host_type::name;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_encoded_host(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    // first try parsing it
    auto t = grammar::parse(
        s, detail::host_rule).value();
    BOOST_ASSERT(t.host_type !=
        urls::host_type::none);
    check_invariants();
    std::memcpy(
        set_host_impl(t.match.size()),
        t.match.data(),
        t.match.size());
    std::memcpy(
        u_.ip_addr_,
        t.addr,
        sizeof(u_.ip_addr_));
    if(t.host_type ==
            urls::host_type::name)
        u_.decoded_[id_host] =
            t.name.size();
    else
        u_.decoded_[id_host] =
            t.match.size();
    u_.host_type_ = t.host_type;
    check_invariants();
    return *this;
}

//------------------------------------------------

char*
url_base::
set_port_impl(std::size_t n)
{
    check_invariants();
    if(u_.len(id_user) != 0)
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
    u_.split(id_user, 2);
    u_.split(id_pass, 0);
    u_.split(id_host, 0);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    check_invariants();
    return dest + 3;
}

url_base&
url_base::
remove_port() noexcept
{
    check_invariants();
    resize_impl(id_port, 0);
    u_.port_number_ = 0;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_port(std::uint16_t n)
{
    check_invariants();
    auto s =
        detail::make_printed(n);
    auto dest = set_port_impl(
        s.string().size());
    std::memcpy(
        dest, s.string().data(),
            s.string().size());
    u_.port_number_ = n;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_port(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto t = grammar::parse(
        s, detail::port_rule{}).value();
    auto dest =
        set_port_impl(t.str.size());
    std::memcpy(dest,
        t.str.data(), t.str.size());
    if(t.has_number)
        u_.port_number_ = t.number;
    else
        u_.port_number_ = 0;
    check_invariants();
    return *this;
}

//------------------------------------------------

url_base&
url_base::
remove_authority() noexcept
{
    check_invariants();
    if(u_.len(id_user) == 0)
    {
        // no authority
        return *this;
    }
    if(u_.get(id_path
        ).starts_with("//"))
    {
        // prepend "/."
        auto p = resize_impl(
            id_user, id_path, 2);
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
            id_user, id_path, 0);
    }
    u_.host_type_ =
        urls::host_type::none;
    check_invariants();
    return *this;
}

url_base&
url_base::
set_encoded_authority(
    string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    auto t = grammar::parse(
        s, authority_rule).value();
    auto n = s.size() + 2;
    auto const need_slash =
        ! is_path_absolute() &&
        u_.len(id_path) > 0;
    if(need_slash)
        ++n;
    auto dest = resize_impl(
        id_user, id_path, n);
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
    check_invariants();
    return *this;
}

//------------------------------------------------

url_base&
url_base::
remove_origin() noexcept
{
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
            id_scheme, id_path, 1);
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
            id_scheme, id_path, 0);
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
            id_scheme, id_path, 0);
        check_invariants();
        return *this;
    }

    // need "./"
    auto dest = resize_impl(
        id_scheme, id_path, 2);
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
edit_segments(
    std::size_t i0,
    std::size_t i1,
    std::size_t n,
    std::size_t nseg)
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

    reserve(c);
    // start of output
    auto dest = s_ + p0;

    // move and size
    std::memmove(
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
    auto dest = edit_segments(
        i0, i1, n, nseg);
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

bool
url_base::
set_path_absolute(bool absolute)
{
    if(u_.len(id_path) == 0)
    {
        if(! absolute)
            return true;
        auto dest = resize_impl(
            id_path, 1);
        *dest = '/';
        // VFALCO Update table
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
        resize_impl(id_port, n);
        // VFALCO Update table
        return true;
    }

    if(! absolute)
        return true;
    auto n = u_.len(id_port);
    auto dest = resize_impl(
        id_port, n + 1) + n;
    u_.split(id_port, n);
    *dest = '/';
    // VFALCO Update table
    return true;
}

url_base&
url_base::
set_encoded_path(
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

url_base&
url_base::
set_path(
    pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    int abs_hint;
    if(!s.empty() && s.front() == '/')
        abs_hint = 1;
    else
        abs_hint = 0;
    edit_segments(
        0, u_.nseg_,
        detail::view_path_iter(s),
        detail::view_path_iter(s),
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

auto
url_base::
param(
    std::size_t i) const noexcept ->
        raw_param
{
    auto const make_param =
    [this](
        std::size_t pos,
        std::size_t n)
    {
        string_view s(s_ + pos, n);
        auto i = s.find_first_of('=');
        if(i == string_view::npos)
            return raw_param{ pos, n, 0 };
        return raw_param{ pos, i, n - i };
    };

    if(u_.nparam_ == 0)
        return { u_.offset(id_query), 0, 0 };
    if(i == u_.nparam_)
        return { u_.offset(id_frag), 0, 0 };
    BOOST_ASSERT(i <= u_.nparam_);
    auto n = u_.len(id_query);
    if(u_.nparam_ < 2)
        return make_param(
            u_.offset(id_query), n);
    auto it = s_ + u_.offset(id_query);
    auto start = it;
    auto const last =
        s_ + u_.offset(id_frag);
    BOOST_ASSERT(n > 0);
    for(;;)
    {
        for(;;)
        {
            ++it;
            if(it == last)
                break;
            if(*it == '&')
                break;
        }
        if(i == 0)
            break;
        start = it;
        --i;
    }
    return make_param(
        start - s_, it - start);
}

char*
url_base::
edit_params(
    std::size_t i0,
    std::size_t i1,
    std::size_t n,
    std::size_t nparam)
{
    BOOST_ASSERT(i1 >= i0);
    BOOST_ASSERT(i1 - i0 <= u_.nparam_);

    // new number of params
    auto const nparam1 =
        u_.nparam_+ nparam - (i1 - i0);

    // [r0, r1) range to replace
    auto const r0 = param(i0);
    auto const r1 = param(i1);

    // old size of [r0, r1)
    auto const n0 = r1.pos - r0.pos;

    // adjust capacity
    reserve(size() + n - n0);

    // start of output
    auto dest = s_ + r0.pos;

    // move and size
    std::memmove(
        dest + n,
        s_ + r1.pos,
        size() - r1.pos);
    u_.set_size(
        id_query,
        u_.len(id_query) + (
            n - n0));
    u_.nparam_ = nparam1;
    s_[size()] = '\0';
    return dest;
}

void
url_base::
edit_params(
    std::size_t i0,
    std::size_t i1,
    detail::any_query_iter&& it0,
    detail::any_query_iter&& it1,
    bool set_hint)
{
    check_invariants();
    if(! set_hint)
        set_hint = has_query();

    // measure
    error_code ec;
    std::size_t n = 0;
    std::size_t nparam = 0;
    bool more = it0.measure(n, ec);
    if(ec.failed())
        detail::throw_invalid_argument();
    bool prefix;
    if(more)
    {
        ++n;
        prefix = i0 == 0;
        for(;;)
        {
            ++nparam;
            more = it0.measure(n, ec);
            if(ec.failed())
                detail::throw_invalid_argument();
            if(! more)
                break;
            ++n;
        }
    }
    else if(
        i0 == 0 &&
        set_hint)
    {
        prefix = true;
        ++n;
    }
    else
    {
        prefix = false;
    }

    // copy
    auto dest = edit_params(
        i0, i1, n, nparam);
    if(prefix)
        *dest++ = '?';
    if(nparam > 0)
    {
        auto const last = dest + n;
        if(i0 != 0)
            *dest++ = '&';
        for(;;)
        {
            it1.copy(dest, last);
            if(--nparam == 0)
                break;
            *dest++ = '&';
        }
    }

    check_invariants();
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
    resize_impl(id_query, 0);
    u_.nparam_ = 0;
    u_.decoded_[id_query] = 0;
    return *this;
}

url_base&
url_base::
set_encoded_query(
    string_view s)
{
    if (s.empty())
        remove_query();
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    edit_params(
        0,
        u_.nparam_,
        detail::enc_query_iter(s),
        detail::enc_query_iter(s),
        true);
    u_.decoded_[id_query] =
        pct_decode_bytes_unchecked(
            encoded_query());
    check_invariants();
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
        0,
        u_.nparam_,
        detail::plain_query_iter(s),
        detail::plain_query_iter(s),
        true);
    u_.decoded_[id_query] =
        pct_decode_bytes_unchecked(
            encoded_query());
    return *this;
}

url_base&
url_base::
set_query(
    pct_encoded_view s)
{
    if (s.empty())
        remove_query();
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    edit_params(
        0,
        u_.nparam_,
        detail::view_query_iter(s),
        detail::view_query_iter(s),
        true);
    u_.decoded_[id_query] =
        pct_decode_bytes_unchecked(
            encoded_query());
    return *this;
}

//------------------------------------------------
//
// Fragment
//
//------------------------------------------------

char*
url_base::
set_fragment_impl(
    std::size_t n)
{
    auto dest = resize_impl(
        id_frag, n + 1);
    dest[0] = '#';
    return dest + 1;
}

url_base&
url_base::
remove_fragment() noexcept
{
    resize_impl(id_frag, 0);
    u_.decoded_[id_frag] = 0;
    return *this;
}

url_base&
url_base::
set_encoded_fragment(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto t = grammar::parse(
        s, detail::fragment_rule).value(
            BOOST_CURRENT_LOCATION);
    auto dest = set_fragment_impl(s.size());
    u_.decoded_[id_frag] = t.size();
    if(! s.empty())
        std::memcpy(
            dest, s.data(), s.size());
    check_invariants();
    return *this;
}

url_base&
url_base::
set_fragment(string_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = buf.maybe_copy(s);
    check_invariants();
    auto const n = pct_encode_bytes(
        s, detail::fragment_chars);
    auto dest = set_fragment_impl(n);
    pct_encode(
        dest,
        u_.get(id_end).data(),
        s,
        detail::fragment_chars);
    u_.decoded_[id_frag] = s.size();
    check_invariants();
    return *this;
}

url_base&
url_base::
set_fragment(
    pct_encoded_view s)
{
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> buf(
            this->string());
    s = s.maybe_copy(buf);
    check_invariants();
    auto const n =
        detail::pct_encode_bytes_impl(
            s.begin(),
            s.end(),
            detail::fragment_chars);
    auto dest = set_fragment_impl(n);
    detail::pct_encode_impl(
        dest,
        u_.get(id_end).data(),
        s.begin(),
        s.end(),
        detail::fragment_chars);
    u_.decoded_[id_frag] = s.size();
    check_invariants();
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
        reserve(ref.size());
        copy(ref);
        remove_dot_segments();
        return {};
    }
    if(ref.has_authority())
    {
        reserve(
            base.u_.offset(id_user) +
                ref.size());
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
            reserve(
                base.u_.offset(id_query) +
                    ref.size());
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
            reserve(
                base.u_.offset(id_query) +
                    ref.size());
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
        reserve(
            base.u_.offset(id_path) +
                ref.size());
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
    reserve(
        base.u_.offset(id_query) +
            ref.size());
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
    grammar::lut_chars const& cs) noexcept
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
        pct_decode_unchecked(
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
        shrink_impl(id, n);
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
    // normalize host
    if (host_type() == urls::host_type::name)
    {
        normalize_octets_impl(
            id_host,
            detail::reg_name_chars);
    }
    decoded_to_lower_impl(id_host);

    // normalize password
    normalize_octets_impl(id_pass, detail::password_chars);

    // normalize user
    normalize_octets_impl(id_user, detail::user_chars);
    return *this;
}

url_base&
url_base::
normalize_path()
{
    normalize_octets_impl(id_path, detail::path_chars);
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
        shrink_impl(id_path, n);
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
    normalize_octets_impl(
        id_query, detail::query_chars);
    return *this;
}

url_base&
url_base::
normalize_fragment()
{
    normalize_octets_impl(
        id_frag, detail::fragment_chars);
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
        u_.len(id_frag) == 0 ||
        u_.get(id_frag).starts_with('#'));
    BOOST_ASSERT(c_str()[size()] == '\0');
}

char*
url_base::
resize_impl(
    int id,
    std::size_t new_size)
{
    return resize_impl(
        id, id + 1, new_size);
}

char*
url_base::
resize_impl(
    int first,
    int last,
    std::size_t new_len)
{
    auto const n0 = u_.len(first, last);
    if(new_len == 0 && n0 == 0)
        return s_ + u_.offset(first);
    if(new_len <= n0)
    {
        return shrink_impl(first, last, new_len);
    }

    // growing
    std::size_t n = new_len - n0;
    reserve(size() + n);
    auto const pos =
        u_.offset(last);
    // adjust chars
    std::memmove(
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
    std::size_t new_size)
{
    return shrink_impl(
        id, id + 1, new_size);
}

char*
url_base::
shrink_impl(
    int first,
    int last,
    std::size_t new_len)
{
    // shrinking
    auto const n0 = u_.len(first, last);
    BOOST_ASSERT(new_len <= n0);
    std::size_t n = n0 - new_len;
    auto const pos =
        u_.offset(last);
    // adjust chars
    std::memmove(
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

} // urls
} // boost

#endif

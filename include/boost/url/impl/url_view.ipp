//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_URL_VIEW_IPP
#define BOOST_URL_IMPL_URL_VIEW_IPP

#include <boost/url/url_view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/detail/parse.hpp>

namespace boost {
namespace urls {

//----------------------------------------------------------

url_view::
url_view(string_view s)
    : pt_(s.data())
{
    detail::parser pr(s);
    error_code ec;
    detail::parse_url(pt_, s, ec);
    if(ec)
        invalid_part::raise();
}

string_view
url_view::
encoded_url() const
{
    return pt_.get(
        detail::id_scheme,
        detail::id_end);
}

string_view
url_view::
encoded_origin() const noexcept
{
    return pt_.get(
        detail::id_scheme,
        detail::id_path);
}

//----------------------------------------------------------
//
// scheme
//
//----------------------------------------------------------

string_view
url_view::
scheme() const noexcept
{
    auto s = pt_.get(
        detail::id_scheme);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == ':');
    s.remove_suffix(1); // ':'
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
    return pt_.length(
        detail::id_user,
        detail::id_path) != 0;
}

string_view
url_view::
encoded_authority() const noexcept
{
    auto s = pt_.get(
        detail::id_user,
        detail::id_path);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

//----------------------------------------------------------
//
// userinfo
//
//----------------------------------------------------------

bool
url_view::
has_userinfo() const noexcept
{
    if(pt_.length(
        detail::id_user) == 0)
    {
        BOOST_ASSERT(pt_.length(
            detail::id_user,
            detail::id_host) == 0);
        return false;
    }
    BOOST_ASSERT(pt_.get(
        detail::id_user).substr(
            0, 2) == "//");
    if(pt_.length(
        detail::id_user) > 2)
        return true;
    if(pt_.length(
        detail::id_password) > 0)
    {
        BOOST_ASSERT(pt_.get(
            detail::id_password).
            back() == '@');
        return true;
    }
    return false;
}

string_view
url_view::
encoded_userinfo() const noexcept
{
    auto s = userinfo_part();
    if(! s.empty())
    {
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
    }
    return s;
}

string_view
url_view::
userinfo_part() const noexcept
{
    auto s = pt_.get(
        detail::id_user,
        detail::id_host);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.size() >= 2);
    BOOST_ASSERT(
        s.substr(0, 2) == "//");
    s.remove_prefix(2);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == '@');
    return s;
}

string_view
url_view::
encoded_user() const noexcept
{
    auto s = pt_.get(
        detail::id_user);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

string_view
url_view::
encoded_password() const noexcept
{
    auto s = pt_.get(
        detail::id_password);
    if(! s.empty())
    {
        if(s.front() == ':')
        {
            BOOST_ASSERT(
                s.size() >= 2);
            s.remove_prefix(1);
        }
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
    }
    return s;
}

string_view
url_view::
password_part() const noexcept
{
    auto s = pt_.get(
        detail::id_password);
    if(! s.empty())
    {
        BOOST_ASSERT(s.front() == '@');
        s.remove_suffix(1);
    }
    return s;
}

//----------------------------------------------------------
//
// host
//
//----------------------------------------------------------

string_view
url_view::
encoded_host_and_port() const noexcept
{
    return pt_.get(
        detail::id_host,
        detail::id_path);
}

string_view
url_view::
encoded_host() const noexcept
{
    return pt_.get(
        detail::id_host);
}

string_view
url_view::
port() const noexcept
{
    auto s = pt_.get(
        detail::id_port);
    BOOST_ASSERT(s.empty() ||
        s.front() == ':');
    if(! s.empty())
        s.remove_prefix(1);
    return s;
}

string_view
url_view::
port_part() const noexcept
{
    auto const s = pt_.get(
        detail::id_port);
    BOOST_ASSERT(s.empty() ||
        s.front() == ':');
    return s;
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
        detail::id_path);
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

string_view
url_view::
encoded_query() const noexcept
{
    auto s = pt_.get(
        detail::id_query);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '?');
    return s.substr(1);
}

string_view
url_view::
query_part() const noexcept
{
    auto s = pt_.get(
        detail::id_query);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '?');
    return s;
}

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

string_view
url_view::
encoded_fragment() const noexcept
{
    auto s = pt_.get(
        detail::id_frag);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s.substr(1);
}

string_view
url_view::
fragment_part() const noexcept
{
    auto s = pt_.get(
        detail::id_frag);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s;
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
    : pt_(nullptr)
    , off_(0)
    , n_(0)
{
}

url_view::
segments_type::
iterator::
iterator(
    detail::parts_view const* pt,
    bool end) noexcept
    : pt_(pt)
{
    if(! pt_)
    {
        off_ = 0;
        n_ = 0;
    }
    else if( end ||
        pt_->nseg == 0)
    {
        off_ = pt_->offset(
            detail::id_query);
        n_ = 0;
    }
    else
    {
        off_ = pt_->offset(
            detail::id_path);
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
        pt_->data() + off_, n_ };
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
        off_ != pt_->offset(
            detail::id_frag));
    off_ = off_ + n_;
    if(off_ == pt_->offset(
        detail::id_frag))
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
    char const* const s = pt_->c_str();
    BOOST_ASSERT(
        off_ != pt_->offset(
            detail::id_path));
    auto const begin =
        s + pt_->offset(
            detail::id_path);
    auto p = s + off_;
    while(--p > begin)
    {
        if(*p == '/')
        {
            off_ = p - s;
            parse();
            return *this;
        }
    }
    // fails for relative-uri
    //BOOST_ASSERT(*p == '/');
    auto const off = p - s;
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
    char const* const s = pt_->c_str();
    BOOST_ASSERT(off_ !=
        pt_->offset(
            detail::id_frag));
    auto const end =
        s + pt_->offset(
            detail::id_frag);
    auto const p0 = s + off_;
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
    return iterator(pt_, false);
}

auto
url_view::
segments_type::
end() const noexcept ->
    iterator
{
    return iterator(pt_, true);
}

//----------------------------------------------------------
//
// params_type
//
//----------------------------------------------------------

url_view::
params_type::
iterator::
iterator() noexcept
    : pt_(nullptr)
    , off_(0)
    , nk_(0)
    , nv_(0)
{
}

url_view::
params_type::
iterator::
iterator(
    detail::parts_view const* pt,
    bool end) noexcept
    : pt_(pt)
{
    if(! pt_)
    {
        off_ = 0;
        nk_ = 0;
        nv_ = 0;
    }
    else if( end ||
            pt_->nparam == 0)
    {
        off_ = pt_->offset(
            detail::id_frag);
        nk_ = 0;
        nv_ = 0;
    }
    else
    {
        off_ = pt_->offset(
            detail::id_query);
        parse();
    }
}

auto
url_view::
params_type::
iterator::
operator*() const noexcept ->
    value_type
{
    BOOST_ASSERT(pt_);
    BOOST_ASSERT(pt_->nparam > 0);
    BOOST_ASSERT(nk_ > 0);
    char const* const s = pt_->c_str();
    BOOST_ASSERT(
        off_ == pt_->offset(
            detail::id_query) ?
        s[off_] == '?' :
        s[off_] == '&');
    string_view const k = {
        s + off_ + 1,
        nk_ - 1 };
    if(nv_ == 0)
        return { k, { } };
    BOOST_ASSERT(
        s[off_ + nk_] == '=');
    string_view const v = {
        s + off_ + nk_ + 1,
        nv_ - 1};
    return { k, v };
}

auto
url_view::
params_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(pt_);
    BOOST_ASSERT(pt_->nparam > 0);
    BOOST_ASSERT(
        off_ != pt_->offset(
            detail::id_frag));
    off_ = off_ + nv_ + nk_;
    if(off_ == pt_->offset(
        detail::id_frag))
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
url_view::
params_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(pt_);
    BOOST_ASSERT(pt_->nparam > 0);
    BOOST_ASSERT(
        off_ != pt_->offset(
            detail::id_query));
    char const* const s = pt_->c_str();
    auto const begin =
        s + pt_->offset(
            detail::id_query);
    auto p = s + off_;
    while(--p > begin)
    {
        if(*p == '&')
        {
            off_ = p - s;
            parse();
            return *this;
        }
    }
    BOOST_ASSERT(*p == '?');
    off_ = p - s;
    return *this;
}

void
url_view::
params_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(pt_);
    BOOST_ASSERT(pt_->nparam > 0);
    char const* const s = pt_->c_str();
    auto const end =
        s + pt_->offset(
            detail::id_end);
    auto p = s + off_;
    BOOST_ASSERT(
        ( off_ == pt_->offset(
            detail::id_query) &&
            *p == '?' ) ||
        ( off_ != pt_->offset(
            detail::id_query) &&
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

//----------------------------------------------------------

auto
url_view::
params_type::
begin() const noexcept ->
    iterator
{
    return iterator(pt_, false);
}

auto
url_view::
params_type::
end() const noexcept ->
    iterator
{
    return iterator(pt_, true);
}

bool
url_view::
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
url_view::
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
url_view::
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
url_view::
params_type::
operator[](string_view key) const
{
    auto const it = find(key);
    if(it == end())
        return "";
    return it->value();
}

} // urls
} // boost

#endif

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
#include <boost/url/detail/parse.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/uri_bnf.hpp>
#include <boost/url/rfc/relative_ref_bnf.hpp>

namespace boost {
namespace urls {

string_view
url_view::
get(detail::part id) const noexcept
{
    return pt_.get(id, s_);
}

std::size_t
url_view::
len(detail::part id) const noexcept
{
    return pt_.length(id);
}

//------------------------------------------------

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
    return pt_.get(
        detail::id_scheme,
        detail::id_end,
        s_);
}

string_view
url_view::
encoded_origin() const noexcept
{
    return pt_.get(
        detail::id_scheme,
        detail::id_path,
        s_);
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
        detail::id_scheme);
    if(n == 0)
        return false;
    BOOST_ASSERT(n > 1);
    BOOST_ASSERT(
        get(detail::id_scheme
            ).ends_with(':'));
    return true;
}

string_view
url_view::
scheme() const noexcept
{
    auto s = get(
        detail::id_scheme);
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
    auto const n = 
        pt_.length(
            detail::id_user,
            detail::id_path);
    if(n == 0)
        return false;
    BOOST_ASSERT(n > 1);
    BOOST_ASSERT(pt_.get(
        detail::id_user,
        detail::id_path,
        s_).starts_with("//"));
    return true;
}

string_view
url_view::
encoded_authority() const noexcept
{
    auto s = pt_.get(
        detail::id_user,
        detail::id_path,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() > 1);
        BOOST_ASSERT(
            s.starts_with("//"));
        s.remove_prefix(2);
    }
    return s;
}

// userinfo

bool
url_view::
has_userinfo() const noexcept
{
    auto n = pt_.length(
        detail::id_password);
    if(n == 0)
        return false;
    BOOST_ASSERT(get(
        detail::id_password
            ).ends_with('@'));
    return true;
}

bool
url_view::
has_user() const noexcept
{
    auto const n = len(
        detail::id_user);
    if(n == 0)
    {
        // no authority
        BOOST_ASSERT(pt_.length(
            detail::id_user,
            detail::id_host) == 0);
        return false;
    }
    BOOST_ASSERT(n > 1);
    BOOST_ASSERT(get(
        detail::id_user).
            starts_with("//"));
    if(n > 2)
        return true;
    return false;
}

string_view
url_view::
encoded_userinfo() const noexcept
{
    auto s = pt_.get(
        detail::id_user,
        detail::id_host,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.size() >= 2);
    BOOST_ASSERT(
        s.substr(0, 2) == "//");
    s.remove_prefix(2);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == '@');
    s.remove_suffix(1);
    return s;
}

string_view
url_view::
encoded_user() const noexcept
{
    auto s = pt_.get(
        detail::id_user,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

bool
url_view::
has_password() const noexcept
{
    auto const n = pt_.length(
        detail::id_password);
    if(n > 1)
    {
        BOOST_ASSERT(get(
            detail::id_password
                ).starts_with(':'));
        BOOST_ASSERT(get(
            detail::id_password
                ).ends_with('@'));
        return true;
    }
    BOOST_ASSERT(n == 0 ||
        get(detail::id_password
            ).ends_with('@'));
    return false;
}

string_view
url_view::
encoded_password() const noexcept
{
    auto s = pt_.get(
        detail::id_password,
        s_);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(s.front() == '@');
    case 0:
        return {};
    default:
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
        if(s.front() == ':')
            s.remove_prefix(1);
        return s;
    }
}

// host

string_view
url_view::
encoded_host_and_port() const noexcept
{
    return pt_.get(
        detail::id_host,
        detail::id_path,
        s_);
}

string_view
url_view::
encoded_host() const noexcept
{
    return get(detail::id_host);
}

bool
url_view::
has_port() const noexcept
{
    auto const n =
        len(detail::id_port);
    if(n == 0)
        return false;
    BOOST_ASSERT(
        get(detail::id_port
            ).starts_with(':'));
    return true;
}

string_view
url_view::
port() const noexcept
{
    auto s = pt_.get(
        detail::id_port, s_);
    BOOST_ASSERT(s.empty()
        || s.front() == ':');
    if(! s.empty())
        s.remove_prefix(1);
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
        detail::id_path,
        s_);
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
        detail::id_query, s_);
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
        detail::id_query, s_);
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
        detail::id_frag, s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s.substr(1);
}

string_view
url_view::
fragment_bnf() const noexcept
{
    auto s = pt_.get(
        detail::id_frag, s_);
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
            detail::id_query];
        n_ = 0;
    }
    else
    {
        off_ = pt_->offset[
            detail::id_path];
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
            detail::id_frag]);
    off_ = off_ + n_;
    if(off_ == pt_->offset[
        detail::id_frag])
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
            detail::id_path]);
    auto const begin =
        s_ + pt_->offset[
            detail::id_path];
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
            detail::id_frag]);
    auto const end =
        s_ + pt_->offset[
            detail::id_frag];
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

//----------------------------------------------------------
//
// params_type
//
//----------------------------------------------------------

url_view::
params_type::
iterator::
iterator() noexcept
    : s_(nullptr)
    , pt_(nullptr)
    , off_(0)
    , nk_(0)
    , nv_(0)
{
}

url_view::
params_type::
iterator::
iterator(
    params_type const* v,
    bool end) noexcept
    : s_(v->s_)
    , pt_(v->pt_)
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
        off_ = pt_->offset[
            detail::id_frag];
        nk_ = 0;
        nv_ = 0;
    }
    else
    {
        off_ = pt_->offset[
            detail::id_query];
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
    BOOST_ASSERT(
        off_ == pt_->offset[
            detail::id_query] ?
        s_[off_] == '?' :
        s_[off_] == '&');
    string_view const k = {
        s_ + off_ + 1,
        nk_ - 1 };
    if(nv_ == 0)
        return { k, { } };
    BOOST_ASSERT(
        s_[off_ + nk_] == '=');
    string_view const v = {
        s_ + off_ + nk_ + 1,
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
        off_ != pt_->offset[
            detail::id_frag]);
    off_ = off_ + nv_ + nk_;
    if(off_ == pt_->offset[
        detail::id_frag])
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
        off_ != pt_->offset[
            detail::id_query]);
    auto const begin =
        s_ + pt_->offset[
            detail::id_query];
    auto p = s_ + off_;
    while(--p > begin)
    {
        if(*p == '&')
        {
            off_ = p - s_;
            parse();
            return *this;
        }
    }
    BOOST_ASSERT(*p == '?');
    off_ = p - s_;
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
    auto const end =
        s_ + pt_->offset[
            detail::id_end];
    auto p = s_ + off_;
    BOOST_ASSERT(
        ( off_ == pt_->offset[
            detail::id_query] &&
            *p == '?' ) ||
        ( off_ != pt_->offset[
            detail::id_query] &&
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
    return iterator(this, false);
}

auto
url_view::
params_type::
end() const noexcept ->
    iterator
{
    return iterator(this, true);
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

//------------------------------------------------

namespace detail {

static
void
apply_authority(parts& p,
    optional<rfc::authority_bnf> const& t)
{
    if(! t.has_value())
        return;
    auto const& u =
        t->userinfo;
    if(u.has_value())
    {
        p.resize(
            part::id_user,
            u->user.str.size() + 2);
        if(u->pass.has_value())
            p.resize(
                part::id_password,
                u->pass->str.size() + 2);
        else
            p.resize(
                part::id_password, 1);
    }
    else
    {
        p.resize(
            part::id_password, 1);
    }
    auto const& h =
        t->host;
    if(h.kind() != rfc::host_kind::none)
        p.resize(
            part::id_host,
            h.str().size());
    if(t->port.has_value())
        p.resize(
            part::id_port,
            t->port->str.size() + 1);
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

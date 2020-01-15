//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_VALUE_IPP
#define BOOST_URL_IMPL_VALUE_IPP

#include <boost/url/scheme.hpp>
#include <boost/url/value.hpp>
#include <boost/url/detail/parser.hpp>
#include <stdexcept>

namespace boost {
namespace url {

value::
~value()
{
    if(begin_)
        ::operator delete(begin_);
}

//----------------------------------------------------------

value::
value(string_view s)
{
    set_encoded_uri_reference(s);
}

//----------------------------------------------------------

value&
value::
set_encoded_uri_reference(
    string_view s)
{
    error_code ec;
    detail::parser pr(s);
    pr.parse_url(ec);
    if(ec)
        parse_error::raise();

    reserve(
        s.size(),
        id_path +
            (pr.nseg ? pr.nseg : 1) +
            2 * pr.nparam +
            1
    );
    nseg_ = static_cast<
        unsigned>(pr.nseg);
    nparam_ = static_cast<
        unsigned>(pr.nparam);
    for(id_type id = 2;
        id <= id_end(); ++id)
        offset(id) = static_cast<
            size_type>(s.size());
    s.copy(begin_, s.size());
    split(id_scheme, pr.nscheme);
    split(id_username, pr.nuser);
    split(id_password, pr.npass);
    split(id_hostname, pr.nhost);
    split(id_port, pr.nport);
    pr.write_segments(&offset(id_path));
    if(nparam_ > 0)
        pr.write_params(&offset(id_query()));
    BOOST_ASSERT(
        length(id_fragment()) == pr.nfrag);
    offset(id_end()) =
        static_cast<size_type>(s.size());
    null_term();
    port_ = pr.port;

    return *this;
}

value&
value::
set_encoded_origin(
    string_view s)
{
    if(s.empty())
    {
        erase(
            id_scheme,
            id_path);
        port_ = {};
        return *this;
    }

    error_code ec;
    detail::parser pr(s);
    pr.parse_origin(ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();

    maybe_init();
    merge(id_scheme, id_path);
    s.copy(resize(
        id_scheme, s.size()),
        s.size());
    split(id_scheme, pr.nscheme);
    split(id_username, pr.nuser);
    split(id_password, pr.npass);
    split(id_hostname, pr.nhost);
    split(id_port, pr.nport);
    port_ = pr.port;
    return *this;
}

//----------------------------------------------------------
//
// scheme
//
//----------------------------------------------------------

value&
value::
set_scheme(
    string_view s)
{
    if(s.empty())
    {
        erase(id_scheme);
        return *this;
    }

    detail::parser pr(s);
    pr.parse_scheme();

    maybe_init();
    auto const n = s.size();
    auto const dest =
        resize(id_scheme, n + 1);
    s.copy(dest, n);
    dest[n] = ':';
    return *this;
}

//----------------------------------------------------------
//
// authority
//
//----------------------------------------------------------

value&
value::
set_encoded_authority(
    string_view s)
{
    if(s.empty())
    {
        erase(
            id_username,
            id_path);
        port_ = {};
        return *this;
    }

    detail::parser pr(s);
    pr.parse_authority();

    maybe_init();
    merge(id_username, id_path);
    auto const dest = resize(
        id_username, 2 + s.size());
    dest[0] = '/';
    dest[1] = '/';
    s.copy(dest + 2, s.size());
    split(id_username, 2 + pr.nuser);
    split(id_password, pr.npass);
    split(id_hostname, pr.nhost);
    BOOST_ASSERT(length(id_port) == pr.nport);
    port_ = pr.port;
    return *this;
}

//
// userinfo
//

value&
value::
set_encoded_userinfo(
    string_view s)
{
    if(s.empty())
    {
        if(is_reset())
            return *this;
        if(length(
            id_hostname,
            id_path) == 0)
        {
            // no authority
            erase(
                id_username,
                id_hostname);
            return *this;
        }
        // keep "//"
        merge(id_username, id_hostname);
        resize(id_username, 2);
        return *this;
    }

    error_code ec;
    auto e =
        detail::userinfo_nc_pct_set();
    auto p = e.parse(
        s.begin(), s.end(), ec);
    if(ec)
        invalid_part::raise();
    auto const n0 = static_cast<
        std::size_t>(p - s.begin());
    e = detail::userinfo_pct_set();
    p = e.parse(p, s.end(), ec);
    if(ec)
        invalid_part::raise();
    if(p != s.end())
        invalid_part::raise();

    maybe_init();
    merge(id_username, id_hostname);
    auto dest = resize(
        id_username, 2 + s.size() + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest += 2;
    s.copy(dest, s.size());
    split(id_username, n0);
    dest[s.size()] = '@';
    return *this;
}

value&
value::
set_username(
    string_view s)
{
    if(s.empty())
    {
        if(is_reset())
            return *this;
        if(length(id_username) == 0)
            return *this;
        BOOST_ASSERT(get(id_password).back() == '@');
        BOOST_ASSERT(get(id_username).size() >= 2);
        BOOST_ASSERT(get(id_username)[0] == '/');
        BOOST_ASSERT(get(id_username)[1] == '/');
        if(length(id_password) == 1)
        {
            // remove '@'
            merge(
                id_username,
                id_hostname);
        }
        resize(id_username, 2);
        return *this;
    }

    maybe_init();
    auto const e =
        detail::userinfo_nc_pct_set();
    if(! empty(id_password))
    {
        BOOST_ASSERT(get(
            id_password).back() == '@');
        // preserve "//"
        auto const dest = resize(
            id_username,
            2 + e.encoded_size(s));
        e.encode(dest + 2, s);
        return *this;
    }
    auto const n = e.encoded_size(s);
    auto const dest = resize(
        id_username, 2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    split(id_username, 2 + n);
    e.encode(dest + 2, s);
    return *this;
}

value&
value::
set_encoded_username(
    string_view s)
{
    if(s.empty())
        return set_username(s);

    auto const e =
        detail::userinfo_nc_pct_set();
    e.validate(s);

    maybe_init();
    auto const n = s.size();
    if(! empty(id_password))
    {
        BOOST_ASSERT(get(
            id_password).back() == '@');
        // preserve "//"
        auto const dest = resize(
            id_username, 2 + n);
        s.copy(dest + 2, n);
        return *this;
    }

    // add '@'
    auto const dest = resize(
        id_username, 2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    split(id_username, 2 + n);
    s.copy(dest + 2, n);
    return *this;
}

value&
value::
set_password(
    string_view s)
{
    if(s.empty())
    {
        if(is_reset())
            return *this;
        auto const n =
            length(id_password);
        if(n == 0)
            return *this;
        BOOST_ASSERT(get(id_password).back() == '@');
        BOOST_ASSERT(get(id_username).size() >= 2);
        BOOST_ASSERT(get(id_username)[0] == '/');
        BOOST_ASSERT(get(id_username)[1] == '/');
        if(length(id_username) == 2)
        {
            // remove '@'
            erase(id_password);
            return *this;
        }
        *resize(id_password, 1) = '@';
        return *this;
    }

    maybe_init();
    auto const e =
        detail::userinfo_pct_set();
    auto const n =
        e.encoded_size(s);
    if(length(id_username) != 0)
    {
        auto const dest = resize(
            id_password, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        e.encode(dest + 1, s);
        return *this;
    }
    merge(id_username, id_hostname);
    auto const dest = resize(
        id_username, 2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    e.encode(dest + 3, s);
    split(id_username, 2);
    return *this;
}

value&
value::
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

    maybe_init();
    auto const n = s.size();
    if(length(id_username) != 0)
    {
        auto const dest = resize(
            id_password, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        s.copy(dest + 1, n);
        return *this;
    }
    merge(id_username, id_hostname);
    auto const dest = resize(
        id_username, 2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    s.copy(dest + 3, n);
    split(id_username, 2);
    return *this;
}

//
// host
//

value&
value::
set_encoded_host(
    string_view s)
{
    if(s.empty())
    {
        erase(
            id_hostname,
            id_path);
        port_ = {};
        return *this;
    }

    error_code ec;
    detail::parser pr(s);
    pr.parse_host(ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
    BOOST_ASSERT(s.size() ==
        pr.nhost + pr.nport);

    maybe_init();
    if(length(
        id_username,
        id_path) == 0)
    {
        // add authority
        auto const dest = resize(
            id_username,
            2 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        s.copy(dest + 2, s.size());
        split(id_username, 2);
        split(id_password, 0);
        split(id_hostname, pr.nhost);
    }
    else
    {
        merge(id_hostname, id_path);
        auto const dest = resize(
            id_hostname, s.size());
        s.copy(dest, s.size());
        split(id_hostname, pr.nhost);
    }
    BOOST_ASSERT(
        length(id_port) == pr.nport);
    port_ = pr.port;
    return *this;
}

value&
value::
set_hostname(
    string_view s)
{
    if(s.empty())
    {
        if(is_reset())
            return *this;
        erase(id_hostname);
        return *this;
    }

    maybe_init();
    auto const e =
        detail::reg_name_pct_set();
    e.encode(resize(
        id_hostname,
        e.encoded_size(s)), s);
    return *this;
}

value&
value::
set_encoded_hostname(
    string_view s)
{
    if(s.empty())
        return set_hostname(s);

    maybe_init();
    auto const e =
        detail::reg_name_pct_set();
    e.validate(s);
    s.copy(resize(
        id_hostname, s.size()),
        s.size());
    return *this;
}

value&
value::
set_port(
    optional<unsigned short> num)
{
    if(! num.has_value())
    {
        if(is_reset())
            return *this;
        resize(id_port, 0);
        port_ = {};
        return *this;
    }

    maybe_init();
    detail::port_string s(*num);
    if(length(
        id_username,
        id_path) == 0)
    {
        // add authority
        auto const dest = resize(
            id_username,
            3 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        dest[2] = ':';
        split(id_username, 2);
        split(id_password, 0);
        split(id_hostname, 0);
        s.get().copy(
            dest + 3, s.size());
    }
    else
    {
        auto const dest = resize(
            id_port, 1 + s.size());
        dest[0] = ':';
        s.get().copy(
            dest + 1, s.size());
    }
    port_ = num;
    return *this;
}

value&
value::
set_port_string(string_view s)
{
    if(s.empty())
        return set_port({});

    error_code ec;
    detail::parser pr(s);
    pr.parse_port(ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
    set_port(pr.port);
    return *this;
}

//----------------------------------------------------------
//
// path
//
//----------------------------------------------------------

value&
value::
set_encoded_path(
    string_view s)
{
    if(s.empty())
    {
        if(! is_reset())
            resize_and_merge(
                id_path, id_query(),
                0, 1);
        return *this;
    }

    error_code ec;
    detail::parser pr(s);
    pr.parse_path_abempty(ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();

    maybe_init();
    auto dest = resize_and_merge(
        id_path, id_query(),
        s.size(), pr.nseg);
    s.copy(dest, s.size());
    auto const e =
        detail::pchar_pct_set();
    auto p = s.begin();
    auto p0 = p;
    for(id_type i = id_path;
        i < id_path + pr.nseg; ++i)
    {
        BOOST_ASSERT(
            p < s.end() && *p0 == '/');
        p = e.parse(
            p0 + 1, s.end(), ec);
        split(i, p - p0);
        p0 = p;
    }
    return *this;
}

value&
value::
set_segment(
    int index,
    string_view s)
{
    auto const id =
        id_segment(index);
    auto const e =
        detail::pchar_pct_set();
    auto const n =
        e.encoded_size(s);
    auto const dest = resize(
        id, 1 + n);
    dest[0] = '/';
    e.encode(dest + 1, s);
    return *this;
}

value&
value::
set_encoded_segment(
    int index,
    string_view s)
{
    auto const id =
        id_segment(index);
    error_code ec;
    auto const e =
        detail::pchar_pct_set();
    auto p = e.parse(
        s.begin(), s.end(), ec);
    if(ec || p != s.end())
        invalid_part::raise();
    auto const dest = resize(
        id, 1 + s.size());
    dest[0] = '/';
    s.copy(dest + 1, s.size());
    return *this;
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

value&
value::
set_fragment(
    string_view s)
{
    if(s.empty())
    {
        erase(id_fragment());
        return *this;
    }

    if(s.front() == '#')
        s = s.substr(1);
    auto const e =
        detail::frag_pct_set();
    auto const n =
        e.encoded_size(s);

    auto const dest = resize(
        id_fragment(), 1 + n);
    dest[0] = '#';
    e.encode(dest + 1, s);
    return *this;
}

value&
value::
set_encoded_fragment(
    string_view s)
{
    if(s.empty())
    {
        erase(id_fragment());
        return *this;
    }

    if(s.front() == '#')
        s = s.substr(1);
    auto const e =
        detail::frag_pct_set();
    e.validate(s);

    auto const dest = resize(
        id_fragment(),
        1 + s.size());
    dest[0] = '#';
    s.copy(dest + 1, s.size());
    return *this;
}

//----------------------------------------------------------
//
// implementation
//
//----------------------------------------------------------

// Returns the upper limit on the
// number of allocated bytes.
constexpr
std::size_t
value::
max_size() noexcept
{
    return size_type(-1);
}

// Returns the number of characters used
std::size_t
value::
size() const noexcept
{
    return offset(id_end());
}

// Returns the size of the allocated storage
std::size_t
value::
capacity() const noexcept
{
    return
        std::uintptr_t(end_) -
        std::uintptr_t(begin_);
}


// returns offset ref of piece
auto
value::
offset(id_type id) noexcept ->
    size_type&
{
    return *(end_ - id);
}

void
value::null_term() noexcept
{
    BOOST_ASSERT(
        begin_ + size() <
        reinterpret_cast<
            char const*>(&offset(id_end())));
    begin_[size()] = 0;
}

// merge range [first, last) into first
void
value::
merge(
    id_type first,
    id_type last) noexcept
{
    BOOST_ASSERT(first <= last);
    BOOST_ASSERT(last <= id_end());
    auto const pos = offset(last);
    for(id_type i = first + 1;
        i < last; ++i)
        offset(i) = pos;
}

// split suffix into id+1
// n == amount to keep
void
value::
split(
    id_type id,
    std::size_t n) noexcept
{
    BOOST_ASSERT(id < id_end() - 1);
    BOOST_ASSERT(n <= length(id));
    offset(id + 1) = offset(id) +
        static_cast<size_type>(n);
}

void
value::
maybe_init()
{
    if(begin_)
        return;
    auto const min_chars = 256;
    auto const min_pieces = id_end() + 10;
    auto const capacity =
        min_pieces + (
        min_chars + sizeof(size_type) - 1)
            / sizeof(size_type);
    auto const begin =
        reinterpret_cast<
        size_type*>(::operator new(
        capacity * sizeof(size_type)));
    end_ = begin + capacity;
    std::memset(
        end_ - id_end(),
        0,
        id_end() * sizeof(size_type));
    begin_ = reinterpret_cast<
        char*>(begin);
    begin_[0] = 0; // null term
    end_ = begin + capacity;
    view_begin_ = begin_;
    view_end_ = end_;
}

void
value::
reserve(
    std::size_t char_cap,
    std::size_t id_cap)
{
    // VFALCO HACK
    ++id_cap; // one extra, for id_end()

    // don't shrink
    if( ! is_reset() &&
        char_cap < offset(id_end()))
        char_cap = offset(id_end());
    if( id_cap < id_end())
        id_cap = id_end();

    // enforce size maximums
    auto const limit = max_size()
        // for rounding up
        - sizeof(size_type) - 1
        // for null term
        - 1;
    if( id_cap > limit /
        sizeof(size_type))
        too_large::raise();
    if( char_cap > limit -
        id_cap * sizeof(size_type))
        too_large::raise();

    // calculate bytes needed
    auto needed =
        sizeof(size_type) * (
            id_cap + ((
                char_cap +
                1 + // null term
                sizeof(size_type) - 1) /
            sizeof(size_type)));

    // don't shrink
    auto const cap = capacity();
    if( needed <= cap)
        return;

    // enforce size minimums
    size_type const min_size = 64;
    if( needed < min_size)
    {
        needed = sizeof(size_type) *
            (min_size / sizeof(size_type));
    }
    else if(
        cap <= max_size() - cap)
    {
        // 2x growth factor
        if( needed < cap * 2)
            needed = cap * 2;
    }
    else
    {
        needed = sizeof(size_type) *
            (max_size() / sizeof(size_type));
    }

    // allocate
    auto const begin =
        reinterpret_cast<char*>(
            ::operator new(needed));
    auto const end = 
        reinterpret_cast<
            size_type*>(begin + needed);
    if(! begin_)
    {
        // first time
        std::memset(
            end - id_end(),
            0,
            id_end() * sizeof(size_type));
    }
    else
    {
        // copy chars (and null)
        std::memcpy(
            begin,
            begin_,
            offset(id_end()) + 1);
        // copy offsets
        std::memcpy(
            end - id_end(),
            &offset(id_end()),
            id_end() * sizeof(size_type));
        ::operator delete(begin_);
    }

    begin_ = begin;
    end_ = end;
    view_begin_ = begin_;
    view_end_ = end_;
}

void
value::
reserve(
    std::size_t capacity)
{
    reserve(
        capacity,
        id_end());
}

char*
value::
resize(
    id_type id,
    std::size_t new_size)
{
    auto const end = id_end();
    BOOST_ASSERT(id < end);

    maybe_init();
    auto const len = length(id);
    if(new_size <= len)
    {
        // shrink
        auto const n = static_cast<
            size_type>(len - new_size);
        auto const pos = offset(id + 1);
        std::memmove(
            begin_ + pos - n,
            begin_ + pos,
            offset(end) - pos + 1);
        for(auto i = id + 1; i <= end; ++i)
            offset(i) -= n;
        return begin_ + offset(id);
    }

    // grow
    if( new_size - len >
        max_size() - size())
        too_large::raise();
    reserve(size() - len + new_size);
    auto const n = static_cast<
        size_type>(new_size - len);
    auto const pos = offset(id + 1);
    std::memmove(
        begin_ + pos + n,
        begin_ + pos,
        offset(end) - pos + 1);
    for(auto i = id + 1;
        i <= end; ++i)
        offset(i) += n;
    return begin_ + offset(id);
}

char*
value::
resize_and_merge(
    id_type first,
    id_type last,
    std::size_t nchar,
    std::size_t nid)
{
    BOOST_ASSERT(first <= last);
    BOOST_ASSERT(last <= id_end());
    BOOST_ASSERT(nid >= 1);
    auto const nchar0 =
        length(first, last);
    if( nchar > max_size() -
        size() - nchar0)
        too_large::raise();
    auto const nid0 = last - first;
    if( nid > max_size() -
        id_end() - nid0)
        too_large::raise();
    reserve(
        size() - nchar0 + nchar,
        id_end() - nid0 + nid);
    auto const pos =
        offset(first) + nchar;
    // VFALCO What if there's nothing to move?
    std::memmove(
        begin_ + pos,
        begin_ + offset(last),
        size() - offset(last) + 1);
    std::memmove(
        &offset(id_end() - nid0 +
            static_cast<id_type>(nid)),
        &offset(id_end()),
        (id_end() - last) *
            sizeof(size_type));
    if(last <= id_path)
    {
        // VFALCO ??
    }
    else if(last <= id_query())
    {
        BOOST_ASSERT(
            first >= id_path);
        BOOST_ASSERT(
            nseg_ >= nid0);
        nseg_ += static_cast<
            id_type>(nid - nid0);
    }
    else
    {
        BOOST_ASSERT(
            last <= id_fragment());
        BOOST_ASSERT(
            first >= id_query());
        BOOST_ASSERT(
            nparam_ >= nid0);
        nparam_ += static_cast<
            id_type>(nid - nid0);
    }
    last = static_cast<
        id_type>(first + nid);
    if(nchar >= nchar0)
    {
        auto const d =
            static_cast<size_type>(
                nchar - nchar0);
        for(auto id = last;
            id <= id_end();
            ++id)
            offset(id) += d;
    }
    else
    {
        auto const d =
            static_cast<size_type>(
                nchar0 - nchar);
        for(auto id = last;
            id <= id_end();
            ++id)
            offset(id) -= d;
    }
    for(auto id = first + 1;
        id < last; ++id)
        offset(id) = offset(last);
    return begin_ + offset(first);
}

void
value::
erase(id_type id) noexcept
{
    if(is_reset())
        return;
    resize(id, 0);
}

void
value::
erase(
    id_type first,
    id_type last) noexcept
{
    BOOST_ASSERT(first <= last);
    BOOST_ASSERT(last <= id_end());
    if(is_reset())
        return;
    merge(first, last);
    resize(first, 0);
}

} // url
} // boost

#endif

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
#include <boost/url/detail/parse.hpp>
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

struct value::write_parts
{
    value& v_;

    void
    on_scheme(string_view s) noexcept
    {
        BOOST_ASSERT(v_.length(
            id_scheme) == s.size());
        std::memcpy(
            v_.begin_ + v_.offset(id_scheme),
            s.data(), s.size());
    }

    void
    on_hostname(string_view) noexcept
    {
    }
};

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
    detail::measure_cb m;
    error_code ec;
    detail::parse_scheme(s, ec, m);
    if(ec)
        BOOST_THROW_EXCEPTION(
            system_error(ec));
    resize(id_scheme, m.char_count());
    detail::parse_scheme(
        s, ec, write_parts{*this});
    return *this;
}

//----------------------------------------------------------

value&
value::
set_username(
    string_view s)
{
    // username can't have ':'
    if(s.find_first_of(':') !=
        string_view::npos)
        BOOST_THROW_EXCEPTION(
            system_error(
                error::bad_username_char));
    set(id_username, s);
    return *this;
}

value&
value::
set_encoded_username(
    string_view s)
{
    // encoded username can't have ':'
    set_encoded(id_username, s,
        detail::username_pct_set());
    return *this;
}

value&
value::
set_password(
    string_view s)
{
    set(id_password, s);
    return *this;
}

value&
value::
set_encoded_password(
    string_view s)
{
    set_encoded(id_password, s,
        detail::userinfo_pct_set());
    return *this;
}

value&
value::
set_encoded_userinfo(
    string_view s)
{
    if(s.empty())
    {
        if(! is_reset())
        {
            merge(id_username, id_hostname);
            erase(id_username);
        }
        return *this;
    }
    error_code ec;
    auto const e =
        detail::userinfo_pct_set();
    auto new_size =
        e.decoded_size(s, ec);
    if(ec)
        BOOST_THROW_EXCEPTION(
            system_error(ec));
    maybe_init();
    auto const cur_size =
        length(id_username) +
        length(id_password);
    auto const pos =
        s.find_first_of(':');
    BOOST_ASSERT(s.size() > 0);
    if( pos == string_view::npos ||
        pos == s.size() - 1)
    {
        if(new_size > cur_size)
        {
            if( new_size - cur_size >
                max_size() - capacity())
                BOOST_THROW_EXCEPTION(
                    std::length_error(
                        "too long"));
            reserve(
                offset(id_end()) +
                (new_size - cur_size));
        }

        // no more exceptions
        merge(id_username, id_hostname);
        e.decode(resize(
            id_username, new_size), s);
    }
    else
    {
        BOOST_ASSERT(new_size > 0);
        --new_size; // remove colon
        if(new_size > cur_size)
        {
            if( new_size - cur_size >
                max_size() - capacity())
                BOOST_THROW_EXCEPTION(
                    std::length_error(
                        "too long"));
            reserve(
                offset(id_end()) +
                (new_size - cur_size));
        }

        // no more exceptions
        merge(id_username, id_hostname);
        auto p0 = resize(
            id_username, new_size);
        auto p = e.decode(
            p0, s.substr(0, pos));
        split(id_username, static_cast<
            size_type>(p - p0));
        e.decode(p, s.substr(pos + 1));
    }
    return *this;
}

//----------------------------------------------------------

value&
value::
set_hostname(
    string_view s)
{
    set(id_hostname, s);
    return *this;
}

value&
value::
set_encoded_hostname(
    string_view s)
{
    set_encoded(id_hostname, s,
        detail::reg_name_pct_set());
    return *this;
}

value&
value::
set_port(
    optional<unsigned short> num)
{
    port_ = num;
    return *this;
}

value&
value::
set_port_string(string_view s)
{
    error_code ec;
    auto const num =
        detail::parse_port(s, ec);
    if(ec)
        BOOST_THROW_EXCEPTION(
            system_error(ec));
    port_ = num;
    return *this;
}

value&
value::
set_encoded_host(
    string_view s)
{
    auto const pos =
        s.find_first_of(':');
    if(pos == 0)
        BOOST_THROW_EXCEPTION(system_error(
            error::missing_hostname));
    error_code ec;
    if(pos != string_view::npos)
    {
        string_view p = s.substr(pos + 1);
        if(p.empty())
            BOOST_THROW_EXCEPTION(system_error(
                error::missing_port));
        auto const po =
            detail::parse_port(p, ec);
        if(ec)
            BOOST_THROW_EXCEPTION(
                system_error(ec));
        set_encoded(id_hostname,
            s.substr(0, pos),
            detail::reg_name_pct_set());
        port_ = po;
    }
    else
    {
        set_encoded(id_hostname, s,
            detail::reg_name_pct_set());
        port_ = {};
    }
    return *this;
}

value&
value::
set_encoded_authority(
    string_view)
{
    return *this;
}

//----------------------------------------------------------

// returns offset ref of piece
auto
value::
offset(id_type id) noexcept ->
    size_type&
{
    return *(end_ - id);
}

// Returns the number of characters used
std::size_t
value::
size() const noexcept
{
    return offset(id_end());
}

// Returns the maximum number of characters
// which may be used before a reallocation
std::size_t
value::
capacity() const noexcept
{
    return
        std::uintptr_t(end_ - id_end()) -
        std::uintptr_t(begin_);
}

// merge range [first, last) into first
void
value::
merge(
    id_type first,
    id_type last) noexcept
{
    BOOST_ASSERT(first < last);
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

// split suffix into id+1
// n == amount to move
void
value::
split_suffix(
    id_type id,
    std::size_t n) noexcept
{
    BOOST_ASSERT(id < id_end() - 1);
    BOOST_ASSERT(n <= length(id));
    offset(id + 1) -=
        static_cast<size_type>(n);
}

void
value::
maybe_init()
{
    if(begin_)
        return;
    auto const min_chars = 128;
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
    capacity_ = capacity;
    begin_ = reinterpret_cast<
        char*>(begin);
    end_ = begin + capacity_;
    view_begin_ = begin_;
    view_end_ = end_;
}

void
value::
reserve(
    std::size_t,
    id_type)
{
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

void
value::
realloc(size_type capacity)
{
    if( capacity < id_end())
        capacity = id_end();
    auto const begin = reinterpret_cast<
        size_type*>(::operator new(
        capacity * sizeof(size_type)));
    if(! begin_)
    {
        // VFALCO memset?
        for(id_type i = 1;
            i <= id_end(); ++i)
            offset(i) = 0;
    }
    else
    {
        // VFALCO TODO Use two memcpy here
        std::memcpy(begin, begin_,
            capacity_ * sizeof(size_type));
        ::operator delete(begin_);
    }
    capacity_ = capacity;
    begin_ = reinterpret_cast<
        char*>(begin);
    end_ = begin + capacity_;
    view_begin_ = begin_;
    view_end_ = end_;
}

char*
value::
resize(
    id_type id,
    std::size_t new_size)
{
    maybe_init();
#if 0
    if(new_size > length(id))
    {
        if( new_size - cur_size >
            max_size() - capacity())
            BOOST_THROW_EXCEPTION(
                std::length_error(
                    "too long"));
        reserve(
            offset(id_end()) +
            (new_size - cur_size));
    }
#endif
    auto const end = id_end();
    BOOST_ASSERT(id < end);
    //auto const pos = offset(id);
    auto const len = length(id);

    // shrink
    if(new_size <= len)
    {
        auto const n = static_cast<
            size_type>(len - new_size);
        if(id != end - 1)
        {
            std::memmove(
                begin_ + offset(id + 1) - n,
                begin_ + offset(id + 1),
                offset(id_end()) - offset(id + 1));
            for(auto i = id + 1;
                i <= end; ++i)
                offset(i) -= n;
        }
        return begin_ + offset(id);
    }

    // VFALCO MISSING CALL TO REALLOC!!!!

    // grow
    auto const n = static_cast<
        size_type>(new_size - len);
    if(id != end - 1)
    {
        std::memmove(
            begin_ + offset(id + 1) + n,
            begin_ + offset(id + 1),
            offset(id_end()) - offset(id + 1));
            for(auto i = id + 1;
                i <= end; ++i)
                offset(i) += n;
    }
    return begin_ + offset(id);
}

void
value::
set(
    id_type id,
    string_view s)
{
    if(! s.empty())
    {
        maybe_init();
        std::memcpy(
            resize(id, s.size()),
            s.data(), s.size());
    }
    else if(! is_reset())
    {
        erase(id);
    }
}

void
value::
set_encoded(
    id_type id,
    string_view s,
    detail::pct_encoding e)
{
    if(! s.empty())
    {
        maybe_init();
        error_code ec;
        auto const n =
            e.decoded_size(s, ec);
        if(ec)
            BOOST_THROW_EXCEPTION(
                system_error(ec));
        e.decode(resize(id, n), s);
    }
    else if(! is_reset())
    {
        erase(id);
    }
}

void
value::
erase(id_type id) noexcept
{
    resize(id, 0);
}

} // url
} // boost

#endif

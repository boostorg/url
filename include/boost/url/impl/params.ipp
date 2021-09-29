//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_IPP
#define BOOST_URL_IMPL_PARAMS_IPP

#include <boost/url/params.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

params::
reference::
reference(
    char const* const s,
    std::size_t const nk,
    std::size_t const nv,
    string_value::allocator a)
{
    if(nv > 0)
    {
        // value
        BOOST_ASSERT(s[nk] == '=');
        has_value = true;
        string_view ev{
            s + nk + 1, nv - 1 };
        auto n = detail::
            pct_decode_size_unchecked(ev);
        char *dest;
        value = a.make_string_value(
            n, dest);
        detail::pct_decode_unchecked(
            dest, dest + n, ev, {});
    }
    else
    {
        has_value = false;
    }
    // key
    BOOST_ASSERT(nk > 0);
    BOOST_ASSERT(
        s[0] == '?' || s[0] == '&');
    string_view ek{s + 1, nk - 1};
    auto n = detail::
        pct_decode_size_unchecked(ek);
    char* dest;
    key = a.make_string_value(n, dest);
    detail::pct_decode_unchecked(
        dest, dest + nk, ek, {});
}

auto
params::
operator[](
    std::size_t pos) const ->
        reference
{
    auto const r =
        u_->get_param(pos);
    return reference(
        u_->s_ + r.pos,
            r.nk, r.nv, a_);
}

//------------------------------------------------

auto
params::
iterator::
operator*() const ->
    reference
{
    auto const r =
        u_->get_param(i_);
    return reference(
        u_->s_ + r.pos,
        r.nk,
        r.nv,
        a_);
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

std::size_t
params::
erase(string_view key) noexcept
{
    std::size_t n = 0;
    auto it = find(key);
    while(it != end())
    {
        ++n;
        it = erase(it);
        it = find(it, key);
    }
    return n;
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

std::size_t
params::
count(string_view key) const noexcept
{
    std::size_t n = 0;
    auto it = find(key);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        ++it;
        it = find(it, key);
    }
    return n;
}

auto
params::
find(
    iterator from,
    string_view key) const noexcept ->
        iterator
{
    BOOST_ASSERT(from.u_ == u_);
    auto const end_ = end();
    while(from != end_)
    {
        auto r = u_->get_param(
            from.i_);
        if( detail::key_equal_encoded(
            key, string_view(u_->s_ +
            r.pos + 1, r.nk - 1)))
            break;
        ++from;
    }
    return from;
}


//------------------------------------------------

} // urls
} // boost

#endif

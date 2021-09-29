//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_IPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_IPP

#include <boost/url/params_encoded.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

auto
params_encoded::
operator[](
    std::size_t pos) const ->
        reference
{
    auto const r = u_->get_param(pos);
    if(r.nv > 0)
        return reference{
            string_view(
                u_->s_ + r.pos + 1,
                r.nk - 1),
            string_view(
                u_->s_ + r.pos + r.nk + 1,
                r.nv - 1),
            true};
    return reference{
        string_view(
            u_->s_ + r.pos + 1,
            r.nk - 1),
        string_view{},
        false};
}

//------------------------------------------------

auto
params_encoded::
iterator::
operator*() const ->
    reference
{
    auto const r = u_->get_param(i_);
    if(r.nv > 0)
        return reference{
            string_view(
                u_->s_ + r.pos + 1,
                r.nk - 1),
            string_view(
                u_->s_ + r.pos + r.nk + 1,
                r.nv - 1),
            true};
    return reference{
        string_view(
            u_->s_ + r.pos + 1,
            r.nk - 1),
        string_view{},
        false};
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

std::size_t
params_encoded::
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
params_encoded::
count(string_view key) const noexcept
{
    std::size_t n = 0;
    auto it = find(key);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        it = find(it + 1, key);
    }
    return n;
}

auto
params_encoded::
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

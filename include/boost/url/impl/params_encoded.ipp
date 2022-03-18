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
        value_type
{
    BOOST_ASSERT(pos < u_->nparam_);
    auto const r = u_->param(pos);
    if(r.nv > 0)
        return value_type{
            string_view(
                u_->s_ + r.pos + 1,
                r.nk - 1),
            string_view(
                u_->s_ + r.pos + r.nk + 1,
                r.nv - 1),
            true};
    return value_type{
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
    BOOST_ASSERT(i_ < u_->nparam_);
    auto const r = u_->param(i_);
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
// Element Access
//
//------------------------------------------------

auto
params_encoded::
at(string_view key) const ->
    string_view
{
    url::raw_param r;
    auto it = find(key);
    for(;;)
    {
        if(it == end())
            detail::throw_out_of_range(
                BOOST_CURRENT_LOCATION);
        r = u_->param(it.i_);
        if(r.nv != 0)
            break;
        ++it;
        it = find(it, key);
    }
    return {
        u_->s_ + r.pos +
            r.nk + 1,
        r.nv - 1 };
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

auto
params_encoded::
remove_value(
    iterator pos) ->
        iterator
{
    BOOST_ASSERT(pos.u_ == u_);
    using detail::
        make_enc_params_iter;
    auto r = u_->param(pos.i_);
    value_type v{
        string_view{
            u_->s_ + r.pos + 1,
            r.nk - 1},
        string_view{},
        false};
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        make_enc_params_iter(
            &v, &v + 1),
        make_enc_params_iter(
            &v, &v + 1));
    return pos;
}

auto
params_encoded::
replace_value(
    iterator pos,
    string_view value) ->
        iterator
{
    return emplace_at(
        pos,
        (*pos).key,
        value);
}

auto
params_encoded::
erase(
    iterator first,
    iterator last) ->
        iterator
{
    BOOST_ASSERT(first.u_ == u_);
    BOOST_ASSERT(last.u_ == u_);
    string_view s;
    u_->edit_params(
        first.i_,
        last.i_,
        detail::enc_query_iter(s),
        detail::enc_query_iter(s));
    return first;
}

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
        ++it;
        it = find(it, key);
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
        auto r = u_->param(
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

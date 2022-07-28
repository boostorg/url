//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
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
#include <boost/assert.hpp>

namespace boost {
namespace urls {

auto
params_encoded::
operator[](
    std::size_t pos) const ->
        reference
{
    BOOST_ASSERT(pos < u_->nparam_);
    auto const r = u_->param(pos);
    if(r.nv > 0)
        return {
            string_view(
                u_->s_ + r.pos + 1,
                r.nk - 1),
            string_view(
                u_->s_ + r.pos + r.nk + 1,
                r.nv - 1),
            true};
    return {
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
    query_param_encoded_view r;
    auto it = find(key);
    for(;;)
    {
        if(it == end())
            detail::throw_out_of_range(
                BOOST_CURRENT_LOCATION);
        r = *it;
        if(r.has_value)
            break;
        ++it;
        it = find(it, key);
    }
    return r.value;
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
    BOOST_ASSERT(pos.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(pos.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    using detail::
        make_enc_params_iter;
    auto r = u_->param(pos.impl_.i_);
    reference v{
        string_view{
            u_->s_ + r.pos + 1,
            r.nk - 1},
        string_view{},
        false};
    u_->edit_params(
        pos.impl_.i_,
        pos.impl_.i_ + 1,
        make_enc_params_iter(
            &v, &v + 1),
        make_enc_params_iter(
            &v, &v + 1));
    return std::next(begin(), pos.impl_.i_);
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
    BOOST_ASSERT(first.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(first.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    BOOST_ASSERT(last.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(last.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    string_view s;
    u_->edit_params(
        first.impl_.i_,
        last.impl_.i_,
        detail::enc_query_iter(s),
        detail::enc_query_iter(s));
    return std::next(begin(), first.impl_.i_);
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
    BOOST_ASSERT(from.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(from.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());

    auto const end_ = end();
    while(from != end_)
    {
        if (pct_encoded_view(from.encoded_key()) == key)
            break;
        ++from;
    }
    return from;
}

//------------------------------------------------

} // urls
} // boost

#endif

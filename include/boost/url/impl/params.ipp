//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
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
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

auto
params::
remove_value(
    iterator pos) ->
        iterator
{
    BOOST_ASSERT(pos.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(pos.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    auto r = u_->param(pos.impl_.i_);
    query_param_view v{
        decode_view(string_view{
            u_->s_ + r.pos + 1,
            r.nk - 1}),
        decode_view{},
        false};
    u_->edit_params(
        pos.impl_.i_,
        pos.impl_.i_ + 1,
        detail::make_enc_params_iter(
            &v, &v + 1),
        detail::make_enc_params_iter(
            &v, &v + 1));
    return std::next(begin(), pos.impl_.i_);
}

auto
params::
replace_value(
    iterator pos,
    string_view value) ->
        iterator
{
    auto const r =
        u_->param(pos.impl_.i_);
    string_view key{
        u_->s_ + r.pos + 1,
        r.nk - 1};
    query_param_view v{
        decode_view(key),
        decode_view(value),
        true };
    BOOST_ASSERT(pos.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(pos.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    using detail::
        make_plain_value_iter;
    u_->edit_params(
        pos.impl_.i_,
        pos.impl_.i_ + 1,
        make_plain_value_iter(
            &v, &v + 1),
        make_plain_value_iter(
            &v, &v + 1));
    return std::next(begin(), pos.impl_.i_);
}

auto
params::
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
    BOOST_ASSERT(from.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(from.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());

    auto const end_ = end();
    while(from != end_)
    {
        auto r = *from;
        if( r.key == key )
            break;
        ++from;
    }
    return from;
}

} // urls
} // boost

#endif

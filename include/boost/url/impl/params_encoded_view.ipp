//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_IPP

#include <boost/url/params_encoded_view.hpp>
#include <boost/url/url_base.hpp>
#include <boost/url/grammar/ci_string.hpp>
#include <boost/assert.hpp>
#include <utility>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

std::size_t
params_encoded_view::
count(
    string_view key,
    ignore_case_param ic) const
{
    std::size_t n = 0;
    auto it = find(key, ic);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        ++it;
        it = find(
            it, key, ic);
    }
    return n;
}

auto
params_encoded_view::
find(
    iterator it,
    string_view key,
    ignore_case_param ic) const ->
        iterator
{
    auto const dk = decode_view(key);
    auto const end_ = end();
    if(! ic)
    {
        for(;;)
        {
            if(it == end_)
                return it;
            auto r = *it;
            if( decode_view(r.key) == dk)
                return it;
            ++it;
        }
    }
    for(;;)
    {
        if(it == end_)
            return it;
        if( grammar::ci_is_equal(
                decode_view(it->key), dk))
            return it;
        ++it;
    }
}

auto
params_encoded_view::
find_prev(
    iterator it,
    string_view key,
    ignore_case_param ic) const ->
        iterator
{
    auto const begin_ = begin();
    if(! ic)
    {
        for(;;)
        {
            if(it == begin_)
                return end();
            --it;
            if(it->key == key)
                return it;
        }
    }
    for(;;)
    {
        if(it == begin_)
            return end();
        --it;
        if(grammar::ci_is_equal(
                decode_view(it->key),
                decode_view(key)))
            return it;
    }
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

std::size_t
params_encoded_view::
erase(
    string_view key,
    ignore_case_param ic)
{
    // VFALCO we can't cache end() here
    // because it will be invalidated
    // every time we erase.

    auto it = find_prev(
        end(), key, ic);
    if(it == end())
        return 0;
    std::size_t n = 0;
    for(;;)
    {
        ++n;
        auto prev = find_prev(
            it, key, ic);
        if(prev == end())
        {
            // prev would be invalidated by
            // erase() so we have to compare
            // it to end() beforehand.
            erase(it);
            break;
        }
        erase(it);
        it = prev;
    }
    return n;
}

//------------------------------------------------

auto
params_encoded_view::
reset(
    iterator pos) noexcept ->
        iterator
{
    BOOST_ASSERT(pos.it_.nk > 0);
    return u_->edit_params(
        pos.it_, pos.it_.next(),
        detail::param_encoded_value_iter(
            pos.it_.nk - 1, "", false));
}

auto
params_encoded_view::
set(
    iterator pos,
    string_view value) ->
        iterator
{
    BOOST_ASSERT(pos.it_.nk > 0);
    return u_->edit_params(
        pos.it_,
        pos.it_.next(),
        detail::param_encoded_value_iter(
            pos.it_.nk - 1, value, true));
}

auto
params_encoded_view::
set(
    string_view key,
    string_view value,
    ignore_case_param ic) ->
        iterator
{
    // VFALCO we can't cache end() here
    // because it will be invalidated
    // every time we set or erase.

    auto it0 = find(key, ic);
    if(it0 == end())
        return append(
            param_view(
                key, value));
    it0 = set(it0, value);
    auto it = end();
    for(;;)
    {
        it = find_prev(
            it, key, ic);
        if(it == it0)
            return it0;
        it = erase(it);
    }
}

//------------------------------------------------

} // urls
} // boost

#endif

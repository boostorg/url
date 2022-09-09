//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_VIEW_IPP

#include <boost/url/params_view.hpp>
#include <boost/url/url_base.hpp>
#include <boost/url/grammar/ci_string.hpp>
#include <boost/assert.hpp>
#include <utility>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

std::size_t
params_view::
erase(
    string_view key,
    ignore_case_param ic) noexcept
{
    // VFALCO we can't cache end() here
    // because it will be invalidated
    // every time we erase.
    auto it = find_last(key, ic);
    if(it == end())
        return 0;
    std::size_t n = 0;
    for(;;)
    {
        ++n;
        auto prev = find_last(it, key, ic);
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
params_view::
unset(
    iterator pos) noexcept ->
        iterator
{
    BOOST_ASSERT(pos.it_.nk > 0);
    string_view s;
    return u_->edit_params(
        pos.it_,
        pos.it_.next(),
        detail::param_value_iter(
            pos.it_.nk - 1, s, false));
}

auto
params_view::
set(
    iterator pos,
    string_view value) ->
        iterator
{
    BOOST_ASSERT(pos.it_.nk > 0);
    return u_->edit_params(
        pos.it_,
        pos.it_.next(),
        detail::param_value_iter(
            pos.it_.nk - 1, value, true));
}

auto
params_view::
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
        return append({key, value});
    it0 = set(it0, value);
    auto it = end();
    for(;;)
    {
        it = find_last(it, key, ic);
        if(it == it0)
            return it0;
        it = erase(it);
    }
}

//------------------------------------------------
//
// (implementation)
//
//------------------------------------------------

detail::params_iter_impl
params_view::
find_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl end_(u_->u_, 0);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(end_))
                return it;
            if(*it.key() == key)
                return it;
            it.increment();
        }
    }
    for(;;)
    {
        if(it.equal(end_))
            return it;
        if( grammar::ci_is_equal(
                *it.key(), key))
            return it;
        it.increment();
    }
}

detail::params_iter_impl
params_view::
find_last_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl begin_(u_->u_);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(begin_))
                return { u_->u_, 0 };
            it.decrement();
            if(*it.key() == key)
                return it;
        }
    }
    for(;;)
    {
        if(it.equal(begin_))
            return { u_->u_, 0 };
        it.decrement();
        if(grammar::ci_is_equal(
                *it.key(), key))
            return it;
    }
}

} // urls
} // boost

#endif

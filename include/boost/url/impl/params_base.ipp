//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_BASE_IPP
#define BOOST_URL_IMPL_PARAMS_BASE_IPP

#include <boost/url/params_base.hpp>
#include <boost/url/url_base.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

param_view
params_base::
iterator::
dereference() const
{
    if(! valid_)
    {
        // VFALCO This could be better,
        // we should never shrink size() for
        // a recycled std::string, because
        // otherwise when we resize it larger
        // we will again have to value-init (?)
        // the new chars.
        key_.acquire();
        value_.acquire();
        (*it_.key()).assign_to(*key_);
        has_value_ = it_.has_value();
        if(has_value_)
            (*it_.value()).assign_to(*value_);
        valid_ = true;
    }
    return { *key_, *value_, has_value_ };
}

params_base::
iterator::
iterator(
    iterator const& other) noexcept
    : it_(other.it_)
{
}

auto
params_base::
iterator::
operator=(
    iterator const& other) noexcept ->
        iterator&
{
    it_ = other.it_;
    valid_ = false;
    return *this;
}

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

pct_string_view
params_base::
buffer() const noexcept
{
    auto s = impl_->get(id_query);
    if(s.empty())
        return {};
    return detail::make_pct_string_view(
        s.data() + 1, s.size() - 1,
            impl_->decoded_[id_query]);
}

std::size_t
params_base::
count(
    string_view key,
    ignore_case_param ic) const noexcept
{
    std::size_t n = 0;
    auto it = find(key, ic);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        ++it;
        it = find(it, key, ic);
    }
    return n;
}

//------------------------------------------------
//
// (implementation)
//
//------------------------------------------------

detail::params_iter_impl
params_base::
find_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl end_(*impl_, 0);
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
params_base::
find_last_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl begin_(*impl_);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(begin_))
                return { *impl_, 0 };
            it.decrement();
            if(*it.key() == key)
                return it;
        }
    }
    for(;;)
    {
        if(it.equal(begin_))
            return { *impl_, 0 };
        it.decrement();
        if(grammar::ci_is_equal(
                *it.key(), key))
            return it;
    }
}

} // urls
} // boost

#endif

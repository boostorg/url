//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_BASE_IPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_BASE_IPP

#include <boost/url/params_encoded_base.hpp>
#include <boost/url/url_view_base.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

pct_string_view
params_encoded_base::
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
params_encoded_base::
count(
    pct_string_view key,
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
params_encoded_base::
find_impl(
    detail::params_iter_impl it,
    pct_string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl end_(*impl_, 0);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(end_))
                return it;
            if(*it.key() == *key)
                return it;
            it.increment();
        }
    }
    for(;;)
    {
        if(it.equal(end_))
            return it;
        if( grammar::ci_is_equal(
                *it.key(), *key))
            return it;
        it.increment();
    }
}

detail::params_iter_impl
params_encoded_base::
find_last_impl(
    detail::params_iter_impl it,
    pct_string_view key,
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
            if(*it.key() == *key)
                return it;
        }
    }
    for(;;)
    {
        if(it.equal(begin_))
            return { *impl_, 0 };
        it.decrement();
        if(grammar::ci_is_equal(
                *it.key(), *key))
            return it;
    }
}

} // urls
} // boost

#endif

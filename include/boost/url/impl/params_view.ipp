//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_VIEW_IPP

#include <boost/url/params_view.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
params_view::
at(string_view key) const ->
    pct_encoded_view
{
    auto it = find(key);
    for(;;)
    {
        if(it == end())
            detail::throw_out_of_range(
                BOOST_CURRENT_LOCATION);
        if(it.impl_.nv_ != 0)
            break;
        ++it;
        it = find(it, key);
    }
    string_view ev{
        it.impl_.pos_ + it.impl_.nk_ + 1,
        it.impl_.nv_ - 1 };
    return pct_encoded_view(ev);
}

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

auto
params_view::
begin() const noexcept ->
    iterator
{
    if(n_ > 0)
        return { s_ };
    return end();
}

auto
params_view::
end() const noexcept ->
    iterator
{
    return { s_, n_, 0 };
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

std::size_t
params_view::
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
params_view::
find(
    iterator from,
    string_view key) const noexcept ->
        iterator
{
    BOOST_ASSERT(from.impl_.end_ ==
        s_.data() + s_.size());

    auto const end_ = end();
    while(from != end_)
    {
        if( detail::key_equal_encoded(
            key,
            from.encoded_key()))
            break;
        ++from;
    }
    return from;
}

} // urls
} // boost

#endif

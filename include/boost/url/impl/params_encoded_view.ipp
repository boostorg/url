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
#include <boost/url/url.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/detail/normalize.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
params_encoded_view::
at(string_view key) const ->
    string_view
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
    return {
        it.impl_.pos_ + it.impl_.nk_ + 1,
        it.impl_.nv_ - 1 };
}

//--------------------------------------------
//
// Iterators
//
//--------------------------------------------

auto
params_encoded_view::
begin() const noexcept ->
    iterator
{
    if(n_ > 0)
        return { s_, n_ };
    return end();
}

auto
params_encoded_view::
end() const noexcept ->
    iterator
{
    return { s_, n_ , 0 };
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

std::size_t
params_encoded_view::
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
params_encoded_view::
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
        if( detail::compare_encoded(
                key, from.encoded_key()) == 0)
            break;
        ++from;
    }
    return from;
}

//------------------------------------------------
//
// Parsing
//
//------------------------------------------------

result<params_encoded_view>
parse_query_params(
    string_view s) noexcept
{
    auto rv = grammar::parse(
        s, query_rule{});
    if(! rv)
        return rv.error();
    auto const& v = *rv;
    return params_encoded_view(
        v.string(), v.size());
}

} // urls
} // boost

#endif

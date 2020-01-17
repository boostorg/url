//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_PARAMS_IPP
#define BOOST_URL_IMPL_PARAMS_IPP

#include <boost/url/params.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace url {

//----------------------------------------------------------

bool
params::
contains(string_view key) const noexcept
{
    BOOST_ASSERT(v_);
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            return true;
    return false;
}

std::size_t
params::
count(string_view key) const noexcept
{
    BOOST_ASSERT(v_);
    std::size_t n = 0;
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            ++n;
    return n;
}

auto
params::
find(string_view key) const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    auto it = begin();
    for(auto const last = end();
        it != last; ++it)
        if(detail::key_equal(
            it->encoded_key(),
            key))
            break;
    return it;
}

std::string
params::
operator[](string_view key) const
{
    BOOST_ASSERT(v_);
    auto const it = find(key);
    if(it == end())
        return "";
    return it->value();
}

auto
params::
at(std::size_t i) const ->
    value_type
{
    BOOST_ASSERT(v_);
    if(i >= v_->nparam_)
        out_of_range::raise();
    return { v_, static_cast<
        id_type>(i) };
}

//----------------------------------------------------------

string_view
params::
value_type::
encoded_key() const noexcept
{
    auto s = v_->get(
        v_->detail::id_query +
            2 * i_);
    BOOST_ASSERT(! s.empty());
    BOOST_ASSERT(
        s.front() == '?' ||
        s.front() == '&');
    return s.substr(1);
}

string_view
params::
value_type::
encoded_value() const noexcept
{
    auto s = v_->get(
        v_->detail::id_query +
            2 * i_ + 1);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.front() == '=');
    return s.substr(1);
}

} // url
} // boost

#endif

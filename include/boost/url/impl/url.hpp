//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_HPP
#define BOOST_URL_IMPL_URL_HPP

namespace boost {
namespace urls {

bool
url::
segments_type::
iterator::
operator==(
    iterator other) const noexcept
{
    BOOST_ASSERT(
        v_ != other.v_ ||
        off_ != other.off_ ||
        n_ == other.n_);
    return
        v_ == other.v_ &&
        off_ == other.off_;
}

//----------------------------------------------------------

template<class Allocator>
string_type<Allocator>
url::
params_type::
at( string_view key,
    Allocator const& a) const
{
    auto const it = find(key);
    if(it == end())
        out_of_range::raise();
    return it->value(a);
}

bool
url::
params_type::
iterator::
operator==(
    iterator other) const noexcept
{
    BOOST_ASSERT(
        v_ != other.v_ ||
        off_ != other.off_ || (
            nk_ == other.nk_ &&
            nv_ == other.nv_));
    return
        v_ == other.v_ &&
        off_ == other.off_;
}

} // urls
} // boost

#endif
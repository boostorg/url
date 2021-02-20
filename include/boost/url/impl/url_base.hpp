//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_URL_BASE_HPP
#define BOOST_URL_IMPL_URL_BASE_HPP

namespace boost {
namespace urls {

url_view::
segments_type::
segments_type(
    url_base const& v) noexcept
    : pt_(&v.pt_)
{
}

bool
url_base::
segments_type::
iterator::
operator==(
    iterator other) const noexcept
{
    BOOST_ASSERT(
        pt_ != other.pt_ ||
        off_ != other.off_ ||
        n_ == other.n_);
    return
        pt_ == other.pt_ &&
        off_ == other.off_;
}

auto
url_base::
segments() const noexcept ->
    url_view::segments_type
{
    return url_view::segments_type(*this);
}

auto
url_base::
segments() noexcept ->
    segments_type
{
    return segments_type(*this);
}

//----------------------------------------------------------

url_view::
params_type::
params_type(
    url_base const& v) noexcept
    : pt_(&v.pt_)
{
}

template<class Allocator>
string_type<Allocator>
url_base::
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
url_base::
params_type::
iterator::
operator==(
    iterator other) const noexcept
{
    BOOST_ASSERT(
        pt_ != other.pt_ ||
        off_ != other.off_ || (
            nk_ == other.nk_ &&
            nv_ == other.nv_));
    return
        pt_ == other.pt_ &&
        off_ == other.off_;
}

auto
url_base::
params() const noexcept ->
    url_view::params_type
{
    return url_view::params_type(*this);
}

auto
url_base::
params() noexcept ->
    params_type
{
    return params_type(*this);
}

} // urls
} // boost

#endif

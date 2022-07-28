//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PARAMS_ITERATOR_IMPL_IPP
#define BOOST_URL_DETAIL_IMPL_PARAMS_ITERATOR_IMPL_IPP

#include <boost/url/detail/params_iterator_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

void
params_iterator_impl::
scan() noexcept
{
    if (!pos_)
    {
        nk_ = {};
        nv_ = {};
        return;
    }

    string_view s(pos_, end_ - pos_);
    std::size_t i;
    if(pos_ != begin_ || i_ != 0)
    {
        BOOST_ASSERT(
            s.starts_with('&'));
        i = s.find_first_of('&', 1);
    }
    else
    {
        i = s.find_first_of('&');
    }
    if( i == string_view::npos)
        i = s.size();
    nk_ = string_view(
        pos_, i).find_first_of('=');
    if(nk_ != string_view::npos)
    {
        nv_ = i - nk_;
    }
    else
    {
        // has_value==false
        nk_ = i;
        nv_ = 0;
    }
}

params_iterator_impl::
params_iterator_impl(
    string_view s,
    std::size_t nparam) noexcept
    : begin_(s.data())
    , end_(s.data() + s.size())
    , pos_(nparam ?
        s.data() :
        nullptr)
{
    scan();
}

params_iterator_impl::
params_iterator_impl(
    string_view s,
    std::size_t nparam,
    int) noexcept
    : begin_(s.data())
    , end_(s.data() + s.size())
    , pos_(nullptr)
    , i_(nparam)
{
}

params_view::reference
params_iterator_impl::
dereference() const noexcept
{
    BOOST_ASSERT(begin_ != nullptr);
    BOOST_ASSERT(end_ != nullptr);
    BOOST_ASSERT(pos_ != nullptr);

    int prefix = pos_ != begin_ || i_ != 0;
    if(nv_ > 0)
        return query_param_encoded_view(
            pos_ + prefix,
            nk_ - prefix,
            nv_);
    return query_param_encoded_view(
        pos_ + prefix,
        nk_ - prefix,
        nv_);
}


void
params_iterator_impl::
increment() noexcept
{
    BOOST_ASSERT(begin_ != nullptr);
    BOOST_ASSERT(end_ != nullptr);
    BOOST_ASSERT(pos_ != nullptr);

    ++i_;
    pos_ += nk_ + nv_;
    if(pos_ == end_)
    {
        nk_ = 0;
        nv_ = 0;
        pos_ = nullptr;
        return;
    }
    scan();
}

string_view
params_iterator_impl::
encoded_key() const noexcept
{
    BOOST_ASSERT(begin_ != nullptr);
    BOOST_ASSERT(end_ != nullptr);
    BOOST_ASSERT(pos_ != end_);
    BOOST_ASSERT(pos_ != nullptr);

    if(pos_ != begin_)
        return string_view(
            pos_ + 1, nk_ - 1);
    return string_view{ pos_, nk_ };
}


} // detail
} // url
} // boost

#endif

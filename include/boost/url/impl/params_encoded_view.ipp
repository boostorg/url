//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
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
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

void
params_encoded_view::
iterator::
scan() noexcept
{
    string_view s(
        begin_, begin_ - end_);
    auto i = s.find_first_of('&');
    if( i == string_view::npos)
        i = s.size();
    nk_ = string_view(
        p_, i).find_first_of('=');
    if(nk_ != string_view::npos)
    {
        nv_ = s.size() - nk_;
    }
    else
    {
        // has_key==false
        nk_ = 0;
    }
}

params_encoded_view::
iterator::
iterator(
    string_view s) noexcept
    : begin_(s.data())
    , end_(s.data() + s.size())
{
    if(s.empty())
    {
        p_ = nullptr;
        return;
    }
    p_ = begin_;
    scan();
}

params_encoded_view::
iterator::
iterator(
    string_view s,
    int) noexcept
    : begin_(s.data())
    , end_(s.data() + s.size())
    , p_(nullptr)
{
}

string_view
params_encoded_view::
iterator::
encoded_key() const noexcept
{
    BOOST_ASSERT(p_ != nullptr);
    if( nk_ > 0 &&
        p_[0] == '&')
        return string_view{
            p_ + 1, nk_ - 1};
    return string_view{ p_, nk_ };
}

auto
params_encoded_view::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(p_ != nullptr);
    p_ += nk_ + nv_;
    if(p_ == end_)
    {
        p_ = nullptr;
        return *this;
    }
    scan();
    return *this;
}

auto
params_encoded_view::
iterator::
operator*() const ->
    reference
{
    if(nv_ > 0)
        return reference{
            string_view(
                p_ + 1,
                nk_ - 1),
            string_view(
                p_ + nk_ + 1,
                nv_ - 1),
            true};
    return reference{
        string_view(
            p_ + 1,
            nk_ - 1),
        string_view{},
        false};
}

bool
operator==(
    params_encoded_view::
        iterator a,
    params_encoded_view::
        iterator b) noexcept
{
    BOOST_ASSERT(a.begin_ == b.begin_);
    BOOST_ASSERT(a.end_ == b.end_);
    BOOST_ASSERT(
        a.p_ != b.p_ || (
            a.nk_ == b.nk_ &&
            a.nv_ == b.nv_));
    return a.p_ == b.p_;
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
    return { s_ };
}

auto
params_encoded_view::
end() const noexcept ->
    iterator
{
    return { s_, 0 };
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
    BOOST_ASSERT(
        from.begin_ == s_.data());
    BOOST_ASSERT(from.end_ ==
        s_.data() + s_.size());

    auto const end_ = end();
    while(from != end_)
    {
        // VFALCO need pct-encoded comparison
        if( key == from.encoded_key())
            break;
        ++from;
    }
    return from;
}

} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_VIEW_IPP
#define BOOST_URL_IMPL_SEGMENTS_VIEW_IPP

#include <boost/url/segments_view.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/paths_bnf.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

segments_view::
iterator::
iterator(
    string_view s,
    string_value::allocator const& a) noexcept
    : begin_(s.data())
    , pos_(s.data())
    , next_(s.data())
    , end_(s.data() + s.size())
    , a_(a)
{
    using bnf::parse;
    using bnf_t = path_rootless_bnf;
    using detail::pct_decode_unchecked;
    if(next_ == end_)
    {
        next_ = nullptr;
        return;
    }
    error_code ec;
    if(*next_ == '/')
    {
        // "/" segment
        bnf_t::increment(next_,
            end_, ec, t_);
        BOOST_ASSERT(! ec);
    }
    else
    {
        // segment-nz
        bnf_t::begin(next_,
            end_, ec, t_);
        BOOST_ASSERT(! ec);
    }
}

segments_view::
iterator::
iterator(
    std::size_t,
    string_view s,
    string_value::
        allocator const& a) noexcept
    : begin_(s.data())
    , pos_(s.data() + s.size())
    , end_(s.data() + s.size())
    , a_(a)
{
}

segments_view::
iterator::
iterator() noexcept = default;

segments_view::
iterator::
iterator(
    iterator const&) noexcept = default;

auto
segments_view::
iterator::
operator=(
    iterator const&) noexcept ->
        iterator& = default;

string_value
segments_view::
iterator::
operator*() const noexcept
{
    char* dest;
    auto s = a_.make_string_value(
        t_.decoded_size, dest);
    detail::pct_decode_unchecked(
        dest,
        dest + t_.decoded_size,
        t_.str,
        {});
    return s;
}

auto
segments_view::
iterator::
operator++() noexcept ->
    iterator&
{
    using bnf::parse;
    using bnf_t = path_rootless_bnf;
    using detail::pct_decode_unchecked;
    BOOST_ASSERT(next_ != nullptr);
    pos_ = next_;
    error_code ec;
    // "/" segment
    bnf_t::increment(
        next_, end_, ec, t_);
    if(ec == error::end)
    {
        next_ = nullptr;
        return *this;
    }
    BOOST_ASSERT(! ec);
    return *this;
}

auto
segments_view::
iterator::
operator--() noexcept ->
    iterator&
{
    using bnf::parse;
    using bnf_t = path_rootless_bnf;
    using detail::pct_decode_unchecked;
    BOOST_ASSERT(pos_ != begin_);
    error_code ec;
    while(--pos_ != begin_)
    {
        if(*pos_ != '/')
            continue;
        // "/" segment
        next_ = pos_;
        bnf_t::increment(next_,
            end_, ec, t_);
        BOOST_ASSERT(! ec);
        return *this;
    }
    next_ = pos_;
    if(*next_ == '/')
    {
        // "/" segment
        bnf_t::increment(next_,
            end_, ec, t_);
        BOOST_ASSERT(! ec);
    }
    else
    {
        // segment-nz
        bnf_t::begin(next_,
            end_, ec, t_);
        BOOST_ASSERT(! ec);
    }
    return *this;
}

auto
segments_view::
begin() const noexcept ->
    iterator
{
    return iterator(s_, a_);
}

auto
segments_view::
end() const noexcept ->
    iterator
{
    return iterator(n_, s_, a_);
}

} // urls
} // boost

#endif

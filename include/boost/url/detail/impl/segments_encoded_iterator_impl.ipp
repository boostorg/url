//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_SEGMENTS_ENCODED_ITERATOR_IMPL_IPP
#define BOOST_URL_DETAIL_IMPL_SEGMENTS_ENCODED_ITERATOR_IMPL_IPP

#include <boost/url/detail/segments_encoded_iterator_impl.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

segments_encoded_iterator_impl::
segments_encoded_iterator_impl(
    string_view s,
    std::size_t nseg) noexcept
    : begin_(s.data())
    , pos_(s.data())
    , next_(s.data())
    , end_(s.data() + s.size())
{
    if(nseg == 0)
    {
        next_ = nullptr;
        return;
    }
    auto const n = path_prefix(s);
    begin_ += n;
    next_ += n;
    pos_ += n;
    auto const i = string_view(
        begin_, s.size() - n
            ).find_first_of('/');
    if(i != string_view::npos)
        next_ += i;
    else
        next_ = end_;
    s_ = string_view(
        pos_, next_ - pos_);
}

segments_encoded_iterator_impl::
segments_encoded_iterator_impl(
    string_view s,
    std::size_t nseg,
    int) noexcept
    : i_(nseg)
    , begin_(s.data())
    , pos_(s.data() + s.size())
    , end_(s.data() + s.size())
{
    auto const n = path_prefix(s);
    begin_ += n;
}

void
segments_encoded_iterator_impl::
increment() noexcept
{
    BOOST_ASSERT(next_ != nullptr);
    ++i_;
    pos_ = next_;
    // "/" segment
    auto rv = grammar::parse(
        next_, end_,
            detail::slash_segment_rule);
    if(rv.has_error())
    {
        next_ = nullptr;
        return;
    }
    s_ = rv->encoded();
}

void
segments_encoded_iterator_impl::
decrement() noexcept
{
    BOOST_ASSERT(i_ != 0);
    --i_;
    if(i_ == 0)
    {
        next_ = pos_;
        pos_ = begin_;
        s_ = string_view(
            pos_, next_ - pos_);
        return;
    }
    while(--pos_ != begin_)
    {
        if(*pos_ != '/')
            continue;
        // "/" segment
        next_ = pos_;
        auto rv = grammar::parse(
            next_, end_,
            detail::slash_segment_rule);
        s_ = rv->encoded();
        return;
    }
    next_ = pos_;
    if(*next_ == '/')
    {
        // "/" segment
        auto rv = grammar::parse(
            next_, end_,
            detail::slash_segment_rule);
        s_ = rv->encoded();
    }
    else
    {
        // segment-nz
        auto rv = grammar::parse(
            next_, end_,
                detail::slash_segment_rule);
        s_ = rv->encoded();
    }
}


} // detail
} // url
} // boost

#endif

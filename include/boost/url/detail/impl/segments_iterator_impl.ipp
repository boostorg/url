//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_SEGMENTS_ITERATOR_IMPL_IPP
#define BOOST_URL_DETAIL_IMPL_SEGMENTS_ITERATOR_IMPL_IPP

#include <boost/url/detail/segments_iterator_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

segments_iterator_impl::
segments_iterator_impl(
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
    t_ = grammar::parse(
        next_, end_,
        segment_rule).value();
}

segments_iterator_impl::
segments_iterator_impl(
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

pct_encoded_view
segments_iterator_impl::
dereference() const noexcept
{
    pct_decode_opts opt;
    opt.plus_to_space = false;
    return pct_encoded_view(t_.encoded(), opt);
}


void
segments_iterator_impl::
increment() noexcept
{
    BOOST_ASSERT(next_ != nullptr);
    ++i_;
    pos_ = next_;
    // "/" segment
    auto rv = path_rootless_rule{}.increment(
        next_, end_);
    if(rv == grammar::error::end)
    {
        next_ = nullptr;
        return;
    }
    t_ = rv.value();
}

void
segments_iterator_impl::
decrement() noexcept
{
    BOOST_ASSERT(i_ != 0);
    --i_;
    error_code ec;
    if(i_ == 0)
    {
        next_ = begin_;
        pos_ = begin_;
        t_ = grammar::parse(
            next_, end_,
            segment_rule).value();
        return;
    }
    while(--pos_ != begin_)
    {
        if(*pos_ != '/')
            continue;
        // "/" segment
        next_ = pos_;
        t_ = path_rootless_rule{}.increment(
            next_, end_).value();
        return;
    }
    next_ = pos_;
    if(*next_ == '/')
    {
        // "/" segment
        t_ = path_rootless_rule{}.increment(
            next_, end_).value();
    }
    else
    {
        // segment-nz
        t_ = path_rootless_rule{}.begin(
            next_, end_).value();
    }
}


} // detail
} // url
} // boost

#endif

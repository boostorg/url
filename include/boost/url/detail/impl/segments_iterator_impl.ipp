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

namespace boost {
namespace urls {
namespace detail {

segments_iterator_impl::
segments_iterator_impl(
    string_view s,
    std::size_t nseg,
    const_string::factory const& a) noexcept
    : begin_(s.data())
    , pos_(s.data())
    , next_(s.data())
    , end_(s.data() + s.size())
    , a_(a)
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
    error_code ec;
    grammar::parse(
        next_, end_, ec,
        segment_rule{t_});
    BOOST_ASSERT(! ec);
}

segments_iterator_impl::
segments_iterator_impl(
    string_view s,
    std::size_t nseg,
    const_string::factory const& a,
    int) noexcept
    : i_(nseg)
    , begin_(s.data())
    , pos_(s.data() + s.size())
    , end_(s.data() + s.size())
    , a_(a)
{
    auto const n = path_prefix(s);
    begin_ += n;
}

const_string
segments_iterator_impl::
dereference() const noexcept
{
    return a_(t_.decoded_size,
        [this](
            std::size_t, char* dest)
        {
            pct_decode_opts opt;
            opt.plus_to_space = false;
            pct_decode_unchecked(
                dest, dest + t_.decoded_size,
                    t_.str, opt);
        });
}


void
segments_iterator_impl::
increment() noexcept
{
    using bnf_t = path_rootless_rule;
    BOOST_ASSERT(next_ != nullptr);
    ++i_;
    pos_ = next_;
    error_code ec;
    // "/" segment
    bnf_t::increment(
        next_, end_, ec, t_);
    if(ec == grammar::error::end)
    {
        next_ = nullptr;
        return;
    }
    BOOST_ASSERT(! ec);
}

void
segments_iterator_impl::
decrement() noexcept
{
    using bnf_t = path_rootless_rule;
    BOOST_ASSERT(i_ != 0);
    --i_;
    error_code ec;
    if(i_ == 0)
    {
        next_ = begin_;
        pos_ = begin_;
        grammar::parse(
            next_, end_, ec,
            segment_rule{t_});
        BOOST_ASSERT(! ec.failed());
        return;
    }
    while(--pos_ != begin_)
    {
        if(*pos_ != '/')
            continue;
        // "/" segment
        next_ = pos_;
        bnf_t::increment(next_,
                         end_, ec, t_);
        BOOST_ASSERT(! ec);
        return;
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
}


} // detail
} // url
} // boost

#endif

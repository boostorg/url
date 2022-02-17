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
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/assert.hpp>
#include <ostream>

namespace boost {
namespace urls {

segments_view::
iterator::
iterator(
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
    auto const n =
        detail::path_prefix(s);
    begin_ += n;
    next_ += n;
    pos_ += n;
    error_code ec;
    grammar::parse(
        next_, end_, ec,
        segment_rule{t_});
    BOOST_ASSERT(! ec);
}

segments_view::
iterator::
iterator(
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
    auto const n =
        detail::path_prefix(s);
    begin_ += n;
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

const_string
segments_view::
iterator::
operator*() const noexcept
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

auto
segments_view::
iterator::
operator++() noexcept ->
    iterator&
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
        return *this;
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

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

auto
segments_view::
begin() const noexcept ->
    iterator
{
    return iterator(s_, n_, a_);
}

auto
segments_view::
end() const noexcept ->
    iterator
{
    return iterator(s_, n_, a_, 0);
}

//------------------------------------------------
//
// Friends
//
//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    segments_view const& vw)
{
    auto it = vw.begin();
    auto const end = vw.end();
    if( it != end )
    {
        if( vw.is_absolute() )
            os << "/";
        os << *it;
        while( ++it != end )
            os << '/' << *it;
    }
    return os;
}

} // urls
} // boost

#endif

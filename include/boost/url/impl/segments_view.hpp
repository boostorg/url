//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_VIEW_HPP
#define BOOST_URL_IMPL_SEGMENTS_VIEW_HPP

#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/rfc/paths_bnf.hpp>
#include <cstdint>

namespace boost {
namespace urls {

template<class Alloc>
class segments_view<Alloc>::
    iterator
{
    std::size_t i_ = 0;
    string_type<Alloc> s_;
    char const* begin_ = nullptr;
    char const* pos_ = nullptr;
    char const* next_ = nullptr;
    char const* end_ = nullptr;

    friend segments_view;

    explicit
    iterator(
        string_view s,
        Alloc const& a);

    // end ctor
    iterator(
        std::size_t n,
        string_view s,
        Alloc const& a) noexcept;

public:
    using value_type = string_type<Alloc>;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() noexcept = default;
    iterator(
        iterator const&) noexcept = default;
    iterator& operator=(
        iterator const&) noexcept = default;

    std::size_t
    index() const noexcept
    {
        return i_;
    }

    value_type const&
    operator*() const noexcept
    {
        return s_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &s_;
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        return
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return
            next_ != other.next_ ||
            end_ != other.end_;
    }

    iterator&
    operator++() noexcept;

    iterator&
    operator--() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }
};

//------------------------------------------------

template<class Alloc>
segments_view<Alloc>::
segments_view(
    segments_encoded_view const& sv,
    Alloc const& a) noexcept
    : s_(sv.s_)
    , n_(sv.n_)
    , a_(a)
{
}

//------------------------------------------------

template<class Alloc>
segments_view<Alloc>::
iterator::
iterator(
    string_view s,
    Alloc const& a)
    : s_(a)
    , begin_(s.data())
    , pos_(s.data())
    , next_(s.data())
    , end_(s.data() + s.size())
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
        pct_encoded_str t;
        bnf_t::increment(next_,
            end_, ec, t);
        BOOST_ASSERT(! ec);
        s_ = pct_decode_unchecked(
            t.str, t.decoded_size, {},
                s_.get_allocator());
    }
    else
    {
        // segment-nz
        pct_encoded_str t;
        bnf_t::begin(next_,
            end_, ec, t);
        BOOST_ASSERT(! ec);
        s_ = pct_decode_unchecked(
            t.str, t.decoded_size, {},
                s_.get_allocator());
    }
}

template<class Alloc>
segments_view<Alloc>::
iterator::
iterator(
    std::size_t n,
    string_view s,
    Alloc const& a) noexcept
    : i_(n)
    , s_(a)
    , begin_(s.data())
    , pos_(s.data() + s.size())
    , end_(s.data() + s.size())
{
}

template<class Alloc>
auto
segments_view<Alloc>::
iterator::
operator++() noexcept ->
    iterator&
{
    using bnf::parse;
    using bnf_t = path_rootless_bnf;
    using detail::pct_decode_unchecked;
    BOOST_ASSERT(next_ != nullptr);
    ++i_;
    pos_ = next_;
    error_code ec;
    // "/" segment
    pct_encoded_str t;
    bnf_t::increment(
        next_, end_, ec, t);
    if(ec == error::end)
    {
        next_ = nullptr;
        return *this;
    }
    BOOST_ASSERT(! ec);
    s_ = pct_decode_unchecked(
        t.str, t.decoded_size, {},
            s_.get_allocator());
    return *this;
}

template<class Alloc>
auto
segments_view<Alloc>::
iterator::
operator--() noexcept ->
    iterator&
{
    using bnf::parse;
    using bnf_t = path_rootless_bnf;
    using detail::pct_decode_unchecked;
    BOOST_ASSERT(i_ != 0);
    BOOST_ASSERT(pos_ != begin_);
    --i_;
    error_code ec;
    while(--pos_ != begin_)
    {
        if(*pos_ != '/')
            continue;
        // "/" segment
        next_ = pos_;
        pct_encoded_str t;
        bnf_t::increment(next_,
            end_, ec, t);
        BOOST_ASSERT(! ec);
        s_ = pct_decode_unchecked(
            t.str, t.decoded_size, {},
                s_.get_allocator());
        return *this;
    }
    next_ = pos_;
    if(*next_ == '/')
    {
        // "/" segment
        pct_encoded_str t;
        bnf_t::increment(next_,
            end_, ec, t);
        BOOST_ASSERT(! ec);
        s_ = pct_decode_unchecked(
            t.str, t.decoded_size, {},
                s_.get_allocator());
    }
    else
    {
        // segment-nz
        pct_encoded_str t;
        bnf_t::begin(next_,
            end_, ec, t);
        BOOST_ASSERT(! ec);
        s_ = pct_decode_unchecked(
            t.str, t.decoded_size, {},
                s_.get_allocator());
    }
    return *this;
}

template<class Alloc>
auto
segments_view<Alloc>::
begin() const noexcept ->
    iterator
{
    return iterator(s_, a_);
}

template<class Alloc>
auto
segments_view<Alloc>::
end() const noexcept ->
    iterator
{
    return iterator(n_, s_, a_);
}

} // urls
} // boost

#endif

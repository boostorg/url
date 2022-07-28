//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/segments_encoded_iterator_impl.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <cstdint>
#include <iterator>

namespace boost {
namespace urls {

class segments_encoded_view::iterator
{
    friend segments_encoded_view;

    detail::segments_encoded_iterator_impl impl_;

    iterator(
        string_view s,
        std::size_t nseg) noexcept
        : impl_(s, nseg)
    {
    }

    // end ctor
    iterator(
        string_view s,
        std::size_t nseg,
        int) noexcept
        : impl_(s, nseg, 0)
    {
    }

public:
    using value_type = std::string;
    using reference = string_view;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() = default;

    iterator(iterator const&) noexcept = default;

    iterator& operator=(iterator const&) noexcept = default;

    reference
    operator*() const noexcept
    {
        return impl_.s_;
    }

    iterator&
    operator++() noexcept
    {
        impl_.increment();
        return *this;
    }

    iterator&
    operator--() noexcept
    {
        impl_.decrement();
        return *this;
    }

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

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return impl_.equal(other.impl_);
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !impl_.equal(other.impl_);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------

inline
segments_encoded_view::
segments_encoded_view() noexcept
    : s_("")
    , n_(0)
{
}

inline
bool
segments_encoded_view::
is_absolute() const noexcept
{
    return s_.starts_with('/');
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

inline
string_view
segments_encoded_view::
front() const noexcept
{
    BOOST_ASSERT(! empty());
    return *begin();
}

inline
string_view
segments_encoded_view::
back() const noexcept
{
    BOOST_ASSERT(! empty());
    return *--end();
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
bool
segments_encoded_view::
empty() const noexcept
{
    return size() == 0;
}

inline
std::size_t
segments_encoded_view::
size() const noexcept
{
    return n_;
}

} // urls
} // boost

#endif

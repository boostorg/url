//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_HPP
#define BOOST_URL_IMPL_SEGMENTS_HPP

#include <boost/url/detail/except.hpp>
#include <cstdint>

namespace boost {
namespace urls {

class segments::iterator
{
    std::size_t i_ = 0;
    url* u_ = nullptr;
    segments::value_type v_;

    friend segments;

    iterator(
        url* u,
        std::size_t i) noexcept
        : u_(u)
        , i_(i)
    {
    }

public:
    using value_type =
        segments::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type =
        std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

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
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        return
            u_ == other.u_ &&
            i_ == other.i_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return
            u_ != other.u_ ||
            i_ != other.i_;
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    BOOST_URL_DECL
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

} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PATH_VIEW_HPP
#define BOOST_URL_IMPL_PATH_VIEW_HPP

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

class path_view::iterator
{
    path_view::value_type v_;
    char const* next_ = nullptr;
    char const* end_ = nullptr;

    friend path_view;

    BOOST_URL_DECL
    explicit
    iterator(
        string_view s);

    explicit
    iterator(
        char const* end) noexcept
        : end_(end)
    {
    }

public:
    using value_type =
        path_view::value_type;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type =
        std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() noexcept = default;
    iterator(
        iterator const&) noexcept = default;
    iterator& operator=(
        iterator const&) noexcept = default;

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
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return ! (*this == other);
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }
};

} // urls
} // boost

#endif

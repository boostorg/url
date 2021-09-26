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

#include <boost/url/arrow_proxy.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <cstddef>

namespace boost {
namespace urls {

class segments_view::
    iterator
{
    char const* begin_ = nullptr;
    char const* pos_ = nullptr;
    char const* next_ = nullptr;
    char const* end_ = nullptr;
    string_value::allocator a_;
    pct_encoded_str t_;

    friend segments_view;

    BOOST_URL_DECL
    iterator(
        string_view s,
        string_value::
            allocator const& a) noexcept;

    // end ctor
    BOOST_URL_DECL
    iterator(
        std::size_t n,
        string_view s,
        string_value::
            allocator const& a) noexcept;

public:
    using value_type = string_value;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    BOOST_URL_DECL
    iterator() noexcept;

    BOOST_URL_DECL
    iterator(
        iterator const&) noexcept;

    BOOST_URL_DECL
    iterator&
    operator=(
        iterator const&) noexcept;

    BOOST_URL_DECL
    string_value
    operator*() const noexcept;

    arrow_proxy<string_value>
    operator->() const noexcept
    {
        return {**this};
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

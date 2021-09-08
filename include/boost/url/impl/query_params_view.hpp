//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_PARAMS_VIEW_HPP
#define BOOST_URL_IMPL_QUERY_PARAMS_VIEW_HPP

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

class query_params_view::iterator
{
    query_params_view::value_type v_;
    char const* next_ = nullptr;
    char const* end_ = nullptr;

    friend query_params_view;

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
        query_params_view::value_type;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = std::ptrdiff_t;
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

//------------------------------------------------

template<class Allocator>
string_type<Allocator>
query_params_view::
at( string_view key,
    Allocator const& a) const
{
    auto const it = find(key);
    if(it == end())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return it->value(a);
}

} // urls
} // boost

#endif

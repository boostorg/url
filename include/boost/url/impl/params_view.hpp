//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_view::iterator
{
    char const* end_ = nullptr;
    char const* p_ = nullptr;
    std::size_t nk_ = 0;
    std::size_t nv_ = 0;
    any_allocator<char> a_;
    bool first_ = true;

    friend class params_view;

    void scan() noexcept;

    iterator(
        string_view s,
        any_allocator<char> const& a) noexcept;

    // end
    iterator(
        string_view s,
        any_allocator<char> const& a,
        int) noexcept;

    string_view
    encoded_key() const noexcept;

public:
    using value_type = params_view::value_type;
    using reference = params_view::value_type;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;

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

    BOOST_URL_DECL
    value_type
    operator*() const;

    BOOST_URL_DECL
    friend
    bool
    operator==(
        iterator a,
        iterator b) noexcept;

    friend
    bool
    operator!=(
        iterator a,
        iterator b) noexcept
    {
        return !(a == b);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------

template<class Allocator>
params_view::
params_view(
    string_view s,
    std::size_t n,
    Allocator const& a)
    : s_(s)
    , n_(n)
    , a_(a)
{
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

bool
params_view::
empty() const noexcept
{
    return n_ == 0;
}

std::size_t
params_view::
size() const noexcept
{
    return n_;
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

auto
params_view::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

bool
params_view::
contains(
    string_view key) const noexcept
{
    return find(key) != end();
}

} // urls
} // boost

#endif

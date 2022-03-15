//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_encoded_view::iterator
{
    char const* end_ = nullptr;
    char const* p_ = nullptr;
    std::size_t nk_ = 0;
    std::size_t nv_ = 0;
    bool first_ = true;

    friend class params_encoded_view;

    void scan() noexcept;

    iterator(
        string_view s) noexcept;

    // end
    iterator(
        string_view s,
        int) noexcept;

    string_view
    encoded_key() const noexcept;

public:
    using value_type = query_param;
    using reference = query_param_view;
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
    reference
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

inline
params_encoded_view::
params_encoded_view(
    string_view s,
    std::size_t n) noexcept
    : s_(s)
    , n_(n)
{
}

template<class Allocator>
params_view
params_encoded_view::
decoded(Allocator const& alloc) const
{
    return params_view(s_, n_, alloc);
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
bool
params_encoded_view::
empty() const noexcept
{
    return n_ == 0;
}

inline
std::size_t
params_encoded_view::
size() const noexcept
{
    return n_;
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

inline
auto
params_encoded_view::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

inline
bool
params_encoded_view::
contains(string_view key) const noexcept
{
    return find(key) != end();
}

} // urls
} // boost

#endif

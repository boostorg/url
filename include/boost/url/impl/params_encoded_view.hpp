//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/params_encoded_iterator_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_encoded_view::iterator
{
    detail::params_encoded_iterator_impl impl_;

    friend class params_encoded_view;

    iterator(
        string_view s,
        std::size_t nparam) noexcept
        : impl_(s, nparam)
    {
    }

    // end
    iterator(
        string_view s,
        std::size_t nparam,
        int) noexcept
        : impl_(s, nparam, 0)
    {
    }

    string_view
    encoded_key() const noexcept
    {
        return impl_.encoded_key();
    }

public:
    using value_type = query_param;
    using reference = query_param_encoded_view;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;

    iterator&
    operator++() noexcept
    {
        impl_.increment();
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    reference
    operator*() const
    {
        return impl_.dereference();
    }

    friend
    bool
    operator==(
        iterator const& a,
        iterator const& b) noexcept
    {
        return a.impl_.equal(b.impl_);
    }

    friend
    bool
    operator!=(
        iterator const& a,
        iterator const& b) noexcept
    {
        return !a.impl_.equal(b.impl_);
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

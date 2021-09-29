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
    char const* begin_ = nullptr;
    char const* end_ = nullptr;
    char const* p_ = nullptr;
    std::size_t nk_ = 0;
    std::size_t nv_ = 0;
    string_value::allocator a_;

    friend class params_view;

    void scan() noexcept;

    iterator(
        string_view s,
        string_value::allocator a) noexcept;

    // end
    iterator(
        string_view s,
        int,
        string_value::allocator a) noexcept;

    string_view
    encoded_key() const noexcept;

public:
    using value_type = params_view::value_type;
    using reference = params_view::reference;
    using const_reference =
        params_view::const_reference;
    using pointer = void*;
    using const_pointer = void const*;
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

template<class Key>
auto
params_view::
find(Key const& key) const noexcept ->
    typename std::enable_if<
        is_stringlike<Key>::value,
        iterator>::type
{
    return find(to_string_view(key));
}

template<class Key>
auto
params_view::
find(
    iterator from,
    Key const& key) const noexcept ->
        typename std::enable_if<
            is_stringlike<Key>::value,
            iterator>::type
{
    return find(from,
        to_string_view(key));
}

} // urls
} // boost

#endif

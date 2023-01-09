//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_MATCHES_STORAGE_HPP
#define BOOST_URL_DETAIL_MATCHES_STORAGE_HPP

#include <boost/url/detail/config.hpp>
#include <detail/router.hpp>
#include <memory>
#include <vector>
#include <stdexcept>

namespace boost {
namespace urls {
namespace detail {

// Base route match results
class matches_base
{
protected:
    std::size_t size_ = 0;

    virtual
    string_view const*
    matches() const = 0;

    virtual
    string_view const*
    ids() const = 0;

    matches_base() = default;

    explicit
    matches_base(
        std::size_t size)
        : size_(size)
    {}
public:
    using iterator = string_view*;
    using const_iterator = string_view const*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = string_view&;
    using const_reference = string_view const&;
    using pointer = string_view*;
    using const_pointer = string_view const*;

    const_reference
    at( size_type pos ) const;

    const_reference
    at( string_view id ) const;

    const_reference
    operator[]( size_type pos ) const;

    const_reference
    operator[]( string_view id ) const;

    const_iterator
    find( string_view id ) const;

    const_iterator
    begin() const;

    const_iterator
    end() const;

    bool
    empty() const noexcept;

    size_type
    size() const noexcept;
};

} // detail
} // urls
} // boost

#endif


//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_RANGE_HPP
#define BOOST_URL_BNF_RANGE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/except.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {

template<class T>
class range
{
    string_view s_;

public:
    using value_type = typename
        T::value_type;

    class iterator;

    constexpr range(
        range const&) = default;
    constexpr range& operator=(
        range const&) = default;

    /** Default constructor

        Iteration of default constructed ranges
        is undefined.
    */
    constexpr range() = default;

    /** Constructor
    */
    explicit
    range(
        string_view s) noexcept
        : s_(s)
    {
    }

    /** Return the entire string underlying the range
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    iterator
    begin(error_code& ec) const;

    iterator
    begin() const;

    iterator
    end() const;
};

//------------------------------------------------

template<class T>
class range<T>::iterator
{
    char const* next_ = nullptr;
    char const* end_ = nullptr;
    typename T::value_type v_;

    friend class range;

    explicit
    iterator(string_view s);

    explicit
    iterator(char const* end);

public:
    using value_type =
        typename T::value_type;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() noexcept = default;

    bool
    operator==(
        iterator const& other) const
    {
        return
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator const& other) const
    {
        return !(*this == other);
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

    void
    increment(error_code& ec);

    iterator&
    operator++();

    iterator
    operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/range.hpp>

#endif

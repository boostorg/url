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
    using begin_t = char const*(*)(
        char const*, char const*,
            error_code&, T&);

    using increment_t = char const*(*)(
        char const*, char const*,
            error_code&, T&);

    string_view s_;
    std::size_t n_ = 0;
    begin_t begin_ = nullptr;
    increment_t
        increment_ = nullptr;

    range(
        string_view s,
        std::size_t n,
        begin_t begin,
        increment_t increment)
        : s_(s)
        , n_(n)
        , begin_(begin)
        , increment_(increment)
    {
    }

public:
    using value_type = T;

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

        @par Preconditions

        `s` parses according to the grammar without error

        @param s The string representing the range

        @param n The number of elements in the range
    */
    range(
        string_view s,
        // VFALCO FIX ME
        std::size_t n = 0) noexcept
        : s_(s)
        , n_(n)
    {
    }

    /** Return true if the range is empty
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return the entire string underlying the range
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    iterator
    begin() const;

    iterator
    end() const;

    template<class T, class U>
    friend
    char const*
    parse_range(
        char const* start,
        char const* end,
        error_code& ec,
        range<T>& t,
        U const&);
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/range.hpp>

#endif

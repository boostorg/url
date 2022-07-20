//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_SEGMENTS_ITERATOR_IMPL_HPP
#define BOOST_URL_DETAIL_SEGMENTS_ITERATOR_IMPL_HPP

#include <boost/url/string_view.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <string>

namespace boost {
namespace urls {
namespace detail {

struct segments_iterator_impl
{
    std::size_t i_ = 0;
    char const* begin_ = nullptr;
    char const* pos_ = nullptr;
    char const* next_ = nullptr;
    char const* end_ = nullptr;
    pct_encoded_view t_;

    BOOST_URL_DECL
    segments_iterator_impl(
        string_view s,
        std::size_t nseg) noexcept;

    // end ctor
    BOOST_URL_DECL
    segments_iterator_impl(
        string_view s,
        std::size_t nseg,
        int) noexcept;

    segments_iterator_impl() = default;

    segments_iterator_impl(
        segments_iterator_impl const&) noexcept = default;

    segments_iterator_impl& operator=(
        segments_iterator_impl const&) noexcept = default;

    BOOST_URL_DECL
    pct_encoded_view
    dereference() const noexcept;

    BOOST_URL_DECL
    void
    increment() noexcept;

    BOOST_URL_DECL
    void
    decrement() noexcept;

    bool
    equal(
        segments_iterator_impl const& other) const noexcept
    {
        return
            next_ == other.next_ &&
            end_ == other.end_;
    }
};

} // detail
} // urls
} // boost

#endif

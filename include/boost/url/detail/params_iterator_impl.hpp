//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PARAMS_ITERATOR_IMPL_HPP
#define BOOST_URL_DETAIL_PARAMS_ITERATOR_IMPL_HPP

#include <boost/url/string_view.hpp>
#include <boost/url/params_view.hpp>
#include <string>

namespace boost {
namespace urls {
namespace detail {

struct params_iterator_impl
{
    // range
    char const* begin_ = nullptr;
    char const* end_ = nullptr;
    // current element
    char const* pos_ = nullptr;
    std::size_t i_ = 0;
    // key/val offsets
    std::size_t nk_ = 0;
    std::size_t nv_ = 0;

    BOOST_URL_DECL
    void scan() noexcept;

    BOOST_URL_DECL
    params_iterator_impl(
        string_view s,
        std::size_t nparam) noexcept;

    // end ctor
    BOOST_URL_DECL
    params_iterator_impl(
        string_view s,
        std::size_t nparam,
        int) noexcept;

    params_iterator_impl() = default;

    params_iterator_impl(
        params_iterator_impl const&) noexcept = default;

    params_iterator_impl& operator=(
        params_iterator_impl const&) noexcept = default;

    string_view
    encoded_key() const noexcept;

    BOOST_URL_DECL
    params_view::reference
    dereference() const noexcept;

    BOOST_URL_DECL
    void
    increment() noexcept;

    bool
    equal(
        params_iterator_impl const& other) const noexcept
    {
        BOOST_ASSERT(begin_ == other.begin_);
        BOOST_ASSERT(end_ == other.end_);
        return
            pos_ == other.pos_ &&
            i_ == other.i_;
    }
};

} // detail
} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PARAMS_ITER_IMPL_HPP
#define BOOST_URL_DETAIL_PARAMS_ITER_IMPL_HPP

#include <boost/url/param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

struct params_iter_impl
    : parts_base
{
    url_impl const* u = nullptr;
    std::size_t pos = 0;
    std::size_t nk = 0;
    std::size_t nv = 0;
    std::size_t dk = 0;
    std::size_t dv = 0;
    std::size_t i = 0;

    params_iter_impl() = default;
    params_iter_impl(
        params_iter_impl const&) = default;
    params_iter_impl& operator=(
        params_iter_impl const&) = default;

    // begin
    BOOST_URL_DECL
    explicit
    params_iter_impl(
        url_impl const& u_) noexcept;

    // end
    BOOST_URL_DECL
    params_iter_impl(
        url_impl const& u_,
        int) noexcept;

    // at index
    BOOST_URL_DECL
    params_iter_impl(
        url_impl const& u_,
        std::size_t pos_,
        std::size_t i_) noexcept;

    BOOST_URL_DECL
    void
    increment() noexcept;

    BOOST_URL_DECL
    void
    decrement() noexcept;

    BOOST_URL_DECL
    param_decode_view
    dereference() const noexcept;

    auto
    next() const noexcept ->
        params_iter_impl
    {
        auto next = *this;
        next.increment();
        return next;
    }

    bool
    equal(
        params_iter_impl const&
            other) const noexcept
    {
        // different containers
        BOOST_ASSERT(u == other.u);
        return i == other.i;
    }

private:
    void update() noexcept;
};

} // detail
} // urls
} // boost

#endif

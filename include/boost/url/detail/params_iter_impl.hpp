//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_PARAMS_ITER_IMPL_HPP
#define BOOST_URL_DETAIL_PARAMS_ITER_IMPL_HPP

#include <boost/url/param.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/url/detail/url_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

struct params_iter_impl
    : parts_base
{
    url_impl const* impl = nullptr;
    std::size_t pos = 0;// to '?' or '&'
    std::size_t nk = 0; // includes '?' or '&'
    std::size_t nv = 0; // includes '='
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
    params_iter_impl(
        url_impl const& u_) noexcept;

    // end
    BOOST_URL_DECL
    params_iter_impl(
        url_impl const& u_,
        int) noexcept;

    // at index
    params_iter_impl(
        url_impl const&,
        std::size_t,
        std::size_t) noexcept;

    BOOST_URL_DECL
    param_pct_view
    dereference() const noexcept;

    BOOST_URL_DECL
    void
    increment() noexcept;

    BOOST_URL_DECL
    void
    decrement() noexcept;

    bool
    at_end() const noexcept
    {
        return nk == 0;
    }

    bool
    has_value() const noexcept
    {
        return nv > 0;
    }

    pct_string_view
    key() const noexcept
    {
        return detail::make_pct_string_view(
            string_view(
                impl->cs_ +
                    impl->offset(id_query) +
                    pos + 1,
                nk - 1), dk);
    }

    pct_string_view
    value() const noexcept
    {
        BOOST_ASSERT(has_value());
        return detail::make_pct_string_view(
            string_view(
                impl->cs_ +
                    impl->offset(id_query) +
                    pos + nk + 1,
                nv - 1), dv);
    }

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
        BOOST_ASSERT(impl == other.impl);
        return i == other.i;
    }

private:
    void update() noexcept;
};

} // detail
} // urls
} // boost

#endif

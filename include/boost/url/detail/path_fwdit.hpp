//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PATH_FWDIT_HPP
#define BOOST_URL_DETAIL_PATH_FWDIT_HPP

#include <boost/url/detail/helpers.hpp>
#include <boost/url/string.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

class BOOST_SYMBOL_VISIBLE
    path_fwdit
    : public any_helper::any_fwdit<
        string_view>
{
    std::size_t n_ = 0;
    char const* p_ = nullptr;
    char const* end_ = nullptr;

public:
    using T = string_view;

    explicit
    path_fwdit(
        char const* end) noexcept
        : end_(end)
    {
    }

    BOOST_URL_DECL
    explicit
    path_fwdit(
        string_view s) noexcept;

    ~path_fwdit()
    {
    }

    BOOST_URL_DECL
    any_fwdit<T>&
    operator++() noexcept override;

    T
    operator*() const noexcept override
    {
        return { p_, n_ };
    }

    any_fwdit<T>&
    operator=(
        any_fwdit<T> const& other)
            noexcept override
    {
        auto it = static_cast<
            path_fwdit const&>(
                other);
        p_ = it.p_;
        n_ = it.n_;
        end_ = it.end_;
        return *this;
    }

    bool
    operator==(
        any_fwdit<T> const& other)
            const noexcept override
    {
        return p_ == static_cast<
            path_fwdit const&>(
                other).p_;
    }
};

} // detail
} // urls
} // boost

#endif

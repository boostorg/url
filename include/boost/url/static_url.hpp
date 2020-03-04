//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_STATIC_URL_HPP
#define BOOST_URL_STATIC_URL_HPP

#include <boost/url/config.hpp>
#include <boost/url/url_base.hpp>
#include <cstdlib>
#include <utility>

namespace boost {
namespace urls {

template<std::size_t N>
class static_url
    : public url_base
{
    char buf_[N + 1];

public:
    template<class... Args>
    explicit
    static_url(Args&&... args)
        : url_base(
            std::forward<Args>(args)...)
    {
        // VFALCO TODO
    }

    static_url(
        static_url const&) noexcept
    {
        // VFALCO TODO
    }

    static_url&
    operator=(
        static_url const&) noexcept
    {
        // VFALCO TODO
        return *this;
    }
};

} // urls
} // boost

#endif

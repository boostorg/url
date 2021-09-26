//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_ARROW_PROXY_HPP
#define BOOST_URL_ARROW_PROXY_HPP

#include <boost/url/detail/config.hpp>

namespace boost {
namespace urls {

// https://quuxplusone.github.io/blog/2019/02/06/arrow-proxy/

template<class T>
struct arrow_proxy
{
    T t;

    T *operator->() noexcept
    {
        return &t;
    }
};

} // urls
} // boost

#endif

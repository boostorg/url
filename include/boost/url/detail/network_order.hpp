//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_NETWORK_ORDER_HPP
#define BOOST_URL_DETAIL_NETWORK_ORDER_HPP

#include <boost/assert.hpp>
#include <cstdint>
#include <cstring>

namespace boost {
namespace urls {
namespace detail {

inline
std::uint_least32_t
swap_endian(
    std::uint_least32_t v0) noexcept
{
    return
        ((v0 & 0xff000000) >> 24) |
        ((v0 & 0x00ff0000) >>  8) |
        ((v0 & 0x0000ff00) <<  8) |
        ((v0 & 0x000000ff) << 24);
}

inline
bool
is_big_endian() noexcept
{
    unsigned char t0[4] = {};
    std::uint_least32_t t1 = 1;
    std::memcpy(t0, &t1, 4);
    if(t0[0] == 0)
    {
        // big endian
        BOOST_ASSERT(
            t0[3] == 1);
        return true;
    }
    return false;
}

inline
std::uint_least32_t
host_to_network(
    std::uint_least32_t v) noexcept
{
    if(is_big_endian())
        return v;
    return swap_endian(v);
}

inline
std::uint_least32_t
network_to_host(
    std::uint_least32_t v) noexcept
{
    if(is_big_endian())
        return v;
    return swap_endian(v);
}

} // detail
} // urls
} // boost

#endif

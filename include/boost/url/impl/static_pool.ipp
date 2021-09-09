//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_STATIC_POOL_IPP
#define BOOST_URL_IMPL_STATIC_POOL_IPP

#include <boost/url/static_pool.hpp>
#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

void*
basic_static_pool::
allocate(
    std::size_t bytes,
    std::size_t align)
{
    auto const u0 = std::uintptr_t(top_);
    auto const u = align * (
        (u0 + align - 1) / align);
    auto const p =
        reinterpret_cast<char*>(u);
    if( u < u0 || bytes >
        capacity_ - (p - base_))
        detail::throw_bad_alloc(
            BOOST_CURRENT_LOCATION);
    top_ = reinterpret_cast<
        char*>(p + bytes);
    return p;
}

void
basic_static_pool::
deallocate(
    void*,
    std::size_t,
    std::size_t) noexcept
{
}

} // urls
} // boost

#endif

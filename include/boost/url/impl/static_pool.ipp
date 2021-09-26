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
    auto n = alignment::align_up(
        bytes, align);
    auto p = reinterpret_cast<char*>(
        reinterpret_cast<
            std::uintptr_t>(top_ - n) &
                ~(align - 1));
    if(p < begin_)
        detail::throw_bad_alloc(
            BOOST_CURRENT_LOCATION);
    ++n_;
    return p;
}

void
basic_static_pool::
deallocate(
    void*,
    std::size_t,
    std::size_t) noexcept
{
    BOOST_ASSERT(n_ > 0);
    --n_;
    if(n_ > 0)
        return;
    top_ = end_;
}

} // urls
} // boost

#endif

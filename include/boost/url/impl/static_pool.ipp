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

struct basic_static_pool::item
{
    void* p;
    std::size_t n;

    bool
    in_use() const noexcept
    {
        return (reinterpret_cast<
            std::uintptr_t>(p) & 1) != 0;
    }

    void
    use() noexcept
    {
        p = reinterpret_cast<
            void*>((reinterpret_cast<
            std::uintptr_t>(p) | 1));
    }

    void
    free() noexcept
    {
        p = reinterpret_cast<
            void*>((reinterpret_cast<
            std::uintptr_t>(p) & ~1));
    }

    void*
    ptr() const noexcept
    {
        return reinterpret_cast<
            void*>((reinterpret_cast<
            std::uintptr_t>(p) & ~1));
    }
};

void*
basic_static_pool::
allocate(
    std::size_t bytes,
    std::size_t align)
{
    if( align < 2)
        align = 2;
    bytes = alignment::align_up(
        bytes, align);
    item* it0 = reinterpret_cast<
        item*>(base_);
    item* it1 = it0 + n_;
    {
        // find best fit
        item* best = nullptr;
        std::size_t bestn =
            std::size_t(-1);
        auto it = it0;
        while(it != it1)
        {
            if( ! it->in_use() &&
                it->n >= bytes &&
                it->n < bestn)
            {
                best = it;
                bestn = it->n;
            }
            ++it;
        }
        if(best)
        {
            it->use();
            return it->p;
        }
    }
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

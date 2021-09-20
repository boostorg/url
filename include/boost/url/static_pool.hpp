//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_STATIC_POOL_HPP
#define BOOST_URL_STATIC_POOL_HPP

#include <boost/url/detail/config.hpp>
#include <cstdlib>
#include <type_traits>

namespace boost {
namespace urls {

/** Base class for fixed-storage pool
*/
class basic_static_pool
{
    char* const base_;
    std::size_t const capacity_;
    char* top_;
    std::size_t n_ = 0;

    struct item;

    BOOST_URL_DECL
    void*
    allocate(
        std::size_t bytes,
        std::size_t align);

    BOOST_URL_DECL
    void
    deallocate(
        void* p,
        std::size_t bytes,
        std::size_t align) noexcept;

public:
    /** An allocator using the static pool
    */
    template<class T>
    class allocator_type;

    /** Constructor
    */
    basic_static_pool(
        char* buffer,
        std::size_t size)
        : base_(buffer)
        , capacity_(size)
        , top_(buffer)
    {
    }

    /** Return an allocator which uses the pool.
    */
    inline
    allocator_type<char>
    allocator() noexcept;
};

/** A fixed-size storage pool for allocating memory
*/
template<std::size_t N>
class static_pool : public basic_static_pool
{
    char buf_[N];

public:
    /** Constructor
    */
    static_pool() noexcept
        : basic_static_pool(buf_, N)
    {
    }
};

} // urls
} // boost

#include <boost/url/impl/static_pool.hpp>

#endif

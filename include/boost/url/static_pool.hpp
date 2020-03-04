//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_STATIC_POOL_HPP
#define BOOST_URL_STATIC_POOL_HPP

#include <boost/url/config.hpp>
#include <cstdlib>

namespace boost {
namespace urls {

class basic_static_pool
{
    char* const base_;
    std::size_t const capacity_;
    char* top_;

    template<class T>
    friend class allocator_type;

    void*
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
            BOOST_THROW_EXCEPTION(
                std::bad_alloc());
        top_ = reinterpret_cast<
            char*>(p + bytes);
        return p;
    }

    void
    deallocate(
        void* p,
        std::size_t bytes,
        std::size_t align) noexcept
    {
        (void)p;
        (void)bytes;
        (void)align;
    }

public:
    template<class T>
    class allocator_type
    {
        basic_static_pool* pool_ = nullptr;
   
        template<class U>
        friend class allocator_type;

    public:
        using is_always_equal = std::false_type;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_pointer = T const*;
        using const_reference = T const&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
 
        template<class U>
        struct rebind
        {
            using other = allocator_type<U>;
        };

    #ifndef BOOST_URL_NO_GCC_4_2_WORKAROUND
        // libg++ basic_string requires Allocator to be DefaultConstructible
        // https://code.woboq.org/firebird/include/c++/4.8.2/bits/basic_string.tcc.html#82
        allocator_type() = default;
    #endif

        template<class U>
        allocator_type(
            allocator_type<U> const& other) noexcept
            : pool_(other.pool_)
        {
        }

        explicit
        allocator_type(
            basic_static_pool& pool)
            : pool_(&pool)
        {
        }

        pointer
        allocate(size_type n)
        {
            return reinterpret_cast<T*>(
                pool_->allocate(
                    n * sizeof(T),
                    alignof(T)));
        }

        void
        deallocate(
            pointer p,
            size_type n) noexcept
        {
            pool_->deallocate(p,
                n * sizeof(T),
                alignof(T));
        }

        template<class U>
        bool
        operator==(allocator_type<U> const& other) const noexcept
        {
            return pool_ == other.pool_;
        }

        template<class U>
        bool
        operator!=(allocator_type<U> const& other) const noexcept
        {
            return pool_ != other.pool_;
        }
    };

    basic_static_pool(
        char* buffer,
        std::size_t size)
        : base_(buffer)
        , capacity_(size)
        , top_(buffer)
    {
    }

    allocator_type<char>
    allocator() noexcept
    {
        return allocator_type<char>(*this);
    }
};

template<std::size_t N>
class static_pool : public basic_static_pool
{
    char buf_[N];

public:
    static_pool()
        : basic_static_pool(buf_, N)
    {
    }
};

} // urls
} // boost

#endif

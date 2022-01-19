//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_ANY_ALLOCATOR_HPP
#define BOOST_URL_DETAIL_ANY_ALLOCATOR_HPP

#include <boost/url/detail/type_id.hpp>

#include <boost/core/allocator_access.hpp>
#include <boost/core/empty_value.hpp>
#include <boost/type_traits/type_with_alignment.hpp>

#include <cstddef>

namespace boost {
namespace urls {
namespace detail {

struct any_allocator_base
{
    struct base
    {
        type_id_t type_id;

        virtual ~base() = default;

        virtual
        void*
        allocate(
            std::size_t n,
            std::size_t size,
            std::size_t align) = 0;

        virtual
        void
        deallocate(
            void* p,
            std::size_t n,
            std::size_t size,
            std::size_t align) = 0;

        virtual
        bool
        is_equal(
            const any_allocator_base::base& p) = 0;
    };

    template <typename Allocator>
    struct impl
        : public base
        , private empty_value<
              boost::allocator_rebind_t<
                Allocator, char>>
    {
        using allocator_type =
            boost::allocator_rebind_t<
                Allocator, char>;

        explicit
        impl(allocator_type const& a)
            : empty_value<allocator_type>(
                empty_init_t(), a)
        {
            type_id = get_type_id<
                allocator_type>();
        }

        void*
        allocate(
            std::size_t n,
            std::size_t size,
            std::size_t align) override
        {
            if (align == 1)
                return this->get().allocate(n);
            return allocate_aligned(
                n, size, align);
        }

        void
        deallocate(
            void* p,
            std::size_t n,
            std::size_t size,
            std::size_t align) override
        {
            if (align == 1)
            {
                this->get().deallocate(
                    reinterpret_cast<char*>(p), n);
                return;
            }
            deallocate_aligned(
                p, n, size, align);
        }

        bool
        is_equal(const base& other) override
        {
            auto other_impl = static_cast<
                const impl*>(&other);
            return this->get() ==
                   other_impl->get();
        };

    #if BOOST_WORKAROUND(BOOST_GCC, < 40900)
        using max_align_t = ::max_align_t;
    #else
        using max_align_t = std::max_align_t;
    #endif

        static
        std::size_t
        ceil_div(std::size_t n, std::size_t d)
        {
            return (n + d - 1) / d;
        }

        template <std::size_t N>
        typename std::enable_if<
            alignof(max_align_t) <= N,
            void*>::type
        try_allocate_aligned(
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            using T = max_align_t;
            using A = boost::allocator_rebind_t<
                Allocator, T>;
            return A(this->get()).allocate(
                n * ceil_div(size, align));
        }

        template <std::size_t N>
        typename std::enable_if<
            N<alignof(max_align_t),
            void*>::type
        try_allocate_aligned(
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            if (align <= N)
            {
                using T = typename
                    boost::type_with_alignment<N>::type;
                using A = boost::allocator_rebind_t<
                    Allocator, T>;
                return A(this->get()).allocate(
                    n * ceil_div(size, align));
            }
            return try_allocate_aligned<
                2 * N>(n, size, align);
        }

        void*
        allocate_aligned(
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            return try_allocate_aligned<
                2>(n, size, align);
        }


        template <std::size_t N>
        typename std::enable_if<
            alignof(max_align_t) <= N>::type
        try_deallocate_aligned(
            void* p,
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            using T = max_align_t;
            using A = boost::allocator_rebind_t<
                Allocator, T>;
            A(this->get()).deallocate(
                reinterpret_cast<T*>(p),
                n * ceil_div(size, align));
        }

        template <std::size_t N>
        typename std::enable_if<
            N < alignof(max_align_t)>::type
        try_deallocate_aligned(
            void* p,
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            if (align <= N)
            {
                using T = typename
                    boost::type_with_alignment<N>::type;
                using A = boost::allocator_rebind_t<
                    Allocator, T>;
                A(this->get()).deallocate(
                    reinterpret_cast<T*>(p),
                    n * ceil_div(size, align));
                return;
            }
            try_deallocate_aligned<
                2 * N>(p, n, size, align);
        }

        void
        deallocate_aligned(
            void* p,
            std::size_t n,
            std::size_t size,
            std::size_t align)
        {
            try_deallocate_aligned<
                2>(p, n, size, align);
        }
    };
};

} // namespace detail
} // namespace urls
} // namespace boost


#endif

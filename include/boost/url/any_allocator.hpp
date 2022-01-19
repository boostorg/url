//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_ANY_ALLOCATOR_HPP
#define BOOST_URL_ANY_ALLOCATOR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/detail/any_allocator.hpp>

#include <memory>
#include <new>
#include <type_traits>

namespace boost {
namespace urls {

/** A type-erased allocator with shared ownership.

    This type satisfies the requirements for
    <em>Allocator</em>

    @par Specification
    @li <a href="https://en.cppreference.com/w/cpp/named_req/Allocator"
        >Allocator (cppreference.com)</a>
 */
#ifdef BOOST_URL_DOCS
template <class T>
using any_allocator = __see_below__;
#else
template <class T>
class any_allocator
    : private detail::any_allocator_base
{
    std::shared_ptr<base> p_;

    template <class U>
    friend class any_allocator;

public:
    using value_type      = T;
    using pointer         = value_type*;
    using reference       = value_type&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = value_type const&;
    using const_pointer   = value_type const*;
    using is_always_equal = std::false_type;

    template <class U>
    struct rebind
    {
        using other = any_allocator<U>;
    };

#ifndef BOOST_URL_NO_GCC_4_2_WORKAROUND
    any_allocator()       = default;
#endif

    any_allocator(any_allocator const&) = default;

    template <class U>
    explicit
    any_allocator(
        any_allocator<U> const& other) noexcept
        : p_(other.p_)
    {
    }

    template <class Allocator>
    explicit
    any_allocator(Allocator const& a);

    any_allocator&
    operator=(any_allocator const&) = default;

    T*
    allocate(std::size_t n);

    void
    deallocate(T* p, std::size_t n);

    template <class U, class... Args>
    void
    construct(U* p, Args&&... args)
    {
        ::new (p) U(std::forward<
            Args>(args)...);
    }

    template <class U>
    void
    destroy(U* p)
    {
        p->~U();
    }

    friend
    bool
    operator==(
        const any_allocator& a,
        const any_allocator& b) noexcept
    {
        return (a.p_ == b.p_) || (
            a.p_ != nullptr &&
            b.p_ != nullptr && (
                a.p_->type_id == b.p_->type_id &&
                a.p_->is_equal(*b.p_)));
    }

    friend
    bool
    operator!=(
        const any_allocator& a,
        const any_allocator& b) noexcept
    {
        return !(a == b);
    }
};
#endif

} // namespace urls
} // namespace boost

#include <boost/url/impl/any_allocator.hpp>

#endif

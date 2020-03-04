//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_MAKE_SHARED_HPP
#define BOOST_URL_MAKE_SHARED_HPP

#include <boost/beast/url/url_view.hpp>
#include <boost/beast/core/detail/allocator.hpp>
#include <boost/beast/core/detail/empty_base_optimization.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include <memory>
#include <type_traits>

namespace boost {
namespace beast {
namespace urls {

namespace detail {

template<class T, class Allocator>
class extra_alloc
    : private beast::detail::empty_base_optimization<Allocator>
{
    template<class U, class OtherAlloc>
    friend class extra_alloc;

    std::size_t extra_;

public:
    using is_always_equal = std::false_type;
    using value_type = typename
        beast::detail::allocator_traits<typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<T>>::value_type;
    using pointer = typename
        beast::detail::allocator_traits<typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<T>>::pointer;
    using const_pointer = typename
        beast::detail::allocator_traits<typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<T>>::const_pointer;
    using size_type = typename
        beast::detail::allocator_traits<typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<T>>::size_type;
    using difference_type = typename
        beast::detail::allocator_traits<typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<T>>::difference_type;

    template<class U>
    struct rebind
    {
        using other = extra_alloc<U, Allocator>;
    };

    extra_alloc(
        std::size_t extra,
        Allocator const& alloc)
        : beast::detail::empty_base_optimization<Allocator>(alloc)
        , extra_(extra)
    {
    }

    template<class U>
    extra_alloc(extra_alloc<U, Allocator> const& other) noexcept
        : beast::detail::empty_base_optimization<Allocator>(other.member())
        , extra_(other.extra_)
    {
    }

    pointer
    allocate(size_type n)
    {
        BOOST_ASSERT(n == 1);
        using U = typename boost::type_with_alignment<
            alignof(value_type)>::type;
        auto constexpr S = sizeof(U);
        using A = typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<U>;
        A a{this->member()};
        return reinterpret_cast<pointer>(
            std::allocator_traits<A>::allocate(a,
                (n * sizeof(value_type) + extra_ + S - 1) / S));
    }

    void
    deallocate(pointer p, size_type n)
    {
        BOOST_ASSERT(n == 1);
        using U = typename boost::type_with_alignment<
            alignof(value_type)>::type;
        auto constexpr S = sizeof(U);
        using A = typename beast::detail::allocator_traits<
            Allocator>::template rebind_alloc<U>;
        A a{this->member()};
        std::allocator_traits<A>::deallocate(a,
            reinterpret_cast<U*>(p),
                (n * sizeof(value_type) + extra_ + S - 1) / S);
    }

#if defined(BOOST_LIBSTDCXX_VERSION) && BOOST_LIBSTDCXX_VERSION < 60000
    template<class U, class... Args>
    void
    construct(U* ptr, Args&&... args)
    {
        ::new((void*)ptr) U(std::forward<Args>(args)...);
    }

    template<class U>
    void
    destroy(U* ptr)
    {
        ptr->~U();
    }
#endif

    template<class U>
    friend
    bool
    operator==(
        extra_alloc const& lhs,
        extra_alloc<U, Allocator> const& rhs)
    {
        return
            lhs.member() == rhs.member() &&
            lhs.extra_ == rhs.extra_;
    }

    template<class U>
    friend
    bool
    operator!=(
        extra_alloc const& lhs,
        extra_alloc<U, Allocator> const& rhs)
    {
        return ! (lhs == rhs);
    }
};

struct shared_view_impl : url_view
{
    shared_view_impl(url_view const& v)
        : url_view(v.cparts(),
            reinterpret_cast<
                char const*>(this + 1))
    {
    }
};

} // detail

template<
    class Allocator = std::allocator<char>>
std::shared_ptr<url_view>
make_shared(
    url_view const& v,
    Allocator const& alloc = Allocator{})
{
    using T = detail::shared_view_impl;
    auto p = std::allocate_shared<T>(
        detail::extra_alloc<T, Allocator>(
            v.str().size(), alloc), v);
    std::memcpy(
        reinterpret_cast<char*>(p.get() + 1),
        v.str().data(),
        v.str().size());
    return p;
}

} // urls
} // beast
} // boost

#endif

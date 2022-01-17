//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_STATIC_POOL_HPP
#define BOOST_URL_IMPL_STATIC_POOL_HPP

namespace boost {
namespace urls {

template<class T>
class static_pool_allocator
{
    basic_static_pool* pool_ = nullptr;

    template<class U>
    friend class static_pool_allocator;

public:
    using pointer = T*;
    using reference = T&;
    using value_type = T;
    using size_type = std::size_t;
    using const_pointer = T const*;
    using const_reference = T const&;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::false_type;

    template<class U>
    struct rebind
    {
        using other = static_pool_allocator<U>;
    };

#ifndef BOOST_URL_NO_GCC_4_2_WORKAROUND
    // libg++ basic_string requires Allocator to be DefaultConstructible
    // https://code.woboq.org/firebird/include/c++/4.8.2/bits/basic_string.tcc.html#82
    static_pool_allocator() = default;
#endif

    template<class U>
    static_pool_allocator(
        static_pool_allocator<
            U> const& other) noexcept
        : pool_(other.pool_)
    {
    }

    explicit
    static_pool_allocator(
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
            n * sizeof(T), alignof(T));
    }

    template<class U>
    bool
    operator==(static_pool_allocator<
        U> const& other) const noexcept
    {
        return pool_ == other.pool_;
    }

    template<class U>
    bool
    operator!=(static_pool_allocator<
        U> const& other) const noexcept
    {
        return pool_ != other.pool_;
    }
};

//------------------------------------------------

inline
auto
basic_static_pool::
allocator() noexcept ->
    static_pool_allocator<char>
{
    return static_pool_allocator<
        char>(*this);
}

template<class... Args>
auto
basic_static_pool::
make_string(Args&&... args) ->
    string_type
{
    return std::basic_string<
        char, std::char_traits<char>,
            static_pool_allocator<char>>(
        std::forward<Args>(args)...,
            allocator());
}

} // urls
} // boost

#endif

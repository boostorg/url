//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_STRING_HPP
#define BOOST_URL_IMPL_STRING_HPP

#include <boost/url/detail/over_allocator.hpp>
#include <atomic>
#include <memory>
#include <new>

namespace boost {
namespace urls {

struct const_string::result
{
    base* p;
    char* data;
    std::size_t size;
};

//------------------------------------------------

struct const_string::base
{
    std::atomic<
        std::uint32_t> refs{1};

    void
    release(std::size_t size) noexcept
    {
        if(--refs > 0)
            return;
        destroy(size);
    }

    virtual
    void
    destroy(std::size_t size) noexcept = 0;
};

//------------------------------------------------

class const_string::factory::base
{
public:
    std::atomic<
        std::uint32_t> refs{1};

    void
    release() noexcept
    {
        if(--refs > 0)
            return;
        destroy();
    }

    virtual
    void
    destroy() noexcept = 0;

    virtual
    result
    construct(std::size_t size) const = 0;
};

//------------------------------------------------

template<class Allocator>
class const_string::factory::impl final
    : public base
{
    friend class const_string;

    Allocator a_;

    struct string final : const_string::base
    {
        Allocator a_;

        string(Allocator const& a) noexcept
            : a_(a)
        {
        }

        void
        destroy(std::size_t size) noexcept override
        {
            detail::over_allocator<
                string, Allocator> a(size, a_);
            this->~string();
            a.deallocate(this, 1);
        }
    };

public:
    explicit
    impl(Allocator const& a) noexcept
        : a_(a)
    {
    }

    void
    destroy() noexcept override
    {
        typename detail::allocator_traits<
            Allocator>::template rebind_alloc<
                impl> a(a_);
        this->~impl();
        a.deallocate(this, 1);
    }

    result
    construct(std::size_t size) const override
    {
        // VFALCO Should we do something special when size==0?
        detail::over_allocator<
            string, Allocator> a(size, a_);
        auto p = ::new(a.allocate(1)) string(a_);
        return result{ p, reinterpret_cast<
            char*>(p + 1), size };
    }
};

//------------------------------------------------

template<class Allocator>
const_string::
factory::
factory(Allocator const& a)
    : p_([&a]
    {
        using A = typename
            detail::allocator_traits<
                Allocator>::template rebind_alloc<
                    factory::impl<Allocator>>;
        return ::new(A(a).allocate(1)
            ) factory::impl<Allocator>(a);
    }())
{
}

template<class InitFn>
const_string
const_string::
factory::
operator()(
    std::size_t n,
    InitFn const& init) const
{
    // VFALCO Should we do something special when n==0?
    if (is_small(n))
        return const_string(n,
            std::allocator<char>{}, init);
    auto r = p_->construct(n);
    const_string s(r);
    init(n, r.data);
    return s;
}

//------------------------------------------------

template<class Allocator>
const_string::
const_string(
    string_view s,
    Allocator const& a)
{
    // VFALCO Should we do something special when n==0?
    if (is_small(s.size()))
    {
        std::memcpy(data_.buf_,
            s.data(), s.size());
        static_cast<string_view&>(*this
            ) = { data_.buf_, s.size()};
        return;
    }
    auto r = factory::impl<Allocator>(
        a).construct(s.size());
    static_cast<string_view&>(
        *this) = { r.data, r.size };
    std::memcpy(r.data, s.data(), s.size());
    data_.p_ = r.p;
}

template<
    class Allocator,
    class InitFn>
const_string::
const_string(
    std::size_t size,
    Allocator const& a,
    InitFn const& init)
{
    if (is_small(size))
    {
        init(size, data_.buf_);
        static_cast<string_view&>(
            *this) = { data_.buf_, size };
        return;
    }
    auto r = factory::impl<
        Allocator>(a).construct(size);
    static_cast<string_view&>(
        *this) = { r.data, r.size };
    init(size, r.data);
    data_.p_ = r.p;
}

} // urls
} // boost

#endif

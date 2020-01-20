//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_VALUE_HPP
#define BOOST_URL_VALUE_HPP

#include <boost/url/config.hpp>
#include <boost/url/basic_value.hpp>
#include <memory>
#include <utility>

namespace boost {
namespace url {

namespace detail {

template<class Allocator>
class alloc_storage
    : public storage
{
    Allocator a_;
    char* p_ = nullptr;
    std::size_t size_ = 0;
    std::size_t cap_ = 0;

    using traits =
        std::allocator_traits<
            Allocator>;

public:
    explicit
    alloc_storage(
        Allocator const& a)
        : a_(a)
    {
    }

    ~alloc_storage()
    {
        if(p_)
            traits::deallocate(a_,
                p_, cap_ + 1);
    }

    std::size_t
    capacity() const noexcept override
    {
        return cap_;
    }

    char*
    resize(std::size_t n) override
    {
        if(n <= cap_)
        {
            size_ = n;
            p_[n] = 0;
            return p_;
        }

        std::size_t cap =
            traits::max_size(a_);
        if(cap_ < cap - cap_)
        {
            cap = 2 * cap_;
            if( cap < n)
                cap = n;
        }
        auto p = a_.allocate(cap + 1);
        if(p_)
        {
            std::memcpy(
                p, p_, size_ + 1);
            a_.deallocate(
                p_, cap_ + 1);
        }
        p_ = p;
        cap_ = cap;
        size_ = n;
        return p_;
    }
};

} // detail

template<class Allocator>
class dynamic_value
    : private detail::alloc_storage<Allocator>
    , public basic_value
{
public:
    dynamic_value() noexcept
        : detail::alloc_storage<
            Allocator>(Allocator{})
        , basic_value(static_cast<
            detail::storage&>(*this))
    {
    }

    explicit
    dynamic_value(
        string_view s,
        Allocator const& a = {})
        : detail::alloc_storage<Allocator>(a)
        , basic_value(*this, s)
    {
    }

    explicit
    dynamic_value(
        Allocator const& a) noexcept
        : detail::alloc_storage<Allocator>(a)
        , basic_value(*this)
    {
    }

    dynamic_value(
        dynamic_value&&) noexcept
    {
    }

    dynamic_value(
        dynamic_value const&)
    {
    }

    dynamic_value&
    operator=(
        dynamic_value const&)
    {
        return *this;
    }
};

using value = dynamic_value<
    std::allocator<char>>;

} // url
} // boost

#endif

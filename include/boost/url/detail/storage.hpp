//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_STORAGE_HPP
#define BOOST_URL_DETAIL_STORAGE_HPP

#include <cstdlib>
#include <cstring>
#include <memory>

namespace boost {
namespace urls {
namespace detail {

struct storage
{
    virtual std::size_t capacity() const noexcept = 0;
    [[nodiscard]] virtual char* resize(std::size_t n) = 0;
};

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

    [[nodiscard]]
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
        p_[n] = 0;
        return p_;
    }
};

template<class Allocator>
struct storage_member
{
    alloc_storage<Allocator> st_;

    explicit
    storage_member(
        Allocator const& alloc)
        : st_(alloc)
    {
    }
};

} // detail
} // urls
} // boost

#endif

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

#include <boost/url/config.hpp>
#include <cstdlib>
#include <cstring>
#include <memory>

#include <iostream>

namespace boost {
namespace urls {
namespace detail {

struct storage
{
    virtual std::size_t capacity() const noexcept = 0;
    BOOST_URL_NODISCARD virtual char* reserve(std::size_t n) = 0;

    virtual std::size_t size() const noexcept = 0;
    BOOST_URL_NODISCARD virtual char* resize(std::size_t n) = 0;
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
    using allocator_type = Allocator;

    constexpr allocator_type get_allocator() const noexcept
    {
        return a_;
    }

    explicit
    alloc_storage(
        Allocator const& a)
        : a_(a)
    {
    }

    alloc_storage(alloc_storage const &other)
       : a_(traits::select_on_container_copy_construction(other.a_))
       , p_(nullptr)
       , size_(0)
       , cap_(0)
    {
        p_ = resize(other.size());
        copy(other.p_);
    }

    alloc_storage(alloc_storage && other) noexcept
    {
        destroy();
        a_ = std::move(other.a_);
        p_ = std::move(other.p_);
        size_ = std::move(other.size_);
        cap_ = std::move(other.cap_);
        other.clear();
    }


    alloc_storage& operator=(alloc_storage const &other)
    {
        if (this != &other)
        {
            if (this->get_allocator() != other.get_allocator())
                destroy();
            BOOST_IF_CONSTEXPR(traits::propagate_on_container_copy_assignment::value)
                a_ = other.a_;
            resize(other.size_);
            copy(other.p_);
        }
        return *this;
    }

    alloc_storage& operator=(alloc_storage &&other) noexcept(traits::propagate_on_container_move_assignment::value)
    {
        if (this->get_allocator() != other.get_allocator())
            destroy();
        BOOST_IF_CONSTEXPR (traits::propagate_on_container_move_assignment::value)
            a_ = std::move(other.a_);
        if (this->get_allocator() != other.get_allocator())
        {
            resize(other.size_);
            copy(other.p_);
            other.destroy();
        }
        else 
        {
            destroy();
            cap_ = std::move(other.cap_);
            size_ = std::move(other.size_);
            p_ = std::move(other.p_);
            other.clear();
        }
        return *this;
    }

    ~alloc_storage()
    {
        destroy();
    }

    void copy(char * other_p)
    {
        if (size_)
        {
            std::memcpy(
                p_, other_p, size_ + 1);
            p_[size_] = 0;
        }
    }

    std::size_t
    capacity() const noexcept override
    {
        return cap_;
    }

    BOOST_URL_NODISCARD
    char*
    reserve(std::size_t n) override
    {
        if(n <= cap_)
            return p_;

        std::size_t cap =
            traits::max_size(a_);
        if(cap_ < cap - cap_)
        {
            cap = 2 * cap_;
            if( cap < n)
                cap = n;
        }
        auto p = traits::allocate(a_, cap + 1);
        if(p_)
        {
            std::memcpy(
                p, p_, size_ + 1);
            a_.deallocate(
                p_, cap_ + 1);
            p[size_] = 0;
        }
        p_ = p;
        cap_ = cap;
        return p_;
    }

    std::size_t
    size() const noexcept override
    {
        return size_;
    }

    BOOST_URL_NODISCARD
    char*
    resize(std::size_t n) override
    {
        if( char* p = reserve(n) )
        {
            size_ = n;
            p[n] = 0;
            return p;
        }
        else
            return p;
    }

    void swap(alloc_storage &b) noexcept
    {
        BOOST_IF_CONSTEXPR (traits::propagate_on_container_swap::value)
        {
           using std::swap;
           swap(this->a_, b.a_);
        }
        do_swap(b);
    }

private:

    void destroy()
    {
        if(p_)
        {
            traits::deallocate(a_,
                p_, cap_ + 1);
            clear();
        }
    }

    void clear()
    {
        size_ = 0;
        cap_ = 0;
        p_ = nullptr;
    }

    void do_swap(alloc_storage &b) noexcept
    {
        using std::swap;
        swap(this->p_, b.p_);
        swap(this->size_, b.size_);
        swap(this->cap_, b.cap_);
    }

};


template<typename Allocator>
void swap(alloc_storage<Allocator> &a, alloc_storage<Allocator> &b) noexcept
{
    a.swap(b);
}

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

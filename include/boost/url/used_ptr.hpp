//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_USED_PTR_HPP
#define BOOST_URL_USED_PTR_HPP

#include <boost/url/detail/config.hpp>

#include <cstddef>
#include <mutex>
#include <utility>

namespace boost {
namespace urls {

template<class T>
class used_ptr;

//------------------------------------------------

namespace detail {

template<class T>
class recycled
{
public:
    struct U
    {
        T t;
        U* next = nullptr;
    };

    constexpr recycled() = default;

    ~recycled()
    {
        auto it = head_;
        while(it)
        {
            auto next = it->next;
            delete it;
            it = next;
        }
    }

    void
    release(U* u) noexcept
    {
        m_.lock();
        u->next = head_;
        head_ = u;
        m_.unlock();
    }

    U*
    try_acquire()
    {
        std::lock_guard<
            std::mutex> lock(m_);
        auto p = head_;
        if(p)
        {
            // recycle
            head_ = head_->next;
        }
        return p;
    }

private:
    U* head_ = nullptr;
    std::mutex m_;
};

} // detail

//------------------------------------------------

template<class T>
class used_ptr
{
    friend class detail::recycled<T>;

    using R = detail::recycled<T>;
    using U = typename R::U;

    U* p_ = nullptr;
    R* r_ = nullptr;

public:
    /** Acquire a new or recycled object

        This function acquires exclusive
        access to a recycled object if it
        exists, otherwise it acquires
        a new, default constructed object.

        Upon destruction of the pointer,
        the object is recycled.
    */
    used_ptr()
    {
        // VFALCO this needs the guaranteed
        // constexpr-init macro treatment
        static R r;
        p_ = r.try_acquire();
        if(! p_)
            p_ = new U;
        r_ = &r;
    }

    /** Recycle the pointed-to object
    */
    ~used_ptr()
    {
        if(p_)
            r_->release(p_);
    }

    /** Return the pointed-to object
    */
    T*
    get() const noexcept
    {
        return &p_->t;
    }

    /** Return the pointed-to object
    */
    T*
    operator->() const noexcept
    {
        return get();
    }

    /** Return the pointed-to object
    */
    T&
    operator*() const noexcept
    {
        return *get();
    }
};

} // urls
} // boost

#endif

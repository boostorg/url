//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_RECYCLER_HPP
#define BOOST_URL_DETAIL_RECYCLER_HPP

#include <mutex>
#include <utility>

namespace boost {
namespace urls {
namespace detail {

BOOST_URL_DECL
void
recycler_add_impl(
    std::size_t) noexcept;

BOOST_URL_DECL
void
recycler_remove_impl(
    std::size_t) noexcept;

#ifdef BOOST_URL_REPORT

inline
void
recycler_add(
    std::size_t n) noexcept
{
    recycler_add_impl(n);
}

inline
void
recycler_remove(
    std::size_t n) noexcept
{
    recycler_remove_impl(n);
}

#else

inline void recycler_add(
    std::size_t) noexcept
{
}
inline void recycler_remove(
    std::size_t) noexcept
{
}

#endif

template<class T>
struct recycler
{
    struct report;

    struct U
    {
        T t;
        U* next = nullptr;
    };

    constexpr recycler() = default;

    ~recycler()
    {
        std::size_t n = 0;
        auto it = head_;
        while(it)
        {
            ++n;
            auto next = it->next;
            delete it;
            it = next;
        }
        recycler_remove(
            sizeof(U) * n);
    }

    void
    release(U* u) noexcept
    {
        m_.lock();
        u->next = head_;
        head_ = u;
        m_.unlock();
        recycler_add(sizeof(U));
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
            recycler_remove(sizeof(U));
        }
        return p;
    }

private:
    U* head_ = nullptr;
    std::mutex m_;
};

} // detail
} // urls
} // boost

#endif

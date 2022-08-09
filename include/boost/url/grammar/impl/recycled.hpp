//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_RECYCLED_PTR_HPP
#define BOOST_URL_GRAMMAR_IMPL_RECYCLED_PTR_HPP

namespace boost {
namespace urls {
namespace grammar {

//------------------------------------------------

template<class T>
recycled_ptr<T>::
~recycled_ptr()
{
    if(p_)
        bin_->release(p_);
}

template<class T>
recycled_ptr<T>::
recycled_ptr(recycled<T>& bin)
    : bin_(&bin)
    , p_(bin.try_acquire())
{
    if(! p_)
        p_ = new U;
}

template<class T>
recycled_ptr<T>::
recycled_ptr()
    : recycled_ptr([]() -> B&
        {
            // VFALCO this needs the guaranteed
            // constexpr-init macro treatment
            static B r;
            return r;
        }())
{
}

template<class T>
recycled_ptr<T>::
recycled_ptr(
    recycled_ptr&& other) noexcept
    : bin_(other.bin_)
    , p_(other.p_)
{
    other.p_ = nullptr;
}

//------------------------------------------------

template<class T>
recycled<T>::
~recycled()
{
    std::size_t n = 0;
    // VFALCO we should probably deallocate
    // in reverse order of allocation but
    // that requires a doubly-linked list.
    auto it = head_;
    while(it)
    {
        ++n;
        auto next = it->next;
        delete it;
        it = next;
    }
    detail::recycled_remove(
        sizeof(U) * n);
}

template<class T>
auto
recycled<T>::
try_acquire() ->
    U*
{
    std::lock_guard<
        std::mutex> lock(m_);
    auto p = head_;
    if(p)
    {
        // recycle
        head_ = head_->next;
        detail::recycled_remove(sizeof(U));
    }
    return p;
}

template<class T>
void
recycled<T>::
release(U* u) noexcept
{
    m_.lock();
    u->next = head_;
    head_ = u;
    m_.unlock();
    detail::recycled_add(sizeof(U));
}

} // grammar
} // urls
} // boost

#endif

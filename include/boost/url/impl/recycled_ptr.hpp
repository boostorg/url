//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_RECYCLED_PTR_HPP
#define BOOST_URL_IMPL_RECYCLED_PTR_HPP

namespace boost {
namespace urls {

//------------------------------------------------

template<class T>
recycled_ptr<T>::
recycled_ptr()
{
    // VFALCO this needs the guaranteed
    // constexpr-init macro treatment
    static R r;
    p_ = r.try_acquire();
    if(! p_)
        p_ = new U;
    r_ = &r;
}

template<class T>
recycled_ptr<T>::
~recycled_ptr()
{
    if(p_)
        r_->release(p_);
}

template<class T>
recycled_ptr<T>::
recycled_ptr(
    recycled_ptr&& other) noexcept
    : p_(other.p_)
    , r_(other.r_)
{
    other.p_ = nullptr;
}

} // urls
} // boost

#endif

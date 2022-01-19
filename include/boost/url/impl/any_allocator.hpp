//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ANY_ALLOCATOR_HPP
#define BOOST_URL_IMPL_ANY_ALLOCATOR_HPP

#include <boost/config/workaround.hpp>

#include <boost/assert.hpp>
#include <boost/type_traits/make_void.hpp>

#include <cstddef>
#include <new>

namespace boost {
namespace urls {

template <class T>
template <class Allocator>
any_allocator<T>::
any_allocator(Allocator const& a)
{
    using char_allocator_type = boost::
        allocator_rebind_t<Allocator, char>;
    using impl_allocator_type = boost::
        allocator_rebind_t<
            Allocator,
            impl<char_allocator_type>>;
    p_ = std::allocate_shared<
        impl<char_allocator_type>>(
        impl_allocator_type(a),
        char_allocator_type(a));
}

template <class T>
T*
any_allocator<T>::
allocate(
    std::size_t n)
{
    if (p_)
        return reinterpret_cast<T*>(
            p_->allocate(n,
                         sizeof(T),
                         alignof(T)));
    return std::allocator<T>().allocate(n);
}

template <class T>
void
any_allocator<T>::
deallocate(
    T* p,
    std::size_t n)
{
    if (p_)
        return p_->deallocate(p,
                              n,
                              sizeof(T),
                              alignof(T));
    return std::allocator<T>().deallocate(p, n);
}

} // urls
} // boost


#endif

//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RECYCLED_PTR_HPP
#define BOOST_URL_RECYCLED_PTR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/detail/recycler.hpp>
#include <cstddef>

namespace boost {
namespace urls {

template<class T>
class recycled_ptr
{
    friend struct detail::recycler<T>;

    using R = detail::recycler<T>;
    using U = typename R::U;

    U* p_ = nullptr;
    R* r_ = nullptr;

public:
    /** Acquire a new or recycler object

        This function acquires exclusive
        access to a recycler object if it
        exists, otherwise it acquires
        a new, default constructed object.

        Upon destruction of the pointer,
        the object is recycler.
    */
    recycled_ptr();

    /** Recycle the pointed-to object
    */
    ~recycled_ptr();

    /** Constructor

        After the move, the only valid operation
        on the moved-from object is destruction.
    */
    recycled_ptr(
        recycled_ptr&& other) noexcept;

    /** Assignment (deleted)
    */
    recycled_ptr&
    operator=(recycled_ptr&&) = delete;

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

#include <boost/url/impl/recycled_ptr.hpp>

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_OPTIONAL_HPP
#define BOOST_URL_OPTIONAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>
#include <boost/none.hpp>
#include <boost/optional.hpp>

#include <utility>

namespace boost {
namespace urls {

#if 1

template<class T>
using optional = boost::optional<T>;

#else

/** A simplified C++11 optional
*/
template<class T>
class optional
{
    union
    {
        T t_;
    };

    bool has_value_ = false;

public:
    using value_type = T;

    ~optional()
    {
        if(has_value_)
            t_.~T();
    }

    optional() noexcept
    {
    }

    optional(
        none_t) noexcept
        : optional()
    {
    }

    optional(
        optional<T> const& t) noexcept
        : has_value_(t.has_value())
    {
        if(has_value_)
            ::new(&t_) T(*t);
    }

    optional(
        T const& t) noexcept
        : t_(t)
        , has_value_(true)
    {
    }

    T const*
    operator->() const noexcept
    {
        BOOST_ASSERT(
            has_value_);
        return &t_;
    }

    T*
    operator->() noexcept
    {
        BOOST_ASSERT(
            has_value_);
        return &t_;
    }

    T const&
    operator*() const noexcept
    {
        BOOST_ASSERT(
            has_value_);
        return t_;
    }

    T&
    operator*() noexcept
    {
        BOOST_ASSERT(
            has_value_);
        return t_;
    }

    T&
    value()
    {
        if(! has_value_)
            detail::throw_invalid_argument(
                "empty optional",
                BOOST_CURRENT_LOCATION);
        return t_;
    }

    T const&
    value() const
    {
        if(! has_value_)
            detail::throw_invalid_argument(
                "empty optional",
                BOOST_CURRENT_LOCATION);
        return t_;
    }

    explicit
    operator bool() const noexcept
    {
        return has_value_;
    }

    bool
    has_value() const noexcept
    {
        return has_value_;
    }

    void
    reset() noexcept
    {
        if(has_value_)
        {
            t_.~T();
            has_value_ = false;
        }
    }

    template<class... Args>
    T&
    emplace(Args&&... args) noexcept
    {
        if(has_value_)
            t_.~T();
        ::new(&t_) T(std::forward<
            Args>(args)...);
        has_value_ = true;
        return t_;
    }
};

//--------------------------------------

template<class T, class U>
bool
operator==(
    optional<T> const& t,
    optional<U> const& u) noexcept
{
    if(! t.has_value())
        return ! u.has_value();
    if(! u.has_value())
        return false;
    return *t == *u;
}

template<class T, class U>
bool
operator!=(
    optional<T> const& t,
    optional<U> const& u) noexcept
{
    return !(t == u);
}

//--------------------------------------

template<class T>
bool
operator==(
    optional<T> const& t,
    none_t) noexcept
{
    return ! t.has_value();
}

template<class T>
bool
operator!=(
    optional<T> const& t,
    none_t) noexcept
{
    return t.has_value();
}

template<class T>
bool
operator==(
    none_t,
    optional<T> const& t) noexcept
{
    return ! t.has_value();
}

template<class T>
bool
operator!=(
    none_t,
    optional<T> const& t) noexcept
{
    return t.has_value();
}

//--------------------------------------

template<class T, class U>
bool
operator==(
    optional<T> const& t,
    U const& u) noexcept
{
    if(! t.has_value())
        return false;
    return *t == u;
}

template<class T, class U>
bool
operator!=(
    optional<T> const& t,
    U const& u) noexcept
{
    if(! t.has_value())
        return true;
    return *t != u;
}

template<class T, class U>
bool
operator==(
    T const& t,
    optional<U> const& u) noexcept
{
    if(! u.has_value())
        return false;
    return t == *u;
}

template<class T, class U>
bool
operator!=(
    T const& t,
    optional<U> const& u) noexcept
{
    if(! u.has_value())
        return true;
    return t != *u;
}

#endif

} // urls
} // boost

#endif

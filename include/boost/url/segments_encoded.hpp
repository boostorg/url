//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_ENCODED_HPP
#define BOOST_URL_SEGMENTS_ENCODED_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>
#include <iterator>

namespace boost {
namespace urls {

#ifndef BOOST_JSON_DOCS
class url;
#endif

class segments_encoded
{
    url* u_ = nullptr;

    friend class url;

    explicit
    segments_encoded(
        url& u) noexcept
        : u_(&u)
    {
    }

public:
    class iterator;
    class reference;
    class const_iterator;
    class const_reference;

    using value_type = std::string;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = reference*;
    using const_pointer = const_reference const*;

    //--------------------------------------------

    // element access

    inline
    reference
    at(std::size_t i) noexcept;

    inline
    const_reference
    at(std::size_t i) const noexcept;

    inline
    reference
    operator[](std::size_t i) noexcept;

    inline
    const_reference
    operator[](std::size_t i) const noexcept;

    inline
    const_reference
    front() const;

    inline
    reference
    front();

    inline
    const_reference
    back() const;

    inline
    reference
    back();

    // iterators

    inline
    iterator
    begin() noexcept;

    inline
    const_iterator
    begin() const noexcept;

    inline
    const_iterator
    cbegin() const noexcept;

    inline
    iterator
    end() noexcept;

    inline
    const_iterator
    end() const noexcept;

    inline
    const_iterator
    cend() const noexcept;

    // capacity

    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    BOOST_URL_DECL
    std::size_t
    size() const noexcept;

    // modifiers

    BOOST_URL_DECL
    void
    clear() noexcept;
};

//------------------------------------------------

class segments_encoded::
    const_reference
{
    url const* u_ = nullptr;
    std::size_t i_ = 0;

    friend class segments_encoded;
    friend class const_iterator;

    const_reference(
        url const& u,
        std::size_t i) noexcept
        : u_(&u)
        , i_(i)
    {
    }

public:
    BOOST_URL_DECL
    operator
    string_view() const noexcept;

    operator
    std::string() const noexcept
    {
        return std::string(
            string_view(*this));
    }

#ifdef BOOST_URL_HAS_STRING_VIEW
    operator
    std::string_view() const noexcept
    {
        auto s = string_view(*this);
        return { s.data(), s.size() };
    }
#endif

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator==(
        const_reference const& x1,
        T const& x2 ) noexcept
    {
        return
            string_view(x1) ==
            to_string_view(x2);
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator==(
        T const& x1,
        const_reference x2 ) noexcept
    {
        return
            to_string_view(x1) ==
            string_view(x2);
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator!=(
        const_reference const& x1,
        T const& x2 ) noexcept
    {
        return !( x1 == x2 );
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator!=(
        T const& x1,
        const_reference const& x2 ) noexcept
    {
        return !(x1 == x2);
    }
};

//------------------------------------------------

class segments_encoded::
    reference
{
    url* u_ = nullptr;
    std::size_t i_ = 0;

    friend class segments_encoded;
    friend class iterator;

    reference(
        url& u,
        std::size_t i) noexcept
        : u_(&u)
        , i_(i)
    {
    }

public:
    reference&
    operator=(reference const& other)
    {
        *this = string_view(other);
        return *this;
    }

    reference&
    operator=(const_reference const& other)
    {
        *this = string_view(other);
        return *this;
    }

    template<class T>
#ifdef BOOST_JSON_DOCS
    reference&
#else
    typename std::enable_if<
        is_stringish<T>::value,
            reference&>::type
#endif
    operator=(T const& t)
    {
        *this = to_string_view(t);
        return *this;
    }

    BOOST_URL_DECL
    reference&
    operator=(string_view);

    //---

    BOOST_URL_DECL
    operator
    string_view() const noexcept;

    operator
    std::string() const noexcept
    {
        return std::string(
            string_view(*this));
    }

#ifdef BOOST_URL_HAS_STRING_VIEW
    operator
    std::string_view() const noexcept
    {
        auto s = string_view(*this);
        return { s.data(), s.size() };
    }
#endif

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator==(
        reference const& x1,
        T const& x2 ) noexcept
    {
        return
            string_view(x1) ==
            to_string_view(x2);
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator==(
        T const& x1,
        reference x2 ) noexcept
    {
        return
            to_string_view(x1) ==
            string_view(x2);
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator!=(
        reference const& x1,
        T const& x2 ) noexcept
    {
        return !( x1 == x2 );
    }

    template<class T
#ifndef BOOST_JSON_DOCS
        , class = typename std::enable_if<
            is_stringish<T>::value,
                bool>::type
#endif
    >
    friend
    bool
    operator!=(
        T const& x1,
        reference const& x2 ) noexcept
    {
        return !( x1 == x2);
    }

    BOOST_URL_DECL
    friend
    void
    swap(
        reference r0,
        reference r1);
};

//------------------------------------------------

class segments_encoded::iterator
{
    url* u_ = nullptr;
    std::size_t i_ = 0;

    friend class segments_encoded;

    iterator(
        url& u,
        std::size_t i) noexcept
        : u_(&u)
        , i_(i)
    {
    }

public:
    using value_type = std::string;
    using reference =
        segments_encoded::reference;
    using pointer = void*;
    using const_reference =
        segments_encoded::const_reference;
    using const_pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    iterator&
    operator++() noexcept
    {
        ++i_;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator&
    operator--() noexcept
    {
        --i_;
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    reference
    operator*() const noexcept
    {
        return reference(*u_, i_);
    }

    friend
    bool
    operator==(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ == b.u_ &&
            a.i_ == b.i_;
    }

    friend
    bool
    operator!=(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ != b.u_ ||
            a.i_ != b.i_;
    }

    // LegacyRandomAccessIterator

    iterator&
    operator+=(ptrdiff_t n) noexcept
    {
        i_ += n;
        return *this;
    }

    friend
    iterator
    operator+(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return { *it.u_, it.i_ + n };
    }

    friend
    iterator
    operator+(
        ptrdiff_t n,
        iterator it) noexcept
    {
        return { *it.u_, it.i_ + n };
    }

    iterator&
    operator-=(ptrdiff_t n) noexcept
    {
        i_ -= n;
        return *this;
    }

    friend
    iterator
    operator-(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return { *it.u_, it.i_ - n };
    }

    friend
    std::ptrdiff_t
    operator-(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return static_cast<std::ptrdiff_t>(
            b.i_) - a.i_;
    }

    reference
    operator[](ptrdiff_t n)
    {
        return *(*this + n);
    }

    friend
    bool
    operator<(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.i_ < b.i_;
    }

    friend
    bool
    operator>(
        iterator a,
        iterator b)
    {
        return b < a;
    }

    friend
    bool
    operator>=(
        iterator a,
        iterator b)
    {
        return !(a < b);
    }

    friend
    bool
    operator<=(
        iterator a,
        iterator b)
    {
        return !(a > b);
    }
};

//------------------------------------------------

class segments_encoded::const_iterator
{
    url const* u_ = nullptr;
    std::size_t i_ = 0;

    friend class segments_encoded;

    const_iterator(
        url const& u,
        std::size_t i) noexcept
        : u_(&u)
        , i_(i)
    {
    }

public:
    using value_type = std::string;
    using const_reference =
        segments_encoded::const_reference;
    using const_pointer = void const*;
    using reference = const_reference;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    const_iterator(
        iterator const& it) noexcept
        : u_(it.u_)
        , i_(it.i_)
    {
    }

    const_iterator&
    operator++() noexcept
    {
        ++i_;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_iterator&
    operator--() noexcept
    {
        --i_;
        return *this;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    const_reference
    operator*() const noexcept
    {
        return const_reference(*u_, i_);
    }

    friend
    bool
    operator==(
        const_iterator a,
        const_iterator b) noexcept
    {
        return a.u_ == b.u_ &&
            a.i_ == b.i_;
    }

    friend
    bool
    operator!=(
        const_iterator a,
        const_iterator b) noexcept
    {
        return a.u_ != b.u_ ||
            a.i_ != b.i_;
    }

    // LegacyRandomAccessIterator

    const_iterator&
    operator+=(ptrdiff_t n) noexcept
    {
        i_ += n;
        return *this;
    }

    friend
    const_iterator
    operator+(
        const_iterator it,
        ptrdiff_t n) noexcept
    {
        return { *it.u_, it.i_ + n };
    }

    friend
    const_iterator
    operator+(
        ptrdiff_t n,
        const_iterator it) noexcept
    {
        return { *it.u_, it.i_ + n };
    }

    const_iterator&
    operator-=(ptrdiff_t n) noexcept
    {
        i_ -= n;
        return *this;
    }

    friend
    const_iterator
    operator-(
        const_iterator it,
        ptrdiff_t n) noexcept
    {
        return { *it.u_, it.i_ - n };
    }

    friend
    std::ptrdiff_t
    operator-(
        const_iterator a,
        const_iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return static_cast<std::ptrdiff_t>(
            b.i_) - a.i_;
    }

    reference
    operator[](ptrdiff_t n)
    {
        return *(*this + n);
    }

    friend
    bool
    operator<(
        const_iterator a,
        const_iterator b)
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.i_ < b.i_;
    }

    friend
    bool
    operator>(
        const_iterator a,
        const_iterator b)
    {
        return b < a;
    }

    friend
    bool
    operator>=(
        const_iterator a,
        const_iterator b)
    {
        return !(a < b);
    }

    friend
    bool
    operator<=(
        const_iterator a,
        const_iterator b)
    {
        return !(a > b);
    }
};

//------------------------------------------------

auto
segments_encoded::
at(std::size_t i) noexcept ->
    reference
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[i];
}

auto
segments_encoded::
at(std::size_t i) const noexcept ->
    const_reference
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return { *u_, i };
}

auto
segments_encoded::
operator[](std::size_t i) noexcept ->
    reference
{
    return { *u_, i };
}

auto
segments_encoded::
operator[](std::size_t i) const noexcept ->
    const_reference
{
    return { *u_, i };
}

auto
segments_encoded::
front() const ->
    const_reference
{
    return { *u_, 0 };
}

auto
segments_encoded::
front() ->
    reference
{
    return { *u_, 0 };
}

auto
segments_encoded::
back() const ->
    const_reference
{
    return { *u_, size() - 1 };
}

auto
segments_encoded::
back() ->
    reference
{
    return { *u_, size() - 1 };
}

auto
segments_encoded::
begin() noexcept ->
    iterator
{
    return iterator(*u_, 0);
}

auto
segments_encoded::
begin() const noexcept ->
    const_iterator
{
    return const_iterator(*u_, 0);
}

auto
segments_encoded::
cbegin() const noexcept ->
    const_iterator
{
    return const_iterator(*u_, 0);
}

auto
segments_encoded::
end() noexcept ->
    iterator
{
    return iterator(*u_, size());
}

auto
segments_encoded::
end() const noexcept ->
    const_iterator
{
    return const_iterator(*u_, size());
}

auto
segments_encoded::
cend() const noexcept ->
    const_iterator
{
    return const_iterator(*u_, size());
}

} // urls
} // boost

#include <boost/url/impl/segments_encoded.hpp>

#endif

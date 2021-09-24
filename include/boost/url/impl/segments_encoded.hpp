//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_HPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/except.hpp>
#include <iterator>

namespace boost {
namespace urls {

/** A proxy to a path segment
*/
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
    /** Assignment
    */
    reference&
    operator=(reference const& other)
    {
        *this = string_view(other);
        return *this;
    }

    /** Assignment
    */
    inline
    reference&
    operator=(const_reference const& other);

    /** Assignment

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T>
#ifdef BOOST_URL_DOCS
    reference&
#else
    typename std::enable_if<
        is_stringlike<T>::value,
            reference&>::type
#endif
    operator=(T const& t)
    {
        *this = to_string_view(t);
        return *this;
    }

    /** Assignment
    */
    BOOST_URL_DECL
    reference&
    operator=(string_view);

    //---

    /** Conversion
    */
    BOOST_URL_DECL
    operator
    string_view() const noexcept;

    /** Conversion
    */
    operator
    std::string() const noexcept
    {
        return std::string(
            string_view(*this));
    }

#ifdef BOOST_URL_HAS_STRING_VIEW
    /** Conversion
    */
    operator
    std::string_view() const noexcept
    {
        auto s = string_view(*this);
        return { s.data(), s.size() };
    }
#endif

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value &&
            ! std::is_same<reference, T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value &&
            ! std::is_same<reference, T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value,
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

    /** Swap two elements
    */
    BOOST_URL_DECL
    friend
    void
    swap(
        reference r0,
        reference r1);
};

//------------------------------------------------

/** A proxy to a path segment
*/
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
    const_reference(
        reference const& other) noexcept
        : u_(other.u_)
        , i_(other.i_)
    {
    }

    /** Conversion
    */
    BOOST_URL_DECL
    operator
    string_view() const noexcept;

    /** Conversion
    */
    operator
    std::string() const noexcept
    {
        return std::string(
            string_view(*this));
    }

#ifdef BOOST_URL_HAS_STRING_VIEW
    /** Conversion
    */
    operator
    std::string_view() const noexcept
    {
        auto s = string_view(*this);
        return { s.data(), s.size() };
    }
#endif

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value &&
            ! std::is_same<reference, T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value &&
            ! std::is_same<reference, T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value,
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

    /** Comparison

        This function participates in overload
        resolution only if
        `is_stringlike<T>::value == true`.
    */
    template<class T
#ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value,
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
    using const_reference =
        segments_encoded::const_reference;
    using pointer = void*;
    using const_pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    iterator() = default;

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
            a.i_) - b.i_;
    }

    reference
    operator[](ptrdiff_t n) const
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

/** A random-access iterator referencing segments in a url path
*/
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
    using reference = const_reference;
    using pointer = void const*;
    using const_pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    const_iterator() = default;

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
            a.i_) - b.i_;
    }

    const_reference
    operator[](ptrdiff_t n) const
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
//
// Members
//
//------------------------------------------------

template<class FwdIt>
typename std::enable_if<
    is_stringlike<typename
        std::iterator_traits<
            FwdIt>::value_type>::value,
    void>::type
segments_encoded::
assign(FwdIt first, FwdIt last)
{
    u_->edit_segments(
        0, size(),
        make_fwdit(first),
        make_fwdit(last),
        make_fwdit(first));
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
segments_encoded::
at(std::size_t i) ->
    reference
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[i];
}

auto
segments_encoded::
at(std::size_t i) const ->
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

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

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

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

template<class FwdIt>
auto
segments_encoded::
insert(
    const_iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    u_->edit_segments(
        before.i_,
        before.i_,
        make_fwdit(first),
        make_fwdit(last),
        make_fwdit(first));
    return { *u_, before.i_ };
}

template<class T>
auto
segments_encoded::
insert(
    const_iterator before,
    std::initializer_list<T> init) ->
        typename std::enable_if<
            is_stringlike<T>::value,
            iterator>::type
{
    return insert(before,
        init.begin(), init.end());
}

void
segments_encoded::
clear() noexcept
{
    erase(begin(), end());
}

template<class T, class>
auto
segments_encoded::
insert(
    const_iterator before,
    T const& t) ->
        iterator
{
    return insert(before,
        to_string_view(t));
}

template<class FwdIt>
auto
segments_encoded::
insert(
    const_iterator before,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            is_stringlike<typename
                std::iterator_traits<
                    FwdIt>::value_type>::value,
            iterator>::type
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

auto
segments_encoded::
erase(
    const_iterator pos) noexcept ->
        iterator
{
    return erase(pos, pos + 1);
}

void
segments_encoded::
push_back(
    string_view s)
{
    insert(end(), s);
}

void
segments_encoded::
pop_back() noexcept
{
    erase(end() - 1);
}

//------------------------------------------------

auto
segments_encoded::
reference::
operator=(
    const_reference const& other) ->
        reference&
{
    *this = string_view(other);
    return *this;
}

inline
bool
operator==(
    segments_encoded::
        const_reference const& c,
    segments_encoded::
        reference const& r) noexcept
{
    return string_view(c) == string_view(r);
}

inline
bool
operator==(
    segments_encoded::
        reference const& r,
    segments_encoded::
        const_reference const& c) noexcept
{
    return string_view(r) == string_view(c);
}

inline
bool
operator!=(
    segments_encoded::
        const_reference const& c,
    segments_encoded::
        reference const& r) noexcept
{
    return string_view(c) != string_view(r);
}

inline
bool
operator!=(
    segments_encoded::
        reference const& r,
    segments_encoded::
        const_reference const& c) noexcept
{
    return string_view(r) != string_view(c);
}

} // urls
} // boost

#endif

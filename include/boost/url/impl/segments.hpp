//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_HPP
#define BOOST_URL_IMPL_SEGMENTS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/any_path_iter.hpp>
#include <boost/url/detail/copied_strings.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/optional_allocator.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/assert.hpp>
#include <iterator>
#include <new>

namespace boost {
namespace urls {

/** A proxy to a path segment
*/
template<class Allocator>
class segments<Allocator>::
    reference
{
    url* u_ = nullptr;
    std::size_t i_ = 0;
    Allocator a_;

    friend class segments;
    friend class iterator;

    reference(
        url& u,
        std::size_t i,
        Allocator const& a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

    string_type<Allocator>
    operator*() const noexcept
    {
        return urls::pct_decode(
            u_->encoded_segment(i_),
                pchars, {}, a_);
    }

public:
    /** Assignment
    */
    reference&
    operator=(
        reference const& other)
    {
        if( u_ == other.u_ &&
            i_ == other.i_)
            return *this;
        auto s = *other;
        u_->edit_segments(
            i_, i_ + 1,
            detail::make_plain_segs_iter(
                &s, &s + 1),
            detail::make_plain_segs_iter(
                &s, &s + 1));
        return *this;
    }

    /** Assignment
    */
    reference&
    operator=(
        const_reference const& other)
    {
        if( u_ == other.u_ &&
            i_ == other.i_)
            return *this;
        auto s = *other;
        u_->edit_segments(
            i_, i_ + 1,
            detail::make_plain_segs_iter(
                &s, &s + 1),
            detail::make_plain_segs_iter(
                &s, &s + 1));
        return *this;
    }

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
    reference&
    operator=(string_view s)
    {
        u_->edit_segments(
            i_, i_ + 1,
            detail::make_plain_segs_iter(
                &s, &s + 1),
            detail::make_plain_segs_iter(
                &s, &s + 1));
        return *this;
    }

    //---

    /** Conversion
    */
    operator
    value_type() const noexcept
    {
        return **this;
    }

    /** Swap two elements
    */
    template<class Alloc_>
    friend
    void
    swap(
        typename segments<Alloc_>::reference& r0,
        typename segments<Alloc_>::reference& r1);

    //--------------------------------------------

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator==(
        reference const& x1,
        T const& x2 ) noexcept
    {
        return *x1 == to_string_view(x2);
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator==(
        T const& x1,
        reference const& x2) noexcept
    {
        return to_string_view(x1) == *x2;
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator!=(
        reference const& x1,
        T const& x2 ) noexcept
    {
        return !(x1 == x2);
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator!=(
        T const& x1,
        reference const& x2) noexcept
    {
        return !(x1 == x2);
    }
};

//------------------------------------------------

/** A proxy to a path segment
*/
template<class Allocator>
class segments<Allocator>::
    const_reference
{
    url const* u_ = nullptr;
    std::size_t i_ = 0;
    Allocator a_;

    friend class segments;
    friend class const_iterator;

    const_reference(
        url const& u,
        std::size_t i,
        Allocator const& a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

    string_type<Allocator>
    operator*() const noexcept
    {
        return urls::pct_decode(
            u_->encoded_segment(i_),
                pchars, {}, a_);
    }

public:
    const_reference(
        reference const& other) noexcept
        : u_(other.u_)
        , i_(other.i_)
        , a_(other.a_)
    {
    }

    /** Conversion
    */
    operator
    value_type() const noexcept
    {
        return **this;
    }

    //--------------------------------------------
    //
    // Comparison
    //
    //--------------------------------------------

    friend
    bool
    operator==(
        const_reference const& x1,
        const_reference const& x2) noexcept
    {
        return *x1 == *x2;
    }

    friend
    bool
    operator!=(
        const_reference const& x1,
        const_reference const& x2) noexcept
    {
        return *x1 != *x2;
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator==(
        const_reference const& x1,
        T const& x2 ) noexcept
    {
        return *x1 == to_string_view(x2);
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator==(
        T const& x1,
        const_reference const& x2) noexcept
    {
        return to_string_view(x1) == *x2;
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator!=(
        const_reference const& x1,
        T const& x2 ) noexcept
    {
        return !(x1 == x2);
    }

    template<class T
    #ifndef BOOST_URL_DOCS
        , class = typename std::enable_if<
            is_stringlike<T>::value>::type
    #endif
    >
    friend
    bool
    operator!=(
        T const& x1,
        const_reference const& x2) noexcept
    {
        return !(x1 == x2);
    }
};

//------------------------------------------------

template<class Allocator>
class segments<Allocator>::iterator
{
    url* u_ = nullptr;
    std::size_t i_ = 0;
    detail::optional_allocator<
        Allocator> a_;

    friend class segments;

    iterator(
        url& u,
        std::size_t i,
        Allocator const& a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

public:
    using value_type =
        string_type<Allocator>;
    using reference = segments<
        Allocator>::reference;
    using const_reference = segments<
        Allocator>::const_reference;
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
        return reference(*u_, i_, *a_);
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
        return { *it.u_, it.i_ + n, *it.a_ };
    }

    friend
    iterator
    operator+(
        ptrdiff_t n,
        iterator it) noexcept
    {
        return { *it.u_, it.i_ + n, *it.a_ };
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
        return { *it.u_, it.i_ - n, *it.a_ };
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
template<class Allocator>
class segments<Allocator>::const_iterator
{
    url const* u_ = nullptr;
    std::size_t i_ = 0;
    detail::optional_allocator<
        Allocator> a_;

    friend class segments;

    const_iterator(
        url const& u,
        std::size_t i,
        Allocator const& a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

public:
    using value_type =
        string_type<Allocator>;
    using const_reference = segments<
        Allocator>::const_reference;
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
        , a_(it.a_)
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
        return const_reference(*u_, i_, *a_);
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
        return { *it.u_, it.i_ + n, *it.a_ };
    }

    friend
    const_iterator
    operator+(
        ptrdiff_t n,
        const_iterator it) noexcept
    {
        return { *it.u_, it.i_ + n, *it.a_ };
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
        return { *it.u_, it.i_ - n, *it.a_ };
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

template<class Allocator>
template<class FwdIt>
typename std::enable_if<
    is_stringlike<typename
        std::iterator_traits<
            FwdIt>::value_type>::value,
    void>::type
segments<Allocator>::
assign(FwdIt first, FwdIt last)
{
    u_->edit_segments(
        0,
        size(),
        detail::make_plain_segs_iter(
            first, last),
        detail::make_plain_segs_iter(
            first, last));
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

template<class Allocator>
auto
segments<Allocator>::
at(std::size_t i) ->
    reference
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[i];
}

template<class Allocator>
auto
segments<Allocator>::
at(std::size_t i) const ->
    const_reference
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[i];
}

template<class Allocator>
auto
segments<Allocator>::
operator[](std::size_t i) noexcept ->
    reference
{
    return { *u_, i, a_ };
}

template<class Allocator>
auto
segments<Allocator>::
operator[](std::size_t i) const noexcept ->
    const_reference
{
    return { *u_, i, a_ };
}

template<class Allocator>
auto
segments<Allocator>::
front() const ->
    const_reference
{
    return { *u_, 0, a_ };
}

template<class Allocator>
auto
segments<Allocator>::
front() ->
    reference
{
    return { *u_, 0, a_ };
}

template<class Allocator>
auto
segments<Allocator>::
back() const ->
    const_reference
{
    return { *u_, size() - 1, a_ };
}

template<class Allocator>
auto
segments<Allocator>::
back() ->
    reference
{
    return { *u_, size() - 1, a_ };
}

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

template<class Allocator>
auto
segments<Allocator>::
begin() noexcept ->
    iterator
{
    return iterator(
        *u_, 0, a_);
}

template<class Allocator>
auto
segments<Allocator>::
begin() const noexcept ->
    const_iterator
{
    return const_iterator(
        *u_, 0, a_);
}

template<class Allocator>
auto
segments<Allocator>::
cbegin() const noexcept ->
    const_iterator
{
    return const_iterator(
        *u_, 0, a_);
}

template<class Allocator>
auto
segments<Allocator>::
end() noexcept ->
    iterator
{
    return iterator(
        *u_, size(), a_);
}

template<class Allocator>
auto
segments<Allocator>::
end() const noexcept ->
    const_iterator
{
    return const_iterator(
        *u_, size(), a_);
}

template<class Allocator>
auto
segments<Allocator>::
cend() const noexcept ->
    const_iterator
{
    return const_iterator(
        *u_, size(), a_);
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

template<class Allocator>
std::size_t
segments<Allocator>::
size() const noexcept
{
    return u_->segment_count();
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

template<class Allocator>
void
segments<Allocator>::
clear() noexcept
{
    erase(begin(), end());
}

template<class Allocator>
auto
segments<Allocator>::
insert(
    const_iterator before,
    string_view s) ->
        iterator
{
    BOOST_ASSERT(before.u_ == u_);
    u_->edit_segments(
        before.i_,
        before.i_,
        detail::make_plain_segs_iter(
            &s, &s + 1),
        detail::make_plain_segs_iter(
            &s, &s + 1));
    return { *u_, before.i_, a_ };
}

template<class Allocator>
template<class FwdIt>
auto
segments<Allocator>::
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
        detail::make_plain_segs_iter(
            first, last),
        detail::make_plain_segs_iter(
            first, last));
    return { *u_, before.i_, a_ };
}

template<class Allocator>
template<class T>
auto
segments<Allocator>::
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

template<class Allocator>
template<class T, class>
auto
segments<Allocator>::
insert(
    const_iterator before,
    T const& t) ->
        iterator
{
    return insert(before,
        to_string_view(t));
}

template<class Allocator>
template<class FwdIt>
auto
segments<Allocator>::
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

template<class Allocator>
auto
segments<Allocator>::
erase(
    const_iterator pos) noexcept ->
        iterator
{
    return erase(pos, pos + 1);
}

template<class Allocator>
auto
segments<Allocator>::
segments::
erase(
    const_iterator first,
    const_iterator last) noexcept ->
        iterator
{
    BOOST_ASSERT(first.u_ == u_);
    BOOST_ASSERT(last.u_ == u_);
    string_view s;
    u_->edit_segments(
        first.i_, last.i_,
        detail::make_enc_segs_iter(&s, &s),
        detail::make_enc_segs_iter(&s, &s));
    return { *u_, first.i_, a_ };
}

template<class Allocator>
void
segments<Allocator>::
push_back(
    string_view s)
{
    insert(end(), s);
}

template<class Allocator>
void
segments<Allocator>::
pop_back() noexcept
{
    erase(end() - 1);
}

//------------------------------------------------
//
// nested types
//
//------------------------------------------------

template<class Alloc>
void
swap(
    typename segments<Alloc>::reference& r0,
    typename segments<Alloc>::reference& r1)
{
    detail::copied_strings cs(
        r0.u_->encoded_url());
    auto tmp = cs.maybe_copy(
        string_view(r0));
    r0 = r1;
    r1 = tmp;
}

} // urls
} // boost

#endif

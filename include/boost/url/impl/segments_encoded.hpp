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
#include <boost/url/string_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/any_path_iter.hpp>
#include <iterator>

namespace boost {
namespace urls {

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
    using reference = string_view;
    using pointer = void const*;
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

    BOOST_URL_DECL
    string_view
    operator*() const noexcept;

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

    string_view
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
//
// Members
//
//------------------------------------------------

inline
segments_encoded::
segments_encoded(
    url& u) noexcept
    : u_(&u)
{
}

inline
bool
segments_encoded::
is_absolute() const noexcept
{
    return
        u_->len(id_path) != 0 &&
        u_->s_[u_->offset(id_path)] == '/';
}

template<class Allocator>
segments
segments_encoded::
decoded(Allocator const& alloc) const
{
    return segments(*u_, alloc);
}

inline
segments_encoded&
segments_encoded::
operator=(std::initializer_list<string_view> init)
{
    assign( init.begin(), init.end() );
    return *this;
}

template<class FwdIt>
auto
segments_encoded::
assign(
    FwdIt first, FwdIt last) ->
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::value_type,
            string_view>::value>::type
{
    u_->edit_segments(
        0,
        size(),
        detail::make_enc_segs_iter(first, last),
        detail::make_enc_segs_iter(first, last));
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

inline
string_view
segments_encoded::
at(std::size_t i) const
{
    if(i >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[i];
}

inline
string_view
segments_encoded::
front() const noexcept
{
    BOOST_ASSERT(! empty());
    return (*this)[0];
}

inline
string_view
segments_encoded::
back() const noexcept
{
    BOOST_ASSERT(! empty());
    return (*this)[size() - 1];
}

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

inline
auto
segments_encoded::
begin() const noexcept ->
    iterator
{
    return iterator(*u_, 0);
}

inline
auto
segments_encoded::
end() const noexcept ->
    iterator
{
    return iterator(*u_, size());
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
bool
segments_encoded::
empty() const noexcept
{
    return size() == 0;
}

inline
std::size_t
segments_encoded::
size() const noexcept
{
    return u_->nseg_;
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

inline
void
segments_encoded::
clear() noexcept
{
    erase(begin(), end());
}

//------------------------------------------------

inline
auto
segments_encoded::
insert(
    iterator before,
    std::initializer_list<
            string_view> init) ->
        iterator
{
    return insert(
        before,
        init.begin(),
        init.end());
}

template<class FwdIt>
auto
segments_encoded::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::value_type,
                string_view>::value,
            iterator>::type
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

template<class FwdIt>
auto
segments_encoded::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    u_->edit_segments(
        before.i_,
        before.i_,
        detail::make_enc_segs_iter(
            first, last),
        detail::make_enc_segs_iter(
            first, last));
    return { *u_, before.i_ };
}

//------------------------------------------------

inline
auto
segments_encoded::
replace(
    iterator pos,
    string_view s) ->
        iterator
{
    return replace(
        pos, pos + 1,
            &s, &s + 1);
}

inline
auto
segments_encoded::
replace(
    iterator from,
    iterator to,
    std::initializer_list<
        string_view> init) ->
    iterator
{
    return replace(
        from,
        to,
        init.begin(),
        init.end());
}

template<class FwdIt>
auto
segments_encoded::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::value_type,
                string_view>::value,
            iterator>::type
{
    BOOST_ASSERT(from.u_ == u_);
    BOOST_ASSERT(to.u_ == u_);
    u_->edit_segments(
        from.i_,
        to.i_,
        detail::make_enc_segs_iter(first, last),
        detail::make_enc_segs_iter(first, last));
    return from;
}

//------------------------------------------------

inline
auto
segments_encoded::
erase(
    iterator pos) noexcept ->
        iterator
{
    return erase(pos, pos + 1);
}

//------------------------------------------------

inline
void
segments_encoded::
push_back(
    string_view s)
{
    insert(end(), s);
}

inline
void
segments_encoded::
pop_back() noexcept
{
    erase(end() - 1);
}

} // urls
} // boost

#endif

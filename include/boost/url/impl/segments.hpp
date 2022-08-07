//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
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
#include <boost/url/string_view.hpp>
#include <boost/url/detail/any_path_iter.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/segments_iterator_impl.hpp>
#include <boost/url/grammar/detail/copied_strings.hpp>
#include <boost/assert.hpp>
#include <iterator>
#include <new>

namespace boost {
namespace urls {

//------------------------------------------------

class segments::iterator
{
    detail::segments_iterator_impl impl_;

    friend class segments;

    iterator(
        string_view s,
        std::size_t nseg) noexcept
        : impl_(s, nseg)
    {
    }

    // end ctor
    iterator(
        string_view s,
        std::size_t nseg,
        int) noexcept
        : impl_(s, nseg, 0)
    {
    }

public:
    using value_type = std::string;
    using reference = pct_encoded_view;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() noexcept = default;

    iterator(iterator const&) noexcept = default;

    iterator&
    operator=(iterator const&) noexcept = default;

    reference
    operator*() const noexcept
    {
        return impl_.dereference();
    }

    iterator&
    operator++() noexcept
    {
        impl_.increment();
        return *this;
    }

    iterator&
    operator--() noexcept
    {
        impl_.decrement();
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return impl_.equal(other.impl_);
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !impl_.equal(other.impl_);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------

inline
bool
segments::
is_absolute() const noexcept
{
    return
        u_->u_.len(id_path) != 0 &&
        u_->s_[u_->u_.offset(id_path)] == '/';
}

inline
segments&
segments::
operator=(std::initializer_list<
    string_view> init)
{
    assign(init.begin(), init.end());
    return *this;
}

template<class FwdIt>
auto
segments::
assign(FwdIt first, FwdIt last) ->
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::value_type,
            string_view>::value>::type
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

inline
auto
segments::
front() const ->
    pct_encoded_view
{
    BOOST_ASSERT(! empty());
    return *begin();
}

inline
auto
segments::
back() const ->
    pct_encoded_view
{
    BOOST_ASSERT(! empty());
    return *std::prev(end());
}

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

inline
auto
segments::
begin() const noexcept ->
    iterator
{
    return iterator(
        u_->encoded_path(), u_->u_.nseg_);
}

inline
auto
segments::
end() const noexcept ->
    iterator
{
    return iterator(
        u_->encoded_path(), u_->u_.nseg_, 0);
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
bool
segments::
empty() const noexcept
{
    return size() == 0;
}

inline
std::size_t
segments::
size() const noexcept
{
    return u_->u_.nseg_;
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

inline
void
segments::
clear() noexcept
{
    erase(begin(), end());
}

//------------------------------------------------

inline
auto
segments::
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
segments::
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
segments::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    u_->edit_segments(
        before.impl_.i_,
        before.impl_.i_,
        detail::make_plain_segs_iter(
            first, last),
        detail::make_plain_segs_iter(
            first, last));
    return std::next(begin(), before.impl_.i_);
}

//------------------------------------------------

inline
auto
segments::
replace(
    iterator pos,
    string_view s) ->
        iterator
{
    return replace(
        pos, std::next(pos),
            &s, &s + 1);
}

inline
auto
segments::
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
segments::
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
    BOOST_ASSERT(from.impl_.begin_ >= u_->string().data());
    BOOST_ASSERT(from.impl_.end_ <= u_->string().data() +
        u_->string().size());
    BOOST_ASSERT(to.impl_.begin_ >= u_->string().data());
    BOOST_ASSERT(to.impl_.end_ >= u_->string().data() +
        u_->string().size());
    u_->edit_segments(
        from.impl_.i_,
        to.impl_.i_,
        detail::make_plain_segs_iter(
            first, last),
        detail::make_plain_segs_iter(
            first, last));
    return std::next(begin(), from.impl_.i_);
}

//------------------------------------------------

inline
auto
segments::
erase(
    iterator pos) noexcept ->
        iterator
{
    return erase(pos, std::next(pos));
}

//------------------------------------------------

inline
void
segments::
push_back(
    string_view s)
{
    insert(end(), s);
}

inline
void
segments::
pop_back() noexcept
{
    erase(std::prev(end()));
}

} // urls
} // boost

#endif

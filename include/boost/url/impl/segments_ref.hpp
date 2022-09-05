//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_REF_HPP
#define BOOST_URL_IMPL_SEGMENTS_REF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/segments_base.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/any_segments_iter.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>
#include <iterator>
#include <new>

namespace boost {
namespace urls {

inline
segments_ref::
segments_ref(
    url_base& u) noexcept
    : segments_base(
        detail::path_ref(u.u_))
    , u_(&u)
{
}

inline
segments_ref&
segments_ref::
operator=(segments_ref const& other)
{
    assign(other.begin(), other.end());
    return *this;
}

inline
segments_ref&
segments_ref::
operator=(segments_view const& other)
{
    assign(other.begin(), other.end());
    return *this;
}

inline
segments_ref&
segments_ref::
operator=(std::initializer_list<
    string_view> init)
{
    assign(init.begin(), init.end());
    return *this;
}

inline
segments_ref::
operator
segments_view() const noexcept
{
    return segments_view(ref_);
}

inline
void
segments_ref::
clear() noexcept
{
    erase(begin(), end());
}

template<class FwdIt>
auto
segments_ref::
assign(FwdIt first, FwdIt last) ->
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value>::type
{
    u_->edit_segments(
        begin().it_,
        end().it_,
        detail::make_segments_iter(
            first, last));
}

//------------------------------------------------

inline
auto
segments_ref::
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
segments_ref::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::reference,
                string_view>::value,
            iterator>::type
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

template<class FwdIt>
auto
segments_ref::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    u_->edit_segments(
        before.it_,
        before.it_,
        detail::make_segments_iter(
            first, last));
    return std::next(begin(), before.it_.index);
}

//------------------------------------------------

inline
auto
segments_ref::
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
segments_ref::
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
segments_ref::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::reference,
                string_view>::value,
            iterator>::type
{
    u_->edit_segments(
        from.it_,
        to.it_,
        detail::make_segments_iter(
            first, last));
    return std::next(begin(), from.it_.index);
}

//------------------------------------------------

inline
auto
segments_ref::
erase(
    iterator pos) noexcept ->
        iterator
{
    return erase(pos, std::next(pos));
}

//------------------------------------------------

inline
void
segments_ref::
push_back(
    string_view s)
{
    insert(end(), s);
}

inline
void
segments_ref::
pop_back() noexcept
{
    erase(std::prev(end()));
}

} // urls
} // boost

#endif

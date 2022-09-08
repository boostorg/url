//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_REF_HPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_REF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/detail/segments_iter_impl.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/any_segments_iter.hpp>
#include <iterator>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

inline
void
segments_encoded_ref::
clear() noexcept
{
    erase(begin(), end());
}

template<class FwdIt>
auto
segments_encoded_ref::
assign(FwdIt first, FwdIt last) ->
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            pct_string_view>::value>::type
{
    u_->edit_segments(
        begin().it_,
        end().it_,
        detail::make_segments_encoded_iter(
            first, last));
}

template<class FwdIt>
auto
segments_encoded_ref::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::reference,
                pct_string_view>::value,
            iterator>::type
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

inline
auto
segments_encoded_ref::
erase(
    iterator pos) noexcept ->
        iterator
{
    return erase(pos, std::next(pos));
}

template<class FwdIt>
auto
segments_encoded_ref::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        typename std::enable_if<
            std::is_convertible<typename
                std::iterator_traits<
                    FwdIt>::reference,
                pct_string_view>::value,
            iterator>::type
{
    return u_->edit_segments(
        from.it_,
        to.it_,
        detail::make_segments_encoded_iter(
            first, last));
}

//------------------------------------------------

inline
void
segments_encoded_ref::
push_back(
    pct_string_view s)
{
    insert(end(), s);
}

inline
void
segments_encoded_ref::
pop_back() noexcept
{
    erase(std::prev(end()));
}

//------------------------------------------------

template<class FwdIt>
auto
segments_encoded_ref::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    return u_->edit_segments(
        before.it_,
        before.it_,
        detail::make_segments_encoded_iter(
            first, last));
}

} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_IPP
#define BOOST_URL_IMPL_SEGMENTS_IPP

#include <boost/url/segments.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

string_value
segments::
iterator::
operator*() const
{
    auto s = u_->encoded_segment(i_);
    auto n = detail::
        pct_decode_size_unchecked(s);
    char* dest;
    auto v =
        a_.make_string_value(n, dest);
    detail::pct_decode_unchecked(
        dest, dest + n, s, {});
    return v;
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
segments::
operator[](
    std::size_t i) const ->
    string_value
{
    auto s = u_->encoded_segment(i);
    auto n = detail::
        pct_decode_size_unchecked(s);
    char* dest;
    auto v =
        a_.make_string_value(n, dest);
    detail::pct_decode_unchecked(
        dest, dest + n, s, {});
    return v;
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

auto
segments::
insert(
    iterator before,
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

auto
segments::
segments::
erase(
    iterator first,
    iterator last) noexcept ->
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

} // urls
} // boost

#endif

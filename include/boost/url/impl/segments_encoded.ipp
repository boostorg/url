//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_IPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_IPP

#include <boost/url/segments_encoded.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/copied_strings.hpp>

namespace boost {
namespace urls {

string_view
segments_encoded::
iterator::
operator*() const noexcept
{
    BOOST_ASSERT(i_ < u_->nseg_);
    auto r = u_->segment(i_);
    if(u_->cs_[r.pos] == '/')
        return { u_->cs_ + r.pos + 1,
            r.len - 1 };
    return { u_->cs_ + r.pos, r.len };
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
segments_encoded::
operator[](
    std::size_t i) const noexcept ->
    reference
{
    BOOST_ASSERT(i < u_->nseg_);
    auto r = u_->segment(i);
    if(u_->cs_[r.pos] == '/')
        return { u_->cs_ + r.pos + 1,
            r.len - 1 };
    return { u_->cs_ + r.pos, r.len };
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

auto
segments_encoded::
insert(
    iterator before,
    string_view s0) ->
        iterator
{
    BOOST_ASSERT(before.u_ == u_);
    detail::copied_strings cs(
        u_->encoded_url());
    auto s = cs.maybe_copy(s0);
    u_->edit_segments(
        before.i_,
        before.i_,
        detail::make_enc_segs_iter(
            &s, &s + 1),
        detail::make_enc_segs_iter(
            &s, &s + 1));
    return { *u_, before.i_ };
}

auto
segments_encoded::
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
    return { *u_, first.i_ };
}

} // urls
} // boost

#endif

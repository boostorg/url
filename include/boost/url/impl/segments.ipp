//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
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
#include <boost/url/detail/path.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

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
    BOOST_ASSERT(
        before.impl_.pos_ >=
        u_->string().data());
    BOOST_ASSERT(
        before.impl_.pos_ <=
        u_->string().data() +
        u_->string().size());
    grammar::detail::copied_strings<
        BOOST_URL_STACK_BYTES> cs(
            u_->string());
    s = cs.maybe_copy(s);
    u_->edit_segments(
        before.impl_.i_,
        before.impl_.i_,
        detail::make_plain_segs_iter(
            &s, &s + 1),
        detail::make_plain_segs_iter(
            &s, &s + 1));
    return std::next(begin(), before.impl_.i_);
}

auto
segments::
segments::
erase(
    iterator first,
    iterator last) noexcept ->
        iterator
{
    BOOST_ASSERT(first.impl_.pos_ >= u_->string().data());
    BOOST_ASSERT(last.impl_.pos_ >= u_->string().data());
    BOOST_ASSERT(first.impl_.pos_ <= u_->string().data() +
        u_->string().size());
    BOOST_ASSERT(last.impl_.pos_ <= u_->string().data() +
        u_->string().size());
    string_view s;
    u_->edit_segments(
        first.impl_.i_, last.impl_.i_,
        detail::make_enc_segs_iter(&s, &s),
        detail::make_enc_segs_iter(&s, &s));
    return std::next(begin(), first.impl_.i_);
}

} // urls
} // boost

#endif

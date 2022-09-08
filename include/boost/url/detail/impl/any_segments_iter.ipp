//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ANY_SEGMENTS_ITER_IPP
#define BOOST_URL_DETAIL_IMPL_ANY_SEGMENTS_ITER_IPP

#include <boost/url/detail/any_segments_iter.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/decode.hpp>
#include <boost/url/encode.hpp>
#include <boost/url/rfc/pchars.hpp>

namespace boost {
namespace urls {
namespace detail {

//------------------------------------------------
//
// path_iter
//
//------------------------------------------------

path_iter::
path_iter(
    string_view s) noexcept
    : any_segments_iter(&s_)
    , s_(s)
{
    rewind();
}

void
path_iter::
increment() noexcept
{
    pos_ = next_;
    if(pos_ == s_.size())
    {
        pos_ = string_view::npos;
        return;
    }
    // skip '/'
    ++pos_;
    auto const end =
        s_.data() + s_.size();
    auto const p0 =
        s_.data() + pos_;
    auto p = p0;
    while(p != end)
    {
        if(*p == '/')
        {
            next_ = p - s_.data();
            return;
        }
        ++p;
    }
    next_ = s_.size();
}

void
path_iter::
rewind() noexcept
{
    pos_ = 0;
    auto p0 = s_.data();
    auto const end = p0 + s_.size();
    if(p0 != end)
    {
        // skip leading '/'
        // of absolute-path
        if(*p0 == '/')
        {
            ++p0;
            ++pos_;
        }
        auto p = p0;
        while(p != end)
        {
            if(*p == '/')
                break;
            ++p;
        }
        front_ = string_view(
            p0, p - p0);
        next_ = p - s_.data();
    }
    else
    {
        pos_ = string_view::npos;
        front_ = { p0, 0 };
    }
}

bool
path_iter::
measure(
    std::size_t& n) noexcept
{
    if(pos_ == string_view::npos)
        return false;
    string_view s = s_.substr(
        pos_, next_ - pos_);
    encode_opts opt;
    n += urls::encoded_size(
        s,
        opt,
        pchars);
    increment();
    return true;
}

void
path_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(pos_ !=
        string_view::npos);
    string_view s = s_.substr(
        pos_, next_ - pos_);
    encode_opts opt;
    dest += encode(
        dest,
        end,
        s,
        opt,
        pchars);
    increment();
}

//------------------------------------------------
//
// path_encoded_iter
//
//------------------------------------------------

path_encoded_iter::
path_encoded_iter(
    pct_string_view s) noexcept
    : path_iter(s)
{
}

bool
path_encoded_iter::
measure(
    std::size_t& n) noexcept
{
    if(pos_ == string_view::npos)
        return false;
    string_view s = s_.substr(
        pos_, next_ - pos_);
    encode_opts opt;
    n += detail::re_encoded_size_unchecked(
        s,
        opt,
        pchars);
    increment();
    return true;
}

void
path_encoded_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(pos_ !=
        string_view::npos);
    string_view s = s_.substr(
        pos_, next_ - pos_);
    encode_opts opt;
    detail::re_encode_unchecked(
        dest,
        end,
        s,
        opt,
        pchars);
    increment();
}

//------------------------------------------------
//
// segments_iter_base
//
//------------------------------------------------

void
segments_iter_base::
measure_impl(
    string_view s,
    std::size_t& n) noexcept
{
    encode_opts opt;
    n += encoded_size(
        s,
        opt,
        pchars);
}

void
segments_iter_base::
copy_impl(
    string_view s,
    char*& dest,
    char const* end) noexcept
{
    encode_opts opt;
    dest += encode(
        dest,
        end,
        s,
        opt,
        pchars);
}

//------------------------------------------------
//
// segments_encoded_iter_base
//
//------------------------------------------------

bool
segments_encoded_iter_base::
measure_impl(
    pct_string_view s,
    std::size_t& n) noexcept
{
    encode_opts opt;
    n += detail::re_encoded_size_unchecked(
        s,
        opt,
        pchars);
    return true;
}

void
segments_encoded_iter_base::
copy_impl(
    string_view s,
    char*& dest,
    char const* end) noexcept
{
    encode_opts opt;
    detail::re_encode_unchecked(
        dest,
        end,
        s,
        opt,
        pchars);
}

} // detail
} // urls
} // boost

#endif

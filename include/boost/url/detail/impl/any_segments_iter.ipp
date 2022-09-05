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
    , s_(clean(s))
    , p0_(s.data())
    , end_(s.data() + s.size())
{
    rewind();
    front = { p_, n_ };
}

string_view
path_iter::
clean(string_view s) noexcept
{
    // prevent null
    if(s.data() == nullptr)
        return string_view("", 0);
    return s;
}

void
path_iter::
increment() noexcept
{
    p_ += n_;
    if(p_ == end_)
    {
        p_ = nullptr;
        return;
    }
    ++p_;
    string_view s(p_, end_ - p_);
    auto pos = s.find_first_of('/');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

void
path_iter::
rewind() noexcept
{
    if(p0_ != end_)
    {
        auto p0 = p0_;
        if(*p0 == '/')
            ++p0;
        p_ = p0;
        auto p = p0;
        while(p != end_)
        {
            if(*p == '/')
                break;
            ++p;
        }
        n_ = p - p0;
    }
}

bool
path_iter::
measure(
    std::size_t& n) noexcept
{
    if(! p_)
        return false;
    string_view s(p_, n_);
    n += urls::encoded_size(
        s, {}, pchars);
    increment();
    return true;
}

void
path_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(p_ != nullptr);
    dest += encode(
        dest,
        end,
        string_view(p_, n_),
        {},
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
    : any_segments_iter(&s_)
    , s_(clean(s))
    , p0_(s.data())
    , end_(s.data() + s.size())
{
    rewind();
    front = { p_, n_ };
}

string_view
path_encoded_iter::
clean(pct_string_view s) noexcept
{
    // prevent null
    if(s.data() == nullptr)
        return string_view("", 0);
    return s;
}

void
path_encoded_iter::
increment() noexcept
{
    p_ += n_;
    if(p_ == end_)
    {
        p_ = nullptr;
        return;
    }
    ++p_;
    string_view s(p_, end_ - p_);
    auto pos = s.find_first_of('/');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

void
path_encoded_iter::
rewind() noexcept
{
    if(p0_ != end_)
    {
        auto p0 = p0_;
        if(*p0 == '/')
            ++p0;
        p_ = p0;
        auto p = p0;
        while(p != end_)
        {
            if(*p == '/')
                break;
            ++p;
        }
        n_ = p - p0;
    }
}

bool
path_encoded_iter::
measure(
    std::size_t& n) noexcept
{
    if(! p_)
        return false;
    string_view s(p_, n_);
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
    (void)end;
    BOOST_ASSERT(static_cast<
        std::size_t>(
            end - dest) >= n_);
    BOOST_ASSERT(p_ != nullptr);
    if(n_ > 0)
    {
        string_view s(p_, n_);
        encode_opts opt;
        detail::re_encode_unchecked(
            dest,
            end,
            s,
            opt,
            pchars);
    }
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
    n += encoded_size(s, {}, pchars);
}

void
segments_iter_base::
copy_impl(
    string_view s,
    char*& dest,
    char const* end) noexcept
{
    dest += encode(
        dest, end, s, {}, pchars);
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

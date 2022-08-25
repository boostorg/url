 //
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ANY_PATH_ITER_IPP
#define BOOST_URL_DETAIL_IMPL_ANY_PATH_ITER_IPP

#include <boost/url/detail/any_path_iter.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/decode.hpp>
#include <boost/url/encode.hpp>
#include <boost/url/rfc/pchars.hpp>

namespace boost {
namespace urls {
namespace detail {

any_path_iter::
~any_path_iter() noexcept = default;

//------------------------------------------------

void
enc_path_iter::
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

enc_path_iter::
enc_path_iter(
    string_view s) noexcept
    : end_(s.data() + s.size())
{
    if(s.empty())
    {
        n_ = 0;
        p_ = nullptr;
        return;
    }
    std::size_t pos;
    if(s.starts_with('/'))
        s.remove_prefix(1);
    pos = s.find_first_of('/');
    p_ = s.data();
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
    front = { p_, n_ };
}

bool
enc_path_iter::
measure(
    std::size_t& n,
    error_code& ec) noexcept
{
    if(! p_)
        return false;
    string_view s(p_, n_);
    auto rn = urls::decode(s, {}, pchars);
    if( !rn )
    {
        ec = rn.error();
        return false;
    }
    n += s.size();
    increment();
    return true;
}

void
enc_path_iter::
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
        std::memcpy(
            dest, p_, n_);
        dest += n_;
    }
    increment();
}

//------------------------------------------------

void
plain_path_iter::
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

plain_path_iter::
plain_path_iter(
    string_view s) noexcept
    : end_(s.data() + s.size())
{
    if(s.empty())
    {
        n_ = 0;
        p_ = nullptr;
        return;
    }
    std::size_t pos;
    if(s.starts_with('/'))
        s.remove_prefix(1);
    pos = s.find_first_of('/');
    p_ = s.data();
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
    front = { p_, n_ };
}

bool
plain_path_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
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
plain_path_iter::
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

void
view_path_iter::
increment() noexcept
{
    std::advance(p_, n_);
    if(p_ == end_)
    {
        done_ = true;
        return;
    }
    ++p_;
    auto pos = p_;
    n_ = 0;
    while (pos != end_)
    {
        if (*pos == '/')
            break;
        ++pos;
        ++n_;
    }
}

view_path_iter::
view_path_iter(
    decode_view s) noexcept
    : n_(0)
    , end_(s.end())
{
    if(s.empty())
    {
        p_ = s.end();
        done_ = true;
        return;
    }
    p_ = s.begin();
    if (!s.empty() && s.front() == '/')
        ++p_;
    auto pos = p_;
    while (pos != end_)
    {
        if (*pos == '/')
            break;
        ++pos;
        ++n_;
    }
    front = { p_.base(), pos.base() };
}

bool
view_path_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if (done_)
        return false;
    auto it = p_;
    auto end = std::next(p_, n_);
    n += encoded_size_impl(it, end, {}, pchars);
    increment();
    return true;
}

void
view_path_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(!done_);
    auto it = p_;
    auto last = std::next(p_, n_);
    dest += encode_impl(
        dest, end, it, last, {}, pchars);
    increment();
}

//------------------------------------------------

bool
enc_segs_iter_base::
measure_impl(
    string_view s,
    std::size_t& n,
    error_code& ec) noexcept
{
    auto rn = urls::decode(s, {}, pchars);
    if( !rn )
    {
        ec = rn.error();
        return false;
    }
    n += s.size();
    return true;
}

void
enc_segs_iter_base::
copy_impl(
    string_view s,
    char*& dest,
    char const* end) noexcept
{
    (void)end;
    BOOST_ASSERT(static_cast<
        std::size_t>(end - dest) >=
            s.size());
    if(! s.empty())
    {
        std::memcpy(dest,
            s.data(), s.size());
        dest += s.size();
    }
}

//------------------------------------------------

void
plain_segs_iter_base::
measure_impl(
    string_view s,
    std::size_t& n) noexcept
{
    n += encoded_size(s, {}, pchars);
}

void
plain_segs_iter_base::
copy_impl(
    string_view s,
    char*& dest,
    char const* end) noexcept
{
    dest += encode(
        dest, end, s, {}, pchars);
}

} // detail
} // urls
} // boost

#endif

//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ANY_QUERY_ITER_IPP
#define BOOST_URL_DETAIL_IMPL_ANY_QUERY_ITER_IPP

#include <boost/url/detail/any_query_iter.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/detail/charsets.hpp>

namespace boost {
namespace urls {
namespace detail {

any_query_iter::
~any_query_iter() noexcept = default;

//------------------------------------------------

void
enc_query_iter::
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
    auto pos = s.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

enc_query_iter::
enc_query_iter(
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
    pos = s.find_first_of('&');
    p_ = s.data();
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

bool
enc_query_iter::
measure(
    std::size_t& n,
    error_code& ec) noexcept
{
    if(! p_)
        return false;
    string_view s(p_, n_);
    urls::validate_pct_encoding(
        s, ec, query_chars, {});
    if(ec.failed())
        return false;
    n += s.size();
    increment();
    return true;
}

void
enc_query_iter::
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
plain_query_iter::
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
    auto pos = s.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

plain_query_iter::
plain_query_iter(
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
    pos = s.find_first_of('&');
    p_ = s.data();
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

bool
plain_query_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(! p_)
        return false;
    string_view s(p_, n_);
    n += urls::pct_encode_bytes(
        s, query_chars);
    increment();
    return true;
}

void
plain_query_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(p_ != nullptr);
    dest += pct_encode(
        dest, end,
        string_view(p_, n_),
        query_chars);
    increment();
}

//------------------------------------------------

bool
enc_params_iter_base::
measure_impl(
    string_view key,
    string_view const* value,
    std::size_t& n,
    error_code& ec) noexcept
{
    pct_decode_opts opt;
    opt.plus_to_space = true;
    validate_pct_encoding(
        key, ec, query_chars, opt);
    if(ec.failed())
        return false;
    n += key.size();
    if(value)
    {
        validate_pct_encoding(
            *value, ec, query_chars, opt);
        if(ec.failed())
            return false;
        n += 1 + value->size();
    }
    return ! ec.failed();
}

void
enc_params_iter_base::
copy_impl(
    string_view key,
    string_view const* value,
    char*& dest,
    char const* end) noexcept
{
    (void)end;
    // avoid self-copy
    std::size_t kn = key.size();
    BOOST_ASSERT(end - kn >= dest);
    if( key.data() != dest &&
        key.data() != nullptr)
    {
        std::memcpy(dest,
            key.data(), kn);
    }
    dest += kn;
    if(value)
    {
        BOOST_ASSERT(
            end - 1 >= dest);
        *dest++ = '=';
        std::size_t vn =
            value->size();
        BOOST_ASSERT(
            end - vn >= dest);
        if(vn > 0)
        {
            std::memcpy(dest,
                value->data(), vn);
            dest += vn;
        }
    }
}

//------------------------------------------------

void
plain_params_iter_base::
measure_impl(
    string_view key,
    string_view const* value,
    std::size_t& n) noexcept
{
    n += pct_encode_bytes(key, query_chars);
    if(value)
    {
        ++n; // '='
        n += pct_encode_bytes(*value, query_chars);
    }
}

void
plain_params_iter_base::
copy_impl(
    string_view key,
    string_view const* value,
    char*& dest,
    char const* end) noexcept
{
    dest += pct_encode(
        dest, end, key, query_chars);
    if(value)
    {
        *dest++ = '=';
        dest += pct_encode(
            dest, end, *value, query_chars);
    }
}

//------------------------------------------------

void
plain_value_iter_base::
measure_impl(
    string_view key,
    string_view const* value,
    std::size_t& n) noexcept
{
    n += key.size();
    if(value)
    {
        ++n; // '='
        n += pct_encode_bytes(*value, query_chars);
    }
}

void
plain_value_iter_base::
copy_impl(
    string_view key,
    string_view const* value,
    char*& dest,
    char const* end) noexcept
{
    // avoid self-copy
    std::size_t n = key.size();
    BOOST_ASSERT(end - n >= dest);
    // iterator for value only
    BOOST_ASSERT(key.data() == dest);
    dest += n;
    if(value)
    {
        *dest++ = '=';
        dest += pct_encode(
            dest, end, *value, query_chars);
    }
}

} // detail
} // urls
} // boost

#endif

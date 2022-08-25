//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ANY_PARAMS_ITER_IPP
#define BOOST_URL_DETAIL_IMPL_ANY_PARAMS_ITER_IPP

#include <boost/url/detail/any_params_iter.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/decode.hpp>
#include <boost/url/rfc/detail/charsets.hpp>

namespace boost {
namespace urls {
namespace detail {

/*
    When a string is transformed into a range of
    params, the empty string becomes ambiguous:
    it can be an empty range, or a range with
    one param. The value `not_empty` is used on
    construction to inform the transformation
    that the empty string should be treated as
    a one-element range. This simplifies
    edit_params().
*/

//------------------------------------------------
//
// any_params_iter
//
//------------------------------------------------

any_params_iter::
~any_params_iter() noexcept = default;

//------------------------------------------------
//
// encoded_query_iter
//
//------------------------------------------------

encoded_query_iter::
encoded_query_iter(
    string_view s,
    not_empty_param ne) noexcept
    : any_params_iter(
        s.empty() && ! ne)
    , s_(clean(s))
{
    BOOST_ASSERT(s_.data());
    rewind();
}

void
encoded_query_iter::
rewind() noexcept
{
    if(empty())
    {
        p_ = nullptr;
        return;
    }
    p_ = s_.begin();
    auto pos =
        s_.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s_.size();
}

bool
encoded_query_iter::
measure(
    std::size_t& n,
    error_code& ec) noexcept
{
    if(! p_)
        return false;
    // validate
    string_view s(p_, n_);
    auto rv = urls::decode(
        s, {}, query_chars);
    if(! rv)
    {
        ec = rv.error();
        return false;
    }
    n += s.size();
    increment();
    return true;
}

void
encoded_query_iter::
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

void
encoded_query_iter::
increment() noexcept
{
    p_ += n_;
    if(p_ == s_.end())
    {
        p_ = nullptr;
        return;
    }
    ++p_;
    string_view s(p_, s_.end() - p_);
    auto pos = s.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

//------------------------------------------------
//
// query_iter
//
//------------------------------------------------

query_iter::
query_iter(
    string_view s,
    not_empty_param ne) noexcept
    : any_params_iter(
        s.empty() && ! ne)
    , s_(clean(s))
{
    BOOST_ASSERT(s_.data());
    rewind();
}

void
query_iter::
rewind() noexcept
{
    if(empty())
    {
        p_ = nullptr;
        return;
    }
    p_ = s_.begin();
    auto pos =
        s_.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s_.size();
}

bool
query_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(! p_)
        return false;
    n += encoded_size(
        string_view(p_, n_),
            {}, query_chars);
    increment();
    return true;
}

void
query_iter::
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
        query_chars);
    increment();
}

void
query_iter::
increment() noexcept
{
    p_ += n_;
    if(p_ == s_.end())
    {
        p_ = nullptr;
        return;
    }
    ++p_;
    string_view s(
        p_, s_.end() - p_);
    auto pos = s.find_first_of('&');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

//------------------------------------------------
//
// decode_query_iter
//
//------------------------------------------------

decode_query_iter::
decode_query_iter(
    decode_view s,
    not_empty_param ne) noexcept
    : any_params_iter(
        s.empty() && ! ne)
    , it0_(s.begin())
    , end_(s.end())
{
    rewind();
}

void
decode_query_iter::
rewind() noexcept
{
    if(empty())
    {
        at_end_ = true;
        return;
    }
    n_ = 0;
    it_ = it0_;
    at_end_ = false;
    auto it = it_;
    while(it != end_)
    {
        if(*it == '&')
            break;
        ++it;
        ++n_;
    }
}

bool
decode_query_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(at_end_)
        return false;
    auto it = it_;
    auto end = std::next(it_, n_);
    n += encoded_size_impl(
        it, end, {}, query_chars);
    increment();
    return true;
}

void
decode_query_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    BOOST_ASSERT(! at_end_);
    auto it = it_;
    auto last = std::next(it_, n_);
    dest += encode_impl(
        dest, end,
        it, last,
        {}, query_chars);
    increment();
}

void
decode_query_iter::
increment() noexcept
{
    std::advance(it_, n_);
    if(it_ == end_)
    {
        at_end_ = true;
        return;
    }
    ++it_;
    auto pos = it_;
    n_ = 0;
    while (pos != end_)
    {
        if (*pos == '&')
            break;
        ++pos;
        ++n_;
    }
}

//------------------------------------------------
//
// params_encoded_iter_base
//
//------------------------------------------------

bool
params_encoded_iter_base::
measure_impl(
    param_view const& v,
    std::size_t& n,
    error_code& ec) noexcept
{
    decode_opts opt;
    opt.plus_to_space = true;
    auto rv = detail::validate_encoding(
        v.key, opt, query_chars);
    if(! rv)
    {
        ec = rv.error();
        return false;
    }
    n += v.key.size();
    if(v.has_value)
    {
        rv = detail::validate_encoding(
            v.value, opt, query_chars);
        if(! rv)
        {
            ec = rv.error();
            return false;
        }
        n += 1 + v.value.size();
    }
    return true;
}

void
params_encoded_iter_base::
copy_impl(
    char*& dest,
    char const* end,
    param_view const& v) noexcept
{
    (void)end;
    {
        // avoid self-copy
        auto const kn = v.key.size();
        BOOST_ASSERT(end - kn >= dest);
        if( v.key.data() != dest &&
            kn > 0)
        {
            std::memcpy(
                dest,
                v.key.data(),
                kn);
        }
        dest += kn;
    }
    if(v.has_value)
    {
        BOOST_ASSERT(
            end - 1 >= dest);
        *dest++ = '=';
        auto const vn =
            v.value.size();
        BOOST_ASSERT(
            end - vn >= dest);
        if(vn > 0)
        {
            std::memcpy(
                dest,
                v.value.data(),
                vn);
            dest += vn;
        }
    }
}

//------------------------------------------------
//
// params_iter_base
//
//------------------------------------------------

void
params_iter_base::
measure_impl(
    param_view const& v,
    std::size_t& n) noexcept
{
    n += encoded_size(
        v.key, {},
        detail::param_key_chars);
    if(v.has_value)
    {
        ++n; // '='
        n += encoded_size(
            v.value, {},
            detail::param_value_chars);
    }
}

void
params_iter_base::
copy_impl(
    char*& dest,
    char const* end,
    param_view const& v) noexcept
{
    dest += encode(
        dest,
        end,
        v.key,
        {},
        detail::param_key_chars);
    if(v.has_value)
    {
        *dest++ = '=';
        dest += encode(
            dest,
            end,
            v.value,
            {},
            detail::param_value_chars);
    }
}

//------------------------------------------------

void
param_value_iter::
rewind() noexcept
{
    at_end_ = false;
}

bool
param_value_iter::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(at_end_)
        return false;
    n += nk_;
    if(has_value_)
    {
        ++n; // '='
        n += encoded_size(
            value_,
            {},
            detail::param_value_chars);
    }
    at_end_ = true;
    return true;
}

void
param_value_iter::
copy(char*& it, char const* end) noexcept
{
    // skip key
    it += nk_;
    if(! has_value_)
        return;
    *it++ = '=';
    it += encode(
        it, end, value_, {},
        detail::param_value_chars);
}

//------------------------------------------------

void
param_encoded_value_iter::
rewind() noexcept
{
    at_end_ = false;
}

bool
param_encoded_value_iter::
measure(
    std::size_t& n,
    error_code& ec) noexcept
{
    if(at_end_)
        return false;
    n += nk_;
    if(has_value_)
    {
        ++n; // '='
        // validate
        auto rv = urls::decode(
            value_, {}, 
            detail::param_value_chars);
        if(! rv)
        {
            ec = rv.error();
            return false;
        }
        n += value_.size();
    }
    at_end_ = true;
    return true;
}

void
param_encoded_value_iter::
copy(char*& it, char const* end) noexcept
{
    (void)end;
    BOOST_ASSERT(static_cast<
        std::size_t>(end - it) >=
            value_.size());
    // skip key
    it += nk_;
    if(! has_value_)
        return;
    *it++ = '=';
    std::memcpy(
        it,
        value_.data(),
        value_.size());
    it += value_.size();
}

//------------------------------------------------

bool
ci_decoded_key_equal(
    decode_view key,
    string_view match) noexcept
{
    if( key.size() !=
            match.size())
        return false;
    auto it0 = key.begin();
    auto it1 = match.begin();
    auto const end = match.end();
    while(it1 != end)
    {
        if( grammar::to_lower(*it0) !=
            grammar::to_lower(*it1))
            return false;
        ++it0;
        ++it1;
    }
    return true;
}

} // detail
} // urls
} // boost

#endif

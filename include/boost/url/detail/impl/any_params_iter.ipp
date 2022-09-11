//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
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
// query_iter
//
//------------------------------------------------

query_iter::
query_iter(
    string_view s,
    bool ne) noexcept
    : any_params_iter(
        s.empty() && ! ne, &s_)
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
    std::size_t& n) noexcept
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
    dest += encode_unchecked(
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
// params_iter
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
//
// params_encoded_iter
//
//------------------------------------------------

bool
params_encoded_iter_base::
measure_impl(
    param_pct_view const& v,
    std::size_t& n) noexcept
{
    encode_opts opt;
    opt.space_to_plus = true;
    n += detail::re_encoded_size_unchecked(
        v.key,
        opt,
        detail::param_key_chars);
    if(v.has_value)
        n += detail::re_encoded_size_unchecked(
            v.value,
            opt,
            detail::param_value_chars
                ) + 1; // for '='
    return true;
}

void
params_encoded_iter_base::
copy_impl(
    char*& dest,
    char const* end,
    param_view const& v) noexcept
{
    encode_opts opt;
    opt.space_to_plus = true;
    detail::re_encode_unchecked(
        dest,
        end,
        v.key,
        opt,
        detail::param_key_chars);
    if(v.has_value)
    {
        *dest++ = '=';
        detail::re_encode_unchecked(
            dest,
            end,
            v.value,
            opt,
            detail::param_value_chars);
    }
}

//------------------------------------------------
//
// param_value_iter
//
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
    std::size_t& n) noexcept
{
    if(at_end_)
        return false;
    n += nk_; // skip key
    if(has_value_)
    {
        encode_opts opt;
        opt.space_to_plus = true;
        n += encoded_size(
            value_,
            opt,
            detail::param_value_chars
                ) + 1; // for '='
    }
    at_end_ = true;
    return true;
}

void
param_value_iter::
copy(char*& it, char const* end) noexcept
{
    it += nk_; // skip key
    if(! has_value_)
        return;
    *it++ = '=';
    encode_opts opt;
    opt.space_to_plus = true;
    it += encode(
        it,
        end,
        value_,
        opt,
        detail::param_value_chars);
}

//------------------------------------------------
//
// param_encoded_value_iter
//
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
    std::size_t& n) noexcept
{
    if(at_end_)
        return false;
    n += nk_; // skip key
    if(has_value_)
    {
        encode_opts opt;
        opt.space_to_plus = true;
        n += detail::re_encoded_size_unchecked(
            value_,
            opt,
            detail::param_value_chars
                ) + 1; // for '='
    }
    at_end_ = true;
    return true;
}

void
param_encoded_value_iter::
copy(char*& dest, char const* end) noexcept
{
    dest += nk_; // skip key
    if(! has_value_)
        return;
    *dest++ = '=';
    encode_opts opt;
    opt.space_to_plus = true;
    detail::re_encode_unchecked(
        dest,
        end,
        value_,
        opt,
        detail::param_value_chars);
}

} // detail
} // urls
} // boost

#endif

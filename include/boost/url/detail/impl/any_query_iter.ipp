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
#include <boost/url/string.hpp>
#include <boost/url/rfc/charsets.hpp>

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
}

enc_query_iter::
enc_query_iter(
    string_view s) noexcept
    : end_(s.data() + s.size())
{
}

bool
enc_query_iter::
measure(
    std::size_t& n,
    error_code& ec) noexcept
{
    (void)n;
    (void)ec;
    return false;
}

void
enc_query_iter::
copy(
    char*& dest,
    char const* end) noexcept
{
    (void)dest;
    (void)end;
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
    static constexpr auto cs =
        pchars + '/' + '?';
    pct_decode_opts opt;
    opt.plus_to_space = true;
    validate_pct_encoding(
        key, ec, cs, opt);
    if(ec.failed())
        return false;
    n += key.size();
    if(value)
    {
        validate_pct_encoding(
            *value, ec, cs, opt);
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
    // avoid self-copy
    if( key.data() != dest &&
        key.data() != nullptr)
    {
        std::size_t n =
            key.size();
        BOOST_ASSERT(
            end - n >= dest);
        std::memcpy(dest,
            key.data(), n);
        dest += n;
    }
    if(value)
    {
        BOOST_ASSERT(
            end - 1 >= dest);
        *dest++ = '=';
        std::size_t n =
            value->size();
        BOOST_ASSERT(
            end - n >= dest);
        if(n > 0)
        {
            std::memcpy(dest,
                value->data(), n);
            dest += n;
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
    static constexpr auto cs =
        pchars + '/' + '?';
    n += pct_encode_bytes(key, cs);
    if(value)
    {
        ++n; // '='
        n += pct_encode_bytes(
            *value, cs);
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
    static constexpr auto cs =
        pchars + '/' + '?';
    dest += pct_encode(
        dest, end, key, cs);
    if(value)
    {
        *dest++ = '=';
        dest += pct_encode(
            dest, end, *value, cs);
    }
}

} // detail
} // urls
} // boost

#endif

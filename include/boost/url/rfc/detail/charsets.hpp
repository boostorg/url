//
// Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_CHARSETS_HPP
#define BOOST_URL_RFC_DETAIL_CHARSETS_HPP

#include <boost/url/rfc/charsets.hpp>

namespace boost {
namespace urls {
namespace detail {

struct user_chars_t : grammar::lut_chars
{
    constexpr
    user_chars_t() noexcept
        : grammar::lut_chars(
            unreserved_chars +
            subdelim_chars)
    {
    }
};

constexpr user_chars_t user_chars{};

struct password_chars_t : grammar::lut_chars
{
    constexpr
    password_chars_t() noexcept
        : grammar::lut_chars(
            unreserved_chars +
            subdelim_chars + ':')
    {
    }
};

constexpr password_chars_t password_chars{};

struct userinfo_chars_t : grammar::lut_chars
{
    constexpr
        userinfo_chars_t() noexcept
        : grammar::lut_chars(password_chars)
    {
    }
};

constexpr userinfo_chars_t userinfo_chars{};

struct host_chars_t : grammar::lut_chars
{
    constexpr
        host_chars_t() noexcept
        : grammar::lut_chars(
            unreserved_chars +
            subdelim_chars)
    {
    }
};

constexpr host_chars_t host_chars{};

struct reg_name_chars_t : grammar::lut_chars
{
    constexpr
        reg_name_chars_t() noexcept
        : grammar::lut_chars(
            unreserved_chars
            + '-' + '.')
    {
    }
};

constexpr reg_name_chars_t reg_name_chars{};

struct segment_chars_t : grammar::lut_chars
{
    constexpr
        segment_chars_t() noexcept
        : grammar::lut_chars(
            pchars)
    {
    }
};

constexpr segment_chars_t segment_chars{};

struct path_chars_t : grammar::lut_chars
{
    constexpr
        path_chars_t() noexcept
        : grammar::lut_chars(
            segment_chars + '/')
    {
    }
};

constexpr path_chars_t path_chars{};

} // detail
} // urls
} // boost

#endif

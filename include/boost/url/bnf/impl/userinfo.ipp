//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_USERINFO_IPP
#define BOOST_URL_BNF_IMPL_USERINFO_IPP

#include <boost/url/bnf/userinfo.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/algorithm.hpp>
#include <boost/url/bnf/char_sets.hpp>
#include <boost/url/bnf/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace bnf {

struct userinfo::uchar
{
    using value_type = void;

    value_type
    value() const noexcept
    {
        return;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec) noexcept
    {
        auto it = start;
        if(it == end)
        {
            ec = {};
            return start;
        }
        if(is_unreserved(*it))
        {
            ++it;
            return it;
        }
        if(is_sub_delims(*it))
        {
            ++it;
            return it;
        }
        if(*it == ':')
        {
            ++it;
            return it;
        }
        it = consume<pct_encoded>(
            it, end, ec);
        return it;
    }
};

struct userinfo::uchar_nc
{
    using value_type = void;

    value_type
    value() const noexcept
    {
        return;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec) noexcept
    {
        auto it = start;
        if(it == end)
        {
            ec = {};
            return start;
        }
        if(is_unreserved(*it))
        {
            ++it;
            return it;
        }
        if(is_sub_delims(*it))
        {
            ++it;
            return it;
        }
        it = consume<pct_encoded>(
            it, end, ec);
        return it;
    }
};

char const*
userinfo::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    auto it = consume<zero_or_more<
        uchar_nc>>(start, end, ec);
    if(ec)
        return start;
    if(it == end)
    {
        v_.userinfo_ = string_view(
            start, it - start);
        v_.user_ = v_.userinfo_;
        v_.password_ = {};
        return it;
    }
    if(*it != ':')
    {
        v_.userinfo_ = string_view(
            start, it - start);
        v_.user_ = v_.userinfo_;
        v_.password_ = {};
        return it;
    }
    ++it;
    auto it0 = it;
    it = consume<zero_or_more<
        uchar>>(it, end, ec);
    if(ec)
        return start;
    v_.user_ = string_view(
        start, it0 - 1 - start);
    v_.password_ = string_view(
        it0, it - it0);
    v_.userinfo_ = string_view(
        start, it - start);
    return it;
}

} // bnf
} // urls
} // boost

#endif

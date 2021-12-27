//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_BNF_IPP
#define BOOST_URL_IMPL_QUERY_BNF_IPP

#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/bnf/lut_chars.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/detail/pct_encoding.hpp>

namespace boost {
namespace urls {

struct query_bnf::key_chars
    : bnf::lut_chars
{
    constexpr
    key_chars() noexcept
        : bnf::lut_chars(
            pchars
            + '/' + '?'
            - '&' - '=')
    {
    }
};

struct query_bnf::value_chars
    : bnf::lut_chars
{
    constexpr
    value_chars() noexcept
        : bnf::lut_chars(
            pchars
            + '/' + '?'
            - '&')
    {
    }
};

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_bnf& t) noexcept
{
    std::size_t n = 1;
    auto const start = it;
    query_bnf::value_type::value_type v;

    if(! query_bnf::begin(
        it, end, ec, v))
        return false;
    for(;;)
    {
        if(! query_bnf::increment(
            it, end, ec, v))
        {
            if(ec != bnf::error::end)
                return false;
            ec = {};
            break;
        }
        ++n;
    }

    t.v = query_bnf::value_type(
        string_view(
            start, it - start),
        n,
        &query_bnf::begin,
        &query_bnf::increment);
    return true;
}

//------------------------------------------------

bool
query_bnf::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_param_view& t) noexcept
{
    using bnf::parse;
    static constexpr
        query_bnf::key_chars ks{};
    static constexpr
        query_bnf::value_chars vs{};
    pct_encoded_str s;

    // key
    if(! parse(it, end, ec,
        pct_encoded_bnf(ks, s)))
        return false;
    t.key = s.str;

    // "="
    t.has_value = parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! parse(it, end, ec,
            pct_encoded_bnf(vs, s)))
            return false;
        t.value = s.str;
    }
    else
    {
        // key with no value
        ec = {};
    }

    return true;
}

bool
query_bnf::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_param_view& t) noexcept
{
    using bnf::parse;
    static constexpr
        query_bnf::key_chars ks{};
    static constexpr
        query_bnf::value_chars vs{};
    pct_encoded_str s;

    if(! parse(it, end, ec, '&'))
    {
        // end of list
        ec = bnf::error::end;
        return false;
    }

    // key
    if(! parse(it, end, ec,
        pct_encoded_bnf(ks, s)))
        return false;
    t.key = s.str;

    // "="
    t.has_value = parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! parse(it, end, ec,
            pct_encoded_bnf(vs, s)))
            return false;
        t.value = s.str;
    }
    else
    {
        // key with no value
        ec = {};
    }

    return true;
}

//------------------------------------------------

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_part_bnf& t)
{
    using bnf::parse;
    auto start = it;
    if( it == end ||
        *it != '?')
    {
        ec = {};
        t.has_query = false;
        return true;
    }
    ++it;
    if(! parse(it, end, ec, t.query))
        return false;
    t.has_query = true;
    t.query_part = string_view(
        start, it - start);
    return true;
}

} // urls
} // boost

#endif

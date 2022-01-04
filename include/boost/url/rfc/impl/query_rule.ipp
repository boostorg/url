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

#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/detail/pct_encoding.hpp>

namespace boost {
namespace urls {

struct query_rule::key_chars
    : grammar::lut_chars
{
    constexpr
    key_chars() noexcept
        : grammar::lut_chars(
            pchars
            + '/' + '?'
            - '&' - '=')
    {
    }
};

struct query_rule::value_chars
    : grammar::lut_chars
{
    constexpr
    value_chars() noexcept
        : grammar::lut_chars(
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
    query_rule& t) noexcept
{
    std::size_t n = 1;
    auto const start = it;
    query_rule::value_type::value_type v;

    if(! query_rule::begin(
        it, end, ec, v))
        return false;
    for(;;)
    {
        if(! query_rule::increment(
            it, end, ec, v))
        {
            if(ec != grammar::error::end)
                return false;
            ec = {};
            break;
        }
        ++n;
    }

    t.v = query_rule::value_type(
        string_view(
            start, it - start),
        n,
        &query_rule::begin,
        &query_rule::increment);
    return true;
}

//------------------------------------------------

bool
query_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_param_view& t) noexcept
{
    using grammar::parse;
    static constexpr
        query_rule::key_chars ks{};
    static constexpr
        query_rule::value_chars vs{};
    pct_encoded_str s;

    // key
    if(! parse(it, end, ec,
        pct_encoded_rule(ks, s)))
        return false;
    t.key = s.str;

    // "="
    t.has_value = parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! parse(it, end, ec,
            pct_encoded_rule(vs, s)))
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
query_rule::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_param_view& t) noexcept
{
    using grammar::parse;
    static constexpr
        query_rule::key_chars ks{};
    static constexpr
        query_rule::value_chars vs{};
    pct_encoded_str s;

    if(! parse(it, end, ec, '&'))
    {
        // end of list
        ec = grammar::error::end;
        return false;
    }

    // key
    if(! parse(it, end, ec,
        pct_encoded_rule(ks, s)))
        return false;
    t.key = s.str;

    // "="
    t.has_value = parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! parse(it, end, ec,
            pct_encoded_rule(vs, s)))
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
    query_part_rule& t)
{
    using grammar::parse;
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

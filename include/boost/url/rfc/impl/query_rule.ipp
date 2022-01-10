//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_RULE_IPP
#define BOOST_URL_IMPL_QUERY_RULE_IPP

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

//------------------------------------------------

bool
query_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_param_view& t) noexcept
{
    pct_encoded_rule<
        query_rule::key_chars> t0;
    pct_encoded_rule<
        query_rule::value_chars> t1;

    // key
    if(! grammar::parse(it, end, ec, t0))
        return false;
    t.key = t0.s.str;

    // "="
    t.has_value = grammar::parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! grammar::parse(
            it, end, ec, t1))
            return false;
        t.value = t1.s.str;
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
    pct_encoded_rule<
        query_rule::key_chars> t0;
    pct_encoded_rule<
        query_rule::value_chars> t1;

    if(! grammar::parse(
        it, end, ec, '&'))
    {
        // end of list
        ec = grammar::error::end;
        return false;
    }

    // key
    if(! grammar::parse(
        it, end, ec, t0))
        return false;
    t.key = t0.s.str;

    // "="
    t.has_value = grammar::parse(
        it, end, ec, '=');
    if(t.has_value)
    {
        // value
        if(! grammar::parse(
                it, end, ec, t1))
            return false;
        t.value = t1.s.str;
    }
    else
    {
        // key with no value
        ec = {};
    }

    return true;
}

//------------------------------------------------

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    query_part_rule& t) noexcept
{
    auto start = it;
    if( it == end ||
        *it != '?')
    {
        ec = {};
        t.has_query = false;
        return;
    }
    ++it;
    if(! grammar::parse(
        it, end, ec, t.query))
        return;
    t.has_query = true;
    t.query_part = string_view(
        start, it - start);
}

} // urls
} // boost

#endif

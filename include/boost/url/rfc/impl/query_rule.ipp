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

namespace boost {
namespace urls {

static
constexpr
auto
key_chars = pchars
    + '/' + '?' + '[' + ']'
    - '&' - '=';

static
constexpr
auto
value_chars = pchars
    + '/' + '?'
    - '&';

auto
query_rule::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    return grammar::parse_range(
        it, end, *this,
        &query_rule::begin,
        &query_rule::increment);
}

auto
query_rule::
begin(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<query_param_view>
{
    // VFALCO we don't return error::end
    // here, because the empty string still
    // counts as a 1-element range with
    // key="" and value=(none)

    return parse_query_param(it, end);
}

auto
query_rule::
increment(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<query_param_view>
{
    // "&"
    {
        auto rv = grammar::parse(
            it, end,
            grammar::char_rule('&'));
        if(! rv)
        {
            // end of list
            return grammar::error::end;
        }
    }

    return parse_query_param(it, end);
}

auto
query_rule::
parse_query_param(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<query_param_view>
{
    query_param_view t;

    // key
    {
        auto rv = grammar::parse(it, end,
            pct_encoded_rule(key_chars));
        if(! rv)
            return rv.error();    
        t.key = *rv;
    }

    // "="
    {
        auto rv = grammar::parse(
            it, end,
            grammar::char_rule('='));
        t.has_value = rv.has_value();
        if(! t.has_value)
        {
            // key with no value
            return t;
        }
    }

    // value
    {
        auto rv = grammar::parse(it, end,
            pct_encoded_rule(value_chars));
        if(! rv)
            return rv.error();
        t.value = *rv;
    }

    return t;
}

} // urls
} // boost

#endif

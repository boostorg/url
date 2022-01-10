//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATHS_RULE_IPP
#define BOOST_URL_RFC_IMPL_PATHS_RULE_IPP

#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_rule const& t) noexcept
{
    grammar::parse(
        it, end, ec,
        pct_encoded_rule(pchars, t.v));
}

//------------------------------------------------

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_nz_rule const& t) noexcept
{
    auto const start = it;
    if(! grammar::parse(
        it, end, ec,
        pct_encoded_rule(pchars, t.v)))
        return;
    if(it == start)
    {
        // can't be empty
        ec = error::empty_path_segment;
        return;
    }
}

//------------------------------------------------

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_nz_nc_rule const& t) noexcept
{
    auto const start = it;
    static constexpr auto cs =
        pchars - ':';
    if(! grammar::parse(
        it, end, ec,
        pct_encoded_rule(cs, t.v)))
        return;
    if(it == start)
    {
        // can't be empty
        ec = error::empty_path_segment;
        return;
    }
}

//------------------------------------------------

bool
path_abempty_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    return increment(
        it, end, ec, t);
}

bool
path_abempty_rule::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    auto const start = it;
    if(grammar::parse(
        it, end, ec,
        '/', segment_rule{t}))
        return true;
    ec = BOOST_URL_ERR(
        grammar::error::end);
    it = start;
    return false;
}

//------------------------------------------------

bool
path_absolute_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    if(it == end)
    {
        // expected '/'
        ec = BOOST_URL_ERR(
            error::missing_path_segment);
        return false;
    }
    if(*it != '/')
    {
        // expected '/'
        ec = BOOST_URL_ERR(
            error::missing_path_separator);
        return false;
    }
    ++it;
    if(it == end)
        return true;
    if(*it == '/')
    {
        // can't begin with "//"
        ec = BOOST_URL_ERR(
            error::empty_path_segment);
        return false;
    }
    return grammar::parse(
        it, end, ec,
        segment_rule{t});
}

bool
path_absolute_rule::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    auto const start = it;
    if(grammar::parse(
        it, end, ec,
        '/', segment_rule{t}))
        return true;
    ec = BOOST_URL_ERR(
        grammar::error::end);
    it = start;
    return false;
}

//------------------------------------------------

bool
path_noscheme_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    if(grammar::parse(
        it, end, ec,
        segment_nz_nc_rule{t}))
        return true;
    // bad segment-nz-nc
    ec = error::bad_schemeless_path_segment;
    return false;
}

bool
path_noscheme_rule::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    auto const start = it;
    if(grammar::parse(
        it, end, ec,
        '/', segment_rule{t}))
        return true;
    ec = grammar::error::end;
    it = start;
    return false;
}


//------------------------------------------------

bool
path_rootless_rule::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    return grammar::parse(
        it, end, ec,
        segment_nz_rule{t});
}

bool
path_rootless_rule::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using grammar::parse;
    auto const start = it;
    if(grammar::parse(
        it, end, ec,
        '/', segment_rule{t}))
        return true;
    ec = grammar::error::end;
    it = start;
    return false;
}

} // urls
} // boost

#endif

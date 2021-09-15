//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATHS_BNF_IPP
#define BOOST_URL_RFC_IMPL_PATHS_BNF_IPP

#include <boost/url/rfc/paths_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_bnf const& t)
{
    using bnf::parse;
    return parse(it, end, ec,
        pct_encoded_bnf(
            masked_char_set<
                pchar_mask>{}, t.v));
}

//------------------------------------------------

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_nz_bnf const& t)
{
    using bnf::parse;
    auto const start = it;
    if(! parse(it, end, ec,
        pct_encoded_bnf(
            masked_char_set<
                pchar_mask>{}, t.v)))
        return false;
    if(it == start)
    {
        // can't be empty
        ec = error::empty_path_segment;
        return false;
    }
    return true;
}

//------------------------------------------------

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    segment_nz_nc_bnf const& t)
{
    using bnf::parse;
    auto const start = it;
    if(! parse(it, end, ec,
        pct_encoded_bnf(
            masked_char_set<
                pchar_mask &
                ~colon_char_mask>{}, t.v)))
        return false;
    if(it == start)
    {
        // can't be empty
        ec = error::empty_path_segment;
        return false;
    }
    return true;
}

//------------------------------------------------

bool
path_abempty_bnf::
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
path_abempty_bnf::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    auto const start = it;
    if(parse(it, end, ec,
        '/', segment_bnf{t}))
        return true;
    ec = error::end;
    it = start;
    return false;
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    path_abempty_bnf const& t)
{
    bnf::range<pct_encoded_str> r;
    if(! bnf::parse_range(
        it, end, ec, r, t))
        return false;
    t.v.path = r.str();
    t.v.count = r.size();
    return true;
}

//------------------------------------------------

bool
path_absolute_bnf::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    if(it == end)
    {
        // expected '/'
        ec = error::missing_path_segment;
        return false;
    }
    if(*it != '/')
    {
        // expected '/'
        ec = error::missing_path_separator;
        return false;
    }
    ++it;
    if(it == end)
        return true;
    if(*it == '/')
    {
        // can't be empty
        ec = error::empty_path_segment;
        return false;
    }
    return parse(it, end, ec,
        segment_nz_bnf{t});
}

bool
path_absolute_bnf::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    auto const start = it;
    if(parse(it, end, ec,
        '/', segment_bnf{t}))
        return true;
    ec = error::end;
    it = start;
    return false;
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    path_absolute_bnf const& t)
{
    bnf::range<pct_encoded_str> r;
    if(! bnf::parse_range(
            it, end, ec, r, t))
        return false;
    t.v.path = r.str();
    t.v.count = r.size();
    return true;
}

//------------------------------------------------

bool
path_noscheme_bnf::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    if(parse(it, end, ec,
        segment_nz_nc_bnf{t}))
        return true;
    // bad segment-nz-nc
    ec = error::bad_schemeless_path_segment;
    return false;
}

bool
path_noscheme_bnf::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    auto const start = it;
    if(parse(it, end, ec,
        '/', segment_bnf{t}))
        return true;
    ec = error::end;
    it = start;
    return false;
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    path_noscheme_bnf const& t)
{
    bnf::range<pct_encoded_str> r;
    if(! bnf::parse_range(
            it, end, ec, r, t))
        return false;
    t.v.path = r.str();
    t.v.count = r.size();
    return true;
}

//------------------------------------------------

bool
path_rootless_bnf::
begin(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    return parse(it, end, ec,
        segment_nz_bnf{t});
}

bool
path_rootless_bnf::
increment(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_str& t) noexcept
{
    using bnf::parse;
    auto const start = it;
    if(parse(it, end, ec,
        '/', segment_bnf{t}))
        return true;
    ec = error::end;
    it = start;
    return false;
}

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    path_rootless_bnf const& t)
{
    bnf::range<pct_encoded_str> r;
    if(! bnf::parse_range(
            it, end, ec, r, t))
        return false;
    t.v.path = r.str();
    t.v.count = r.size();
    return true;
}

} // urls
} // boost

#endif

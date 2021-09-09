//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PATHS_HPP
#define BOOST_URL_DETAIL_PATHS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {
namespace detail {

struct segment_bnf
{
    pct_encoded_str& v;

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_bnf const& t)
    {
        using bnf::parse;
        return parse(it, end, ec,
            pct_encoded_bnf<
                masked_char_set<
                    pchar_mask>>{t.v});
    }
};

//------------------------------------------------

struct segment_nz_bnf
{
    pct_encoded_str& v;

    friend
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
            pct_encoded_bnf<
                masked_char_set<
                pchar_mask>>{t.v}))
            return false;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return false;
        }
        return true;
    }
};

//------------------------------------------------

struct segment_nz_nc_bnf
{
    pct_encoded_str& v;

    friend
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
            pct_encoded_bnf<
                masked_char_set<
                    pchar_mask &
                    ~colon_char_mask>>{t.v}))
            return false;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return false;
        }
        return true;
    }
};

//------------------------------------------------

// path-abempty  = *( "/" segment )
struct path_abempty_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        return increment(
            it, end, ec, t);
    }

    static
    bool
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

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_abempty_bnf const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
struct path_absolute_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    static
    bool
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
            ec = error::syntax;
            return false;
        }
        if(*it != '/')
        {
            // expected '/'
            ec = error::syntax;
            return false;
        }
        ++it;
        if(it == end)
            return true;
        if(*it == '/')
        {
            // bad empty segment
            ec = error::syntax;
            return false;
        }
        return parse(it, end, ec,
            segment_nz_bnf{t});
    }

    static
    bool
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

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_absolute_bnf const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-noscheme = segment-nz-nc *( "/" segment )
struct path_noscheme_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    static
    bool
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
        // expected segment-nz-nc
        ec = error::syntax;
        return false;
    }

    static
    bool
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

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_noscheme_bnf const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-rootless = segment-nz *( "/" segment )
struct path_rootless_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    static
    bool
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

    static
    bool
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

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_rootless_bnf const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

} // detail
} // urls
} // boost

#endif

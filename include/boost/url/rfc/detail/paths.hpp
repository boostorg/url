//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_PATHS_HPP
#define BOOST_URL_RFC_DETAIL_PATHS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {
namespace detail {

struct segment
{
    pct_encoded_str& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment const& t)
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            pct_encoded<
                pchar_mask>{t.v});
        if(ec)
            return start;
        return it;
    }
};

//------------------------------------------------

struct segment_nz
{
    pct_encoded_str& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment_nz const& t)
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            pct_encoded<
                pchar_mask>{t.v});
        if(ec)
            return start;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return start;
        }
        return it;
    }
};

//------------------------------------------------

struct segment_nz_nc
{
    pct_encoded_str& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment_nz_nc const& t)
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            pct_encoded<
                pchar_mask &
                ~colon_char_mask>{t.v});
        if(ec)
            return start;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return start;
        }
        return it;
    }
};

//------------------------------------------------

// path-abempty  = *( "/" segment )
struct path_abempty
{
    bnf::range<
        pct_encoded_str>& v;

    static
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        return increment(
            start, end, ec, t);
    }

    static
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            '/', segment{t});
        if(! ec.failed())
            return it;
        // VFALCO Check for unrecoverable error
        //if(ec == condition::fatal)
        //  return start;
        ec = error::end;
        return start;
    }

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_abempty const& t)
    {
        return bnf::parse_range(
            start, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
struct path_absolute
{
    bnf::range<
        pct_encoded_str>& v;

    static
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            '/', segment_nz{t});
        if(! ec.failed())
            return it;
        // VFALCO Check for unrecoverable error
        //if(ec == condition::fatal)
        //  return start;
        ec = error::end;
        return start;
    }

    static
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            '/', segment{t});
        if(! ec.failed())
            return it;
        // VFALCO Check for unrecoverable error
        //if(ec == condition::fatal)
        //  return start;
        ec = error::end;
        return start;
    }

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_absolute const& t)
    {
        return bnf::parse_range(
            start, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-noscheme = segment-nz-nc *( "/" segment )
struct path_noscheme
{
    bnf::range<
        pct_encoded_str>& v;

    static
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            segment_nz_nc{t});
        if(! ec.failed())
            return it;
        // VFALCO Check for unrecoverable error
        //if(ec == condition::fatal)
        //  return start;
        ec = error::end;
        return start;
    }

    static
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            '/', segment{t});
        if(! ec.failed())
            return it;
        // VFALCO Check for unrecoverable error
        //if(ec == condition::fatal)
        //  return start;
        ec = error::end;
        return start;
    }

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_noscheme const& t)
    {
        return bnf::parse_range(
            start, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-rootless = segment-nz *( "/" segment )
struct path_rootless
{
    bnf::range<
        pct_encoded_str>& v;

    static
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        return parse(
            start, end, ec,
            segment_nz{t});
    }

    static
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept
    {
        using bnf::parse;
        auto it = parse(
            start, end, ec,
            '/', segment{t});
        if(ec)
        {
            ec = error::end;
            return start;
        }
        return it;
    }

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_rootless const& t)
    {
        return bnf::parse_range(
            start, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-empty    = 0<pchar>
struct path_empty
{
    bnf::range<
        pct_encoded_str>& v;

    static
    char const*
    begin(
        char const* const start,
        char const* const,
        error_code& ec,
        pct_encoded_str&) noexcept
    {
        ec = error::end;
        return start;
    }

    static
    char const*
    increment(
        char const* const start,
        char const* const,
        error_code& ec,
        pct_encoded_str&) noexcept
    {
        // should never get here
        ec = error::end;
        return start;
    }

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_empty const& t)
    {
        return bnf::parse_range(
            start, end, ec, t.v, t);
    }
};

} // detail
} // rfc
} // urls
} // boost

#endif

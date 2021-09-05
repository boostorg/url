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
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment const& t)
    {
        using bnf::parse;
        return parse(it, end, ec,
            pct_encoded<
                pchar_mask>{t.v});
    }
};

//------------------------------------------------

struct segment_nz
{
    pct_encoded_str& v;

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz const& t)
    {
        using bnf::parse;
        auto const start = it;
        if(! parse(it, end, ec,
            pct_encoded<
                pchar_mask>{t.v}))
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

struct segment_nz_nc
{
    pct_encoded_str& v;

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz_nc const& t)
    {
        using bnf::parse;
        auto const start = it;
        if(! parse(it, end, ec,
            pct_encoded<
                pchar_mask &
                ~colon_char_mask>{t.v}))
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
struct path_abempty
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
            '/', segment{t}))
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
        path_abempty const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
struct path_absolute
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
        auto const start = it;
        if(parse(it, end, ec,
            '/', segment_nz{t}))
            return true;
        ec = error::end;
        it = start;
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
            '/', segment{t}))
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
        path_absolute const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-noscheme = segment-nz-nc *( "/" segment )
struct path_noscheme
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
        auto const start = it;
        if(parse(it, end, ec,
            segment_nz_nc{t}))
            return true;
        ec = error::end;
        it = start;
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
            '/', segment{t}))
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
        path_noscheme const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-rootless = segment-nz *( "/" segment )
struct path_rootless
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
            segment_nz{t});
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
            '/', segment{t}))
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
        path_rootless const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

//------------------------------------------------

// path-empty    = 0<pchar>
struct path_empty
{
    bnf::range<
        pct_encoded_str>& v;

    static
    bool
    begin(
        char const*&,
        char const*,
        error_code& ec,
        pct_encoded_str&) noexcept
    {
        ec = error::end;
        return false;
    }

    static
    bool
    increment(
        char const*&,
        char const*,
        error_code& ec,
        pct_encoded_str&) noexcept
    {
        // should never get here
        ec = error::end;
        return false;
    }

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_empty const& t)
    {
        return bnf::parse_range(
            it, end, ec, t.v, t);
    }
};

} // detail
} // rfc
} // urls
} // boost

#endif

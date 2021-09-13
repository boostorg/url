//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATHS_BNF_HPP
#define BOOST_URL_RFC_PATHS_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/range.hpp>
#include <boost/url/pct_encoding_types.hpp>

namespace boost {
namespace urls {

struct segment_bnf
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_bnf const& t);
};

//------------------------------------------------

struct segment_nz_bnf
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz_bnf const& t);
};

//------------------------------------------------

struct segment_nz_nc_bnf
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz_nc_bnf const& t);
};

//------------------------------------------------

// path-abempty  = *( "/" segment )
struct path_abempty_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_abempty_bnf const& t);
};

//------------------------------------------------

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
struct path_absolute_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_absolute_bnf const& t);
};

//------------------------------------------------

// path-noscheme = segment-nz-nc *( "/" segment )
struct path_noscheme_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_noscheme_bnf const& t);
};

//------------------------------------------------

// path-rootless = segment-nz *( "/" segment )
struct path_rootless_bnf
{
    bnf::range<
        pct_encoded_str>& v;

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        path_rootless_bnf const& t);
};

} // urls
} // boost

#endif

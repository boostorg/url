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
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/bnf/range.hpp>
#include <cstddef>

namespace boost {
namespace urls {

/** Information about a parsed path
*/
struct parsed_path
{
    /** The encoded string representing the path
    */
    string_view path;

    /** The number of segments in the path
    */
    std::size_t count = 0;
};

//------------------------------------------------

/** BNF for segment

    @par BNF
    @code
    segment       = *pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_absolute_bnf,
        @ref path_noscheme_bnf,
        @ref path_rootless_bnf,
        @ref pchars,
        @ref segment_nz_bnf,
        @ref segment_nz_nc_bnf
*/
#ifdef BOOST_URL_DOCS
using segment_bnf = __see_below__;
#else
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
#endif

//------------------------------------------------

/** BNF for segment-nz

    @par BNF
    @code
    segment-nz    = 1*pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_absolute_bnf,
        @ref path_noscheme_bnf,
        @ref path_rootless_bnf,
        @ref pchars,
        @ref segment_bnf,
        @ref segment_nz_nc_bnf
*/
#ifdef BOOST_URL_DOCS
using segment_nz_bnf = __see_below__;
#else
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
#endif

//------------------------------------------------

/** BNF for segment_nz_nc

    @par BNF
    @code
    segment-nz-nc   = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_absolute_bnf,
        @ref path_noscheme_bnf,
        @ref path_rootless_bnf,
        @ref pchars,
        @ref segment_bnf,
        @ref segment_nz_bnf,
        @ref subdelim_chars,
        @ref unreserved_chars
*/
#ifdef BOOST_URL_DOCS
using segment_nz_nc_bnf = __see_below__;
#else
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
#endif

//------------------------------------------------

/** BNF for path-abempty grammar

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_absolute_bnf,
        @ref path_noscheme_bnf,
        @ref path_rootless_bnf,
        @ref segment_bnf
*/
#ifdef BOOST_URL_DOCS
using path_abempty_bnf = __see_below__;
#else
struct path_abempty_bnf : bnf::range_
{
    using value_type =
        pct_encoded_str;

    path_abempty_bnf()
        : bnf::range_(this)
    {
    }

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* end,
        error_code& ec,
        pct_encoded_str& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* end,
        error_code& ec,
        pct_encoded_str& t) noexcept;
};
#endif

//------------------------------------------------

/** BNF for path-absolute grammar.

    @par BNF
    @code
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_noscheme_bnf,
        @ref path_rootless_bnf,
        @ref segment_bnf,
        @ref segment_nz_bnf
*/
#ifdef BOOST_URL_DOCS
using path_absolute_bnf = __see_below__;
#else
struct path_absolute_bnf : bnf::range_
{
    using value_type =
        pct_encoded_str;

    path_absolute_bnf()
        : bnf::range_(this)
    {
    }

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
};
#endif

//------------------------------------------------

/** BNF for path-noscheme grammar.

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_absolute_bnf,
        @ref path_rootless_bnf,
        @ref segment_bnf,
        @ref segment_nz_nc_bnf
*/
#ifdef BOOST_URL_DOCS
using path_noscheme_bnf = __see_below__;
#else
struct path_noscheme_bnf : bnf::range_
{
    using value_type =
        pct_encoded_str;

    path_noscheme_bnf()
        : bnf::range_(this)
    {
    }

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
};
#endif

//------------------------------------------------

/** BNF for path-rootless grammar.

    @par Example
    @code
    using bnf::parse;
    path_rootless_bnf t;
    bool success = parse( it, end, ec, t);
    @endcode

    @par BNF
    @code
    path-rootless = segment-nz *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_bnf,
        @ref path_absolute_bnf,
        @ref path_noscheme_bnf,
        @ref segment_bnf,
        @ref segment_nz_nc_bnf
*/
#ifdef BOOST_URL_DOCS
using path_rootless_bnf = __see_below__;
#else
struct path_rootless_bnf : bnf::range_
{
    using value_type =
        pct_encoded_str;

    path_rootless_bnf()
        : bnf::range_(this)
    {
    }

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
};
#endif

} // urls
} // boost

#endif

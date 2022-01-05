//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATHS_RULE_HPP
#define BOOST_URL_RFC_PATHS_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/grammar/range.hpp>
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
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_nz_rule,
        @ref segment_nz_nc_rule
*/
#ifdef BOOST_URL_DOCS
using segment_rule = __see_below__;
#else
struct segment_rule
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_rule const& t);
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
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_rule,
        @ref segment_nz_nc_rule
*/
#ifdef BOOST_URL_DOCS
using segment_nz_rule = __see_below__;
#else
struct segment_nz_rule
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz_rule const& t);
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
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_rule,
        @ref segment_nz_rule,
        @ref subdelim_chars,
        @ref unreserved_chars
*/
#ifdef BOOST_URL_DOCS
using segment_nz_nc_rule = __see_below__;
#else
struct segment_nz_nc_rule
{
    pct_encoded_str& v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        segment_nz_nc_rule const& t);
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
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref segment_rule
*/
#ifdef BOOST_URL_DOCS
using path_abempty_rule = __see_below__;
#else
struct path_abempty_rule : grammar::range_
{
    using value_type =
        pct_encoded_str;

    path_abempty_rule()
        : grammar::range_(this)
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
        @ref path_abempty_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref segment_rule,
        @ref segment_nz_rule
*/
#ifdef BOOST_URL_DOCS
using path_absolute_rule = __see_below__;
#else
struct path_absolute_rule : grammar::range_
{
    using value_type =
        pct_encoded_str;

    path_absolute_rule()
        : grammar::range_(this)
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
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_rootless_rule,
        @ref segment_rule,
        @ref segment_nz_nc_rule
*/
#ifdef BOOST_URL_DOCS
using path_noscheme_rule = __see_below__;
#else
struct path_noscheme_rule : grammar::range_
{
    using value_type =
        pct_encoded_str;

    path_noscheme_rule()
        : grammar::range_(this)
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
    using grammar::parse;
    path_rootless_rule t;
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
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref segment_rule,
        @ref segment_nz_nc_rule
*/
#ifdef BOOST_URL_DOCS
using path_rootless_rule = __see_below__;
#else
struct path_rootless_rule : grammar::range_
{
    using value_type =
        pct_encoded_str;

    path_rootless_rule()
        : grammar::range_(this)
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
